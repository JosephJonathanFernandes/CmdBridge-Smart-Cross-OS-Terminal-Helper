#include "test_framework.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

int test_pass_count = 0;
int test_fail_count = 0;

void setup_test_sandbox() {
    // Ensure it's clean before starting
    teardown_test_sandbox();
    
#ifdef _WIN32
    CreateDirectoryA("temp_test", NULL);
#else
    mkdir("temp_test", 0777);
#endif
}

void teardown_test_sandbox() {
#ifdef _WIN32
    system("cmd.exe /c rmdir /s /q temp_test >nul 2>&1");
#else
    system("rm -rf temp_test");
#endif
}
