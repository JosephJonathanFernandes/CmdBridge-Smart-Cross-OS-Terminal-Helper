#include "benchmark_utils.h"
#include "../src/commands.h"
#include "../src/parser.h"

#define ITERATIONS 1000

int main() {
    double *times_config = malloc(ITERATIONS * sizeof(double));
    double *times_total = malloc(ITERATIONS * sizeof(double));

    for (int i = 0; i < ITERATIONS; i++) {
        double start_total = get_time_ms();
        
        double start_config = get_time_ms();
        CommandTemplate templates[MAX_TEMPLATES];
        load_command_templates(templates);
        double end_config = get_time_ms();
        
        double end_total = get_time_ms();
        
        times_config[i] = end_config - start_config;
        times_total[i] = end_total - start_total;
    }

    print_benchmark_stats("Startup - Load Configuration", times_config, ITERATIONS);
    print_benchmark_stats("Startup - Total", times_total, ITERATIONS);
    
    free(times_config);
    free(times_total);
    
    return 0;
}
