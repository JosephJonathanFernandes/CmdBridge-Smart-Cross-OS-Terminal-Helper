#include "../src/parser.h"
#include "../src/intent.h"
#include "../src/safety.h"
#include "test_framework.h"

void test_injection_attacks() {
    // These tests check if the raw command validator properly identifies shell injections
    ASSERT_FALSE(is_command_safe("find . -name *.txt ; rm -rf /"));
    ASSERT_FALSE(is_command_safe("copy file \"& calc\""));
    ASSERT_FALSE(is_command_safe("find files $(shutdown)"));
    ASSERT_FALSE(is_command_safe("find files `shutdown`"));
    ASSERT_FALSE(is_command_safe("ls | rm -rf /"));
    ASSERT_FALSE(is_command_safe("echo test > /etc/passwd"));
}

void test_dangerous_intents() {
    Intent intent;
    char error_msg[512] = {0};
    
    parse_input("delete folder /", &intent);
    ASSERT_FALSE(validate_intent(&intent, error_msg, sizeof(error_msg)));
    
    parse_input("delete folder C:\\", &intent);
    ASSERT_FALSE(validate_intent(&intent, error_msg, sizeof(error_msg)));
}

void run_all_tests() {
    RUN_TEST(test_injection_attacks);
    RUN_TEST(test_dangerous_intents);
}

TEST_MAIN()
