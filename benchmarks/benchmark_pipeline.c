#include "../src/translator.h"
#include "../src/adapter.h"
#include "../src/capability.h"
#include "../src/safety.h"
#include "../src/environment.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    
    long long start = get_time_ns();
    
    for (int i = 0; i < ITERATIONS; i++) {
        ExecutionIR ir;
        translate_input_to_ir(input, &ir);
        
        CapabilitySupport cap = negotiate_capability(&ir, "windows", "powershell", "../config/dictionary");
        if (cap != CAPABILITY_UNSUPPORTED) {
            AdaptedCommand adapted;
            adapt_ir_to_native(&ir, "windows", "powershell", "../config/dictionary", &adapted);
        }
    }
    
    long long end = get_time_ns();
    double total_ms = (end - start) / 1000000.0;
    double avg_us = ((double)(end - start) / ITERATIONS) / 1000.0;
    
    printf("Iterations: %d\n", ITERATIONS);
    printf("Total Time: %.2f ms\n", total_ms);
    printf("Avg Latency (End-to-End pipeline): %.3f us per command\n", avg_us);
    
    translator_cleanup();
    return 0;
}
