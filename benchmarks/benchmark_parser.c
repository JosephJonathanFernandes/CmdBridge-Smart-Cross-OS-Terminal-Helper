#include "benchmark_utils.h"
#include "../src/parser.h"

#define ITERATIONS 100000

void run_parser_benchmark() {
    double *times = malloc(ITERATIONS * sizeof(double));
    Intent intent;
    const char *input = "copy file \"My Resume.pdf\" backup";
    
    for (int i = 0; i < ITERATIONS; i++) {
        double start = get_time_ms();
        parse_input(input, &intent);
        double end = get_time_ms();
        times[i] = end - start;
    }
    
    print_benchmark_stats("Parser Benchmark", times, ITERATIONS);
    free(times);
}

void run_length_scaling() {
    printf("\nInput Length Scaling Benchmark\n");
    printf("------------------------------\n");
    const int lengths[] = {20, 50, 100, 500, 1000};
    int num_lengths = sizeof(lengths) / sizeof(lengths[0]);
    
    for (int i = 0; i < num_lengths; i++) {
        int len = lengths[i];
        char *input = malloc(len + 1);
        memset(input, 'a', len);
        // Make it look like a real command with spaces
        input[4] = ' '; input[9] = ' '; input[14] = ' ';
        input[len] = '\0';
        
        Intent intent;
        double sum = 0;
        int iters = 10000;
        
        for (int j = 0; j < iters; j++) {
            double start = get_time_ms();
            parse_input(input, &intent);
            double end = get_time_ms();
            sum += (end - start);
        }
        
        printf("%d chars: %.4f ms avg\n", len, sum / iters);
        free(input);
    }
}

int main() {
    run_parser_benchmark();
    run_length_scaling();
    return 0;
}
