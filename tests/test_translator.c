#include "translator.h"
#include "test_framework.h"

void test_translation() {
    ASSERT_TRUE(translator_init("../config/dictionary"));
    
    ExecutionIR ir;
    bool success = translate_input_to_ir("ls -a -l", &ir);
    ASSERT_TRUE(success);
    ASSERT_EQ(INTENT_LIST_DIRECTORY, ir.operation);
    ASSERT_TRUE(ir.show_hidden);
    ASSERT_TRUE(ir.long_format);
    ASSERT_STR_EQ(".", ir.target);
    
    translator_cleanup();
}

void run_all_tests() {
    RUN_TEST(test_translation);
}

TEST_MAIN()
