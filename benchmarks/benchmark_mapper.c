#include "benchmark_utils.h"
#include "../src/intent.h"
#include "../src/commands.h"
#include "../src/os_mapper.h"

#define ITERATIONS 100000

int main() {
    CommandTemplate templates[MAX_TEMPLATES];
    int num_templates = load_command_templates(templates);
    
    Intent intent;
    strcpy(intent.action, "copy");
    strcpy(intent.object, "files");
    strcpy(intent.args[0], "file1.txt");
    strcpy(intent.args[1], "file2.txt");
    intent.argc = 2;

    char mapped_cmd[MAX_CMD_LEN];
    ExecutionMode mode;

    double *times = malloc(ITERATIONS * sizeof(double));

    for (int i = 0; i < ITERATIONS; i++) {
        double start = get_time_ms();
        map_to_os_command(&intent, templates, num_templates, mapped_cmd, &mode);
        double end = get_time_ms();
        times[i] = end - start;
    }

    print_benchmark_stats("Mapper Benchmark", times, ITERATIONS);
    free(times);
    
    return 0;
}
