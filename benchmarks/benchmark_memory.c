#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int alloc_count = 0;
static int free_count = 0;
static size_t peak_heap = 0;
static size_t current_heap = 0;

void* track_malloc(size_t size) {
    alloc_count++;
    current_heap += size;
    if (current_heap > peak_heap) peak_heap = current_heap;
    // We allocate a bit extra to store the size so we can decrement current_heap on free
    size_t *ptr = malloc(size + sizeof(size_t));
    if (!ptr) return NULL;
    *ptr = size;
    return (void*)(ptr + 1);
}

void track_free(void* ptr) {
    if (!ptr) return;
    free_count++;
    size_t *real_ptr = ((size_t*)ptr) - 1;
    current_heap -= *real_ptr;
    free(real_ptr);
}

#define malloc track_malloc
#define free track_free

// Now include the source files directly to track their internal allocations
#include "../src/parser.c"
#include "../src/intent.c"
#include "../src/utils.c"
#include "../src/os_mapper.c"
#include "../src/logger.c"

int main() {
    alloc_count = 0;
    free_count = 0;
    peak_heap = 0;
    current_heap = 0;

    const char *input = "copy file \"My Resume.pdf\" backup";
    Intent intent;
    
    // Measure allocations during parse
    parse_input(input, &intent);
    
    printf("\nMemory Benchmark (Parser)\n");
    printf("Peak Heap:\n%zu bytes\n\n", peak_heap);
    printf("Leaks:\n%d\n\n", alloc_count - free_count);
    printf("Allocations:\n%d\n\n", alloc_count);
    printf("Free:\n%d\n\n", free_count);

    return 0;
}
