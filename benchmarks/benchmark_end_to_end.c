#include "benchmark_utils.h"
#include "../src/parser.h"
#include "../src/commands.h"
#include "../src/os_mapper.h"
#include "../src/safety.h"

#define ITERATIONS 10000

int main() {
    CommandTemplate templates[MAX_TEMPLATES];
    int num_templates = load_command_templates(templates);
    
    const char *input = "copy file \"My Resume.pdf\" backup";
    double *times = malloc(ITERATIONS * sizeof(double));

    for (int i = 0; i < ITERATIONS; i++) {
        double start = get_time_ms();
        
        Intent intent;
        parse_input(input, &intent);
        
        char mapped_cmd[MAX_CMD_LEN];
        ExecutionMode mode;
        map_to_os_command(&intent, templates, num_templates, mapped_cmd, &mode);
        
        is_command_safe(mapped_cmd);
        
        double end = get_time_ms();
        times[i] = end - start;
    }

    print_benchmark_stats("End-to-End Benchmark", times, ITERATIONS);
    free(times);
    
    return 0;
}
