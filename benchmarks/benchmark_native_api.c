#include "benchmark_utils.h"
#include "../src/native_api/file_ops.h"
#include "../src/intent.h"

#define ITERATIONS 100

int main() {
    double *times_cmdbridge = malloc(ITERATIONS * sizeof(double));
    double *times_os = malloc(ITERATIONS * sizeof(double));

    for (int i = 0; i < ITERATIONS; i++) {
        // Measure CmdBridge API overhead
        Intent intent;
        strcpy(intent.action, "create");
        strcpy(intent.object, "folders");
        strcpy(intent.args[0], "test_benchmark_folder_1");
        intent.argc = 1;

        double start_cmdbridge = get_time_ms();
        execute_native_api(&intent); // CmdBridge wrapper
        double end_cmdbridge = get_time_ms();
        times_cmdbridge[i] = end_cmdbridge - start_cmdbridge;

        // Measure raw OS API
        double start_os = get_time_ms();
#ifdef _WIN32
        CreateDirectoryA("test_benchmark_folder_2", NULL);
#else
        mkdir("test_benchmark_folder_2", 0755);
#endif
        double end_os = get_time_ms();
        times_os[i] = end_os - start_os;
        
        // Cleanup
#ifdef _WIN32
        RemoveDirectoryA("test_benchmark_folder_1");
        RemoveDirectoryA("test_benchmark_folder_2");
#else
        rmdir("test_benchmark_folder_1");
        rmdir("test_benchmark_folder_2");
#endif
    }

    print_benchmark_stats("Native API - CmdBridge Overhead", times_cmdbridge, ITERATIONS);
    print_benchmark_stats("Native API - OS Raw Call", times_os, ITERATIONS);
    
    free(times_cmdbridge);
    free(times_os);
    
    return 0;
}
