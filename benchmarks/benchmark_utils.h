#ifndef BENCHMARK_UTILS_H
#define BENCHMARK_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

// Returns time in milliseconds
static inline double get_time_ms() {
#ifdef _WIN32
    LARGE_INTEGER frequency, time;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&time);
    return (double)time.QuadPart * 1000.0 / (double)frequency.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
#endif
}

// Qsort helper for doubles
static int compare_doubles(const void *a, const void *b) {
    double arg1 = *(const double *)a;
    double arg2 = *(const double *)b;
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

static inline void print_benchmark_stats(const char *name, double *times, int iterations) {
    qsort(times, iterations, sizeof(double), compare_doubles);
    
    double sum = 0;
    for (int i = 0; i < iterations; i++) {
        sum += times[i];
    }
    double avg = sum / iterations;
    double median = times[iterations / 2];
    double p95 = times[(int)(iterations * 0.95)];
    double min = times[0];
    double max = times[iterations - 1];
    
    printf("\n%s\n", name);
    printf("Iterations:\n%d\n\n", iterations);
    printf("Average:\n%.3f ms\n\n", avg);
    printf("Median:\n%.3f ms\n\n", median);
    printf("P95:\n%.3f ms\n\n", p95);
    printf("Min:\n%.3f ms\n\n", min);
    printf("Max:\n%.3f ms\n\n", max);
}

#endif // BENCHMARK_UTILS_H
