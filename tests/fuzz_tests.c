#include "../src/parser.h"
#include "../src/intent.h"
#include "test_framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

// A simple internal fuzzer to ensure the parser doesn't crash on garbage
void test_parser_fuzzing() {
    srand((unsigned int)time(NULL));
    char fuzzed_input[1024];
    
    // Run 10,000 iterations of random strings
    for (int i = 0; i < 10000; i++) {
        int len = rand() % 500;
        for (int j = 0; j < len; j++) {
            // Random ascii characters including some spaces and quotes
            fuzzed_input[j] = (char)(rand() % 95 + 32); 
        }
        fuzzed_input[len] = '\0';
        
        Intent intent;
        parse_input(fuzzed_input, &intent);
        
        // As long as it doesn't crash, the fuzz test is considered successful.
    }
    ASSERT_TRUE(1);
}

// LLVM libFuzzer entry point
#ifdef __cplusplus
extern "C" {
#endif
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Size >= 1024) return 0;
    char input[1024];
    memcpy(input, Data, Size);
    input[Size] = '\0';
    
    Intent intent;
    parse_input(input, &intent);
    return 0;
}
#ifdef __cplusplus
}
#endif

void run_all_tests() {
    RUN_TEST(test_parser_fuzzing);
}

TEST_MAIN()
