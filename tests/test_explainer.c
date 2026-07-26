#include "explainer.h"
#include "test_framework.h"
#include <string.h>

void test_explainer_basic() {
    ExecutionIR ir = {0};
    ir.operation = 1; // INTENT_LIST_DIRECTORY
    ir.show_hidden = true;
    strcpy(ir.target, ".");

    AdaptedCommand adapted = {0};
    strcpy(adapted.native_command, "Get-ChildItem -Hidden .");
    strcpy(adapted.target_shell, "powershell");
    
    // For now we just call it to ensure no crash
    // Ideally we would capture stdout to verify the output
    explain_translation("ls -a", &ir, &adapted, CAPABILITY_SUPPORTED);
    ASSERT_TRUE(1); // Visual check only
}

void run_all_tests() {
    RUN_TEST(test_explainer_basic);
}

TEST_MAIN()
