#include "adapter.h"
#include "test_framework.h"
#include <string.h>

void test_adapter_basic() {
    ExecutionIR ir = {0};
    ir.operation = 1; // INTENT_LIST_DIRECTORY
    ir.show_hidden = true;

    AdaptedCommand adapted;
    bool success = adapt_ir_to_native(&ir, "windows", "powershell", "../config/dictionary", &adapted);
    
    ASSERT_TRUE(success);
    if (success) {
        ASSERT_STR_EQ("Get-ChildItem -Hidden", adapted.native_command);
        ASSERT_TRUE(adapted.score.confidence > 80);
    }
}

void run_all_tests() {
    RUN_TEST(test_adapter_basic);
}

TEST_MAIN()
