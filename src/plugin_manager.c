#include "plugin_manager.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <dirent.h>
#endif

#define MAX_PLUGINS 32
static LoadedPlugin g_plugins[MAX_PLUGINS];
static int g_plugin_count = 0;

typedef struct {
    char filename[256];
    char error_reason[256];
} FailedPlugin;
static FailedPlugin g_failed_plugins[MAX_PLUGINS];
static int g_failed_plugin_count = 0;

extern CB_HostAPI g_host_api;

typedef CB_Plugin* (*RegisterPluginFunc)(uint32_t);

bool plugin_manager_init(const char* plugins_dir) {
    g_plugin_count = 0;
    g_failed_plugin_count = 0;
    
#ifdef _WIN32
    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*.dll", plugins_dir);
    
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFileA(search_path, &fd);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (g_plugin_count >= MAX_PLUGINS) break;
            
            char dll_path[MAX_PATH];
            snprintf(dll_path, sizeof(dll_path), "%s\\%s", plugins_dir, fd.cFileName);
            
            HMODULE handle = LoadLibraryA(dll_path);
            if (handle) {
                RegisterPluginFunc reg_func = (RegisterPluginFunc)GetProcAddress(handle, "cb_register_plugin");
                if (reg_func) {
                    CB_Plugin* plugin = reg_func(CMDBRIDGE_API_VERSION);
                    if (plugin) {
                            if (plugin->struct_size != sizeof(CB_Plugin)) {
                                log_msg(LOG_WARN, "Rejected plugin: %s (Struct size mismatch)", fd.cFileName);
                                if (g_failed_plugin_count < MAX_PLUGINS) {
                                    strncpy(g_failed_plugins[g_failed_plugin_count].filename, fd.cFileName, 255);
                                    strncpy(g_failed_plugins[g_failed_plugin_count].error_reason, "Struct size mismatch", 255);
                                    g_failed_plugin_count++;
                                }
                                FreeLibrary(handle);
                                continue;
                            }
                            
                            g_plugins[g_plugin_count].os_handle = handle;
                            g_plugins[g_plugin_count].plugin = plugin;
                            g_plugins[g_plugin_count].disabled = false;
                            
                            if (plugin->init) {
                                if (plugin->init(NULL, &g_host_api) != 0) {
                                    log_msg(LOG_ERROR, "Plugin init failed: %s", plugin->name);
                                    g_plugins[g_plugin_count].disabled = true;
                                    
                                    if (g_failed_plugin_count < MAX_PLUGINS) {
                                        strncpy(g_failed_plugins[g_failed_plugin_count].filename, fd.cFileName, 255);
                                        strncpy(g_failed_plugins[g_failed_plugin_count].error_reason, "Initialization failed", 255);
                                        g_failed_plugin_count++;
                                    }
                                }
                            }
                            
                            log_msg(LOG_INFO, "Loaded plugin: %s (v%s)", plugin->name, plugin->version);
                            g_plugin_count++;
                        } else {
                            log_msg(LOG_WARN, "Rejected plugin: %s (API mismatch or refused)", fd.cFileName);
                            if (g_failed_plugin_count < MAX_PLUGINS) {
                                strncpy(g_failed_plugins[g_failed_plugin_count].filename, fd.cFileName, 255);
                                strncpy(g_failed_plugins[g_failed_plugin_count].error_reason, "API version mismatch or refused", 255);
                                g_failed_plugin_count++;
                            }
                            FreeLibrary(handle);
                        }
                    } else {
                        log_msg(LOG_WARN, "Rejected plugin: %s (Missing cb_register_plugin)", fd.cFileName);
                        if (g_failed_plugin_count < MAX_PLUGINS) {
                            strncpy(g_failed_plugins[g_failed_plugin_count].filename, fd.cFileName, 255);
                            strncpy(g_failed_plugins[g_failed_plugin_count].error_reason, "Missing cb_register_plugin export", 255);
                            g_failed_plugin_count++;
                        }
                        FreeLibrary(handle);
                    }
                } else {
                    log_msg(LOG_WARN, "Failed to load DLL: %s", dll_path);
                    if (g_failed_plugin_count < MAX_PLUGINS) {
                        strncpy(g_failed_plugins[g_failed_plugin_count].filename, fd.cFileName, 255);
                        strncpy(g_failed_plugins[g_failed_plugin_count].error_reason, "Failed to load shared library", 255);
                        g_failed_plugin_count++;
                    }
                }
        } while (FindNextFileA(hFind, &fd));
        FindClose(hFind);
    }
