#include "../src/safety.h"
#include "../src/intent.h"
#include "test_framework.h"
#include <string.h>

void test_safety_root_deletion() {
    Intent intent;
    init_intent(&intent);
    strcpy(intent.action, "delete");
    strcpy(intent.object, "folders");
    intent.argc = 1;
    
    strcpy(intent.args[0], "/");
    char error_msg[512] = {0};
    ASSERT_FALSE(validate_intent(&intent, error_msg, sizeof(error_msg)));
    ASSERT_EQ(RISK_CRITICAL, get_intent_risk(&intent));
    
    strcpy(intent.args[0], "C:\\");
    ASSERT_FALSE(validate_intent(&intent, error_msg, sizeof(error_msg)));
    ASSERT_EQ(RISK_CRITICAL, get_intent_risk(&intent));
}

void test_safety_self_copy() {
    Intent intent;
    init_intent(&intent);
    strcpy(intent.action, "copy");
    strcpy(intent.object, "files");
    intent.argc = 2;
    strcpy(intent.args[0], "file.txt");
    strcpy(intent.args[1], "file.txt");
    
    char error_msg[512] = {0};
    ASSERT_FALSE(validate_intent(&intent, error_msg, sizeof(error_msg)));
}

void test_safety_commands() {
    ASSERT_TRUE(is_command_safe("ls -la"));
    ASSERT_TRUE(is_command_safe("mkdir folder"));
    
    ASSERT_FALSE(is_command_safe("rm -rf /"));
    ASSERT_FALSE(is_command_safe("Remove-Item -Recurse C:\\"));
    ASSERT_FALSE(is_command_safe("format C:"));
    ASSERT_FALSE(is_command_safe("shutdown /s"));
}

void run_all_tests() {
    RUN_TEST(test_safety_root_deletion);
    RUN_TEST(test_safety_self_copy);
    RUN_TEST(test_safety_commands);
}

TEST_MAIN()
