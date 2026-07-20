#include "benchmark_utils.h"
#include "../src/safety.h"

#define ITERATIONS 100000

int main() {
    const char *cmd = "rm -rf /";
    double *times = malloc(ITERATIONS * sizeof(double));

    for (int i = 0; i < ITERATIONS; i++) {
        double start = get_time_ms();
        is_command_safe(cmd);
        double end = get_time_ms();
        times[i] = end - start;
    }

    print_benchmark_stats("Safety Benchmark", times, ITERATIONS);
    free(times);
    
    return 0;
}