#else
    DIR* dir = opendir(plugins_dir);
    if (dir) {
        struct dirent* ent;
        while ((ent = readdir(dir)) != NULL) {
            if (g_plugin_count >= MAX_PLUGINS) break;
            
            if (strstr(ent->d_name, ".so") != NULL || strstr(ent->d_name, ".dylib") != NULL) {
                char so_path[1024];
                snprintf(so_path, sizeof(so_path), "%s/%s", plugins_dir, ent->d_name);
                
                void* handle = dlopen(so_path, RTLD_LAZY);
                if (handle) {
                    RegisterPluginFunc reg_func = (RegisterPluginFunc)dlsym(handle, "cb_register_plugin");
                    if (reg_func) {
                        CB_Plugin* plugin = reg_func(CMDBRIDGE_API_VERSION);
                        if (plugin) {
                            if (plugin->struct_size != sizeof(CB_Plugin)) {
                                log_msg(LOG_WARN, "Rejected plugin: %s (Struct size mismatch)", ent->d_name);
                                dlclose(handle);
                                continue;
                            }
                            
                            g_plugins[g_plugin_count].os_handle = handle;
                            g_plugins[g_plugin_count].plugin = plugin;
                            g_plugins[g_plugin_count].disabled = false;
                            
                            if (plugin->init) {
                                if (plugin->init(NULL, &g_host_api) != 0) {
                                    log_msg(LOG_ERROR, "Plugin init failed: %s", plugin->name);
                                    g_plugins[g_plugin_count].disabled = true;
                                }
                            }
                            
                            log_msg(LOG_INFO, "Loaded plugin: %s (v%s)", plugin->name, plugin->version);
                            g_plugin_count++;
                        } else {
                            log_msg(LOG_WARN, "Rejected plugin: %s (API mismatch or refused)", ent->d_name);
                            dlclose(handle);
                        }
                    } else {
                        log_msg(LOG_WARN, "Rejected plugin: %s (Missing cb_register_plugin)", ent->d_name);
                        dlclose(handle);
                    }
                } else {
                    log_msg(LOG_WARN, "Failed to load SO: %s (%s)", so_path, dlerror());
                }
            }
        }
        closedir(dir);
    }
#endif

    return true;
}

void plugin_manager_cleanup(void) {
    for (int i = 0; i < g_plugin_count; i++) {
        if (g_plugins[i].plugin && g_plugins[i].plugin->cleanup) {
            g_plugins[i].plugin->cleanup(NULL);
        }
#ifdef _WIN32
        if (g_plugins[i].os_handle) FreeLibrary((HMODULE)g_plugins[i].os_handle);
#else
        if (g_plugins[i].os_handle) dlclose(g_plugins[i].os_handle);
#endif
    }
    g_plugin_count = 0;
}

CB_Plugin* plugin_manager_get_claiming_plugin(const char* raw_input) {
    for (int i = 0; i < g_plugin_count; i++) {
        if (g_plugins[i].disabled) continue;
        
        CB_Plugin* p = g_plugins[i].plugin;
        if (p && p->claims_command) {
            if (p->claims_command(NULL, raw_input)) {
                return p;
            }
        }
    }
    return NULL;
}

void plugin_manager_print_registry(void) {
    printf("Loaded Plugins\n\n");
    if (g_plugin_count == 0) {
        printf("No plugins loaded.\n");
        return;
    }
    
    for (int i = 0; i < g_plugin_count; i++) {
        CB_Plugin* p = g_plugins[i].plugin;
        printf("%s\n\n", p->name);
        printf("Version:\n%s\n\n", p->version);
        printf("API:\n%d\n\n", CMDBRIDGE_API_VERSION); // We only load v1 right now
        
        printf("Claims:\n");
        if (p->num_claims > 0 && p->claims) {
            for (uint32_t c = 0; c < p->num_claims; c++) {
                printf("- %s (priority: %u)\n", p->claims[c].name, p->claims[c].priority);
            }
        } else {
            printf("None\n");
        }
        printf("\n");
        
        printf("Status:\n%s\n\n", g_plugins[i].disabled ? "Disabled" : "Loaded");
        printf("--------------------\n\n");
    }
    
    if (g_failed_plugin_count > 0) {
        printf("Failed Plugins\n\n");
        for (int i = 0; i < g_failed_plugin_count; i++) {
            printf("%s\n\n", g_failed_plugins[i].filename);
            printf("Reason:\n%s\n\n", g_failed_plugins[i].error_reason);
            printf("Status:\nRejected\n\n");
            printf("--------------------\n\n");
        }
    }
}
