#include "capability.h"
#include "test_framework.h"
#include <string.h>

void test_capability_check() {
    ExecutionIR ir = {0};
    ir.operation = 1; // INTENT_LIST_DIRECTORY

    CapabilitySupport support = negotiate_capability(&ir, "windows", "powershell", "../config/dictionary");
    ASSERT_TRUE(support == CAPABILITY_SUPPORTED);
}

void test_capability_unsupported() {
    ExecutionIR ir = {0};
    ir.operation = 999; // Unknown operation

    CapabilitySupport support = negotiate_capability(&ir, "linux", "bash", "../config/dictionary");
    ASSERT_TRUE(support == CAPABILITY_UNSUPPORTED);
}

void run_all_tests() {
    RUN_TEST(test_capability_check);
    RUN_TEST(test_capability_unsupported);
}

TEST_MAIN()
