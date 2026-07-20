#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int test_pass_count;
extern int test_fail_count;

#define ASSERT_TRUE(condition) \
    do { \
        if (condition) { \
            test_pass_count++; \
        } else { \
            test_fail_count++; \
            fprintf(stderr, "FAIL: %s:%d: Expected true but got false\n", __FILE__, __LINE__); \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        if (!(condition)) { \
            test_pass_count++; \
        } else { \
            test_fail_count++; \
            fprintf(stderr, "FAIL: %s:%d: Expected false but got true\n", __FILE__, __LINE__); \
        } \
    } while(0)

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            test_pass_count++; \
        } else { \
            test_fail_count++; \
            fprintf(stderr, "FAIL: %s:%d: Expected %d but got %d\n", __FILE__, __LINE__, (int)(expected), (int)(actual)); \
        } \
    } while(0)

#define ASSERT_STR_EQ(expected, actual) \
    do { \
        const char *e = (expected); \
        const char *a = (actual); \
        if (e == NULL && a == NULL) { \
            test_pass_count++; \
        } else if (e != NULL && a != NULL && strcmp(e, a) == 0) { \
            test_pass_count++; \
        } else { \
            test_fail_count++; \
            fprintf(stderr, "FAIL: %s:%d: Expected string '%s' but got '%s'\n", __FILE__, __LINE__, e ? e : "NULL", a ? a : "NULL"); \
        } \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        printf("Running " #test_func "...\n"); \
        test_func(); \
    } while(0)

#define TEST_MAIN() \
    int main() { \
        test_pass_count = 0; \
        test_fail_count = 0; \
        run_all_tests(); \
        printf("\nTEST RESULTS: %d passed, %d failed\n", test_pass_count, test_fail_count); \
        return test_fail_count == 0 ? 0 : 1; \
    }

// You must define this in your test files
void run_all_tests();

// Filesystem sandbox utilities
void setup_test_sandbox();
void teardown_test_sandbox();

#ifdef __cplusplus
}
#endif

#endif // TEST_FRAMEWORK_H
