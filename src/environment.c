#include "environment.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#else
#include <unistd.h>
#endif

// Helper to lowercase a string
static void to_lowercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

int detect_environment(EnvironmentInfo* env, const char* cli_shell_override) {
    if (!env) return 0;
    
    // Set OS
#ifdef _WIN32
    strcpy(env->os, "windows");
#elif __APPLE__
    strcpy(env->os, "macos");
#else
    strcpy(env->os, "linux");
#endif

    // 1. CLI Override
    if (cli_shell_override && strlen(cli_shell_override) > 0) {
        strncpy(env->shell, cli_shell_override, sizeof(env->shell)-1);
        env->shell[sizeof(env->shell)-1] = '\0';
        to_lowercase(env->shell);
        env->confidence = 1.0f;
        return 1;
    }

    // 2. Parent Process Detection
#ifdef _WIN32
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        DWORD myPid = GetCurrentProcessId();
        DWORD parentPid = 0;
        
        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (pe32.th32ProcessID == myPid) {
                    parentPid = pe32.th32ParentProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        
        if (parentPid != 0 && Process32First(hSnapshot, &pe32)) {
            do {
                if (pe32.th32ProcessID == parentPid) {
                    char parent_name[256];
                    strncpy(parent_name, pe32.szExeFile, sizeof(parent_name)-1);
                    parent_name[sizeof(parent_name)-1] = '\0';
                    to_lowercase(parent_name);
                    
                    if (strstr(parent_name, "powershell") || strstr(parent_name, "pwsh")) {
                        strcpy(env->shell, "powershell");
                        env->confidence = 0.9f;
                        CloseHandle(hSnapshot);
                        return 1;
                    }
                    if (strstr(parent_name, "cmd")) {
                        strcpy(env->shell, "cmd");
                        env->confidence = 0.9f;
                        CloseHandle(hSnapshot);
                        return 1;
                    }
                    if (strstr(parent_name, "bash")) {
                        strcpy(env->shell, "bash");
                        env->confidence = 0.9f;
                        CloseHandle(hSnapshot);
                        return 1;
                    }
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
#else
    // On Linux, read /proc/getppid()/comm
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/comm", getppid());
    FILE* f = fopen(path, "r");
    if (f) {
        char comm[256] = {0};
        if (fgets(comm, sizeof(comm), f)) {
            comm[strcspn(comm, "\r\n")] = '\0';
            to_lowercase(comm);
            if (strstr(comm, "bash")) {
                strcpy(env->shell, "bash");
                env->confidence = 0.9f;
                fclose(f);
                return 1;
            }
            if (strstr(comm, "zsh")) {
                strcpy(env->shell, "zsh");
                env->confidence = 0.9f;
                fclose(f);
                return 1;
            }
            if (strstr(comm, "fish")) {
                strcpy(env->shell, "fish");
                env->confidence = 0.9f;
                fclose(f);
                return 1;
            }
        }
        fclose(f);
    }
#endif

    // 3. Environment Heuristics
#ifdef _WIN32
    if (getenv("PSModulePath") != NULL) {
        strcpy(env->shell, "powershell");
        env->confidence = 0.8f;
        return 1;
    }
    if (getenv("PROMPT") != NULL) {
        strcpy(env->shell, "cmd");
        env->confidence = 0.6f;
        return 1;
    }
#else
    if (getenv("BASH_VERSION") != NULL || (getenv("SHELL") && strstr(getenv("SHELL"), "bash"))) {
        strcpy(env->shell, "bash");
        env->confidence = 0.8f;
        return 1;
    }
    if (getenv("ZSH_VERSION") != NULL || (getenv("SHELL") && strstr(getenv("SHELL"), "zsh"))) {
        strcpy(env->shell, "zsh");
        env->confidence = 0.8f;
        return 1;
    }
    if (getenv("SHELL") && strstr(getenv("SHELL"), "fish")) {
        strcpy(env->shell, "fish");
        env->confidence = 0.8f;
        return 1;
    }
#endif

    // 4. Safe Fallback
    strcpy(env->shell, "unknown");
    env->confidence = 0.2f;
    return 1;
}
