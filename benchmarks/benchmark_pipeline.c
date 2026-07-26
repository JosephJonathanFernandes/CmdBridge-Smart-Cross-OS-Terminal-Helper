#include "../src/translator.h"
#include "../src/adapter.h"
#include "../src/capability.h"
#include "../src/safety.h"
#include "../src/environment.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int compare_long_long(const void* a, const void* b) {
    long long arg1 = *(const long long*)a;
    long long arg2 = *(const long long*)b;
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

#ifdef _WIN32
#include <windows.h>
long long get_time_ns() {
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000000LL) / freq.QuadPart;
}
#else
#include <time.h>
long long get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}
#endif

int main() {
    printf("Pipeline Benchmark\n");
    printf("------------------\n");
    
    if (!translator_init("../config/dictionary")) {
        printf("Failed to load dictionaries.\n");
        return 1;
    }
    
    const int ITERATIONS = 10000;
    const char* input = "ls -a -l";
    
    // Cold start
    long long cold_start = get_time_ns();
    ExecutionIR ir_cold;
    translate_input_to_ir(input, &ir_cold);
    CapabilitySupport cap_cold = negotiate_capability(&ir_cold, "windows", "powershell", "../config/dictionary");
    if (cap_cold != CAPABILITY_UNSUPPORTED) {
        AdaptedCommand adapted_cold;
        adapt_ir_to_native(&ir_cold, "windows", "powershell", "../config/dictionary", &adapted_cold);
    }
    long long cold_end = get_time_ns();
    long long cold_latency_ns = cold_end - cold_start;
    
    long long* latencies = malloc(ITERATIONS * sizeof(long long));
    long long total_warm_ns = 0;
    
    for (int i = 0; i < ITERATIONS; i++) {
        long long start_iter = get_time_ns();
        
        ExecutionIR ir;
        translate_input_to_ir(input, &ir);
        
        CapabilitySupport cap = negotiate_capability(&ir, "windows", "powershell", "../config/dictionary");
        if (cap != CAPABILITY_UNSUPPORTED) {
            AdaptedCommand adapted;
            adapt_ir_to_native(&ir, "windows", "powershell", "../config/dictionary", &adapted);
        }
        
        long long end_iter = get_time_ns();
        latencies[i] = end_iter - start_iter;
        total_warm_ns += latencies[i];
    }
    
    qsort(latencies, ITERATIONS, sizeof(long long), compare_long_long);
    
    long long p95_ns = latencies[(int)(ITERATIONS * 0.95)];
    long long p99_ns = latencies[(int)(ITERATIONS * 0.99)];
    
    double avg_us = (total_warm_ns / (double)ITERATIONS) / 1000.0;
    
    printf("Iterations: %d\n", ITERATIONS);
    printf("Cold Start Latency: %.3f us\n", cold_latency_ns / 1000.0);
    printf("Avg Warm Latency: %.3f us per command\n", avg_us);
    printf("P95 Latency: %.3f us\n", p95_ns / 1000.0);
    printf("P99 Latency: %.3f us\n", p99_ns / 1000.0);
    
    free(latencies);
    translator_cleanup();
    return 0;
}
