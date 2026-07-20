#include "../src/parser.h"
#include "../src/intent.h"
#include "../src/os_mapper.h"
#include "../src/safety.h"
#include "../src/commands.h"
#include "../src/native_api/file_ops.h"
#include "test_framework.h"
#include <stdio.h>
#include <string.h>

CommandTemplate templates[MAX_TEMPLATES];
int template_count = 0;

void setup() {
    setup_test_sandbox();
    template_count = load_command_templates(templates);
    ASSERT_TRUE(template_count > 0);
}

void test_full_pipeline_create_folder() {
    Intent intent;
    parse_input("create folder temp_test/my_new_folder", &intent);
    
    char error_msg[512] = {0};
    ASSERT_TRUE(validate_intent(&intent, error_msg, sizeof(error_msg)));
    ASSERT_EQ(RISK_SAFE, get_intent_risk(&intent));
    
    ASSERT_TRUE(execute_native_api(&intent));
    ASSERT_TRUE(api_file_exists("temp_test/my_new_folder"));
}

void test_full_pipeline_copy_file() {
    FILE *f = fopen("temp_test/copy_source.txt", "w");
    if (f) {
        fputs("data", f);
        fclose(f);
    }
    
    Intent intent;
    parse_input("copy file temp_test/copy_source.txt temp_test/copy_dest.txt", &intent);
    
    char error_msg[512] = {0};
    ASSERT_TRUE(validate_intent(&intent, error_msg, sizeof(error_msg)));
    
    ASSERT_TRUE(execute_native_api(&intent));
    ASSERT_TRUE(api_file_exists("temp_test/copy_dest.txt"));
}

void test_full_pipeline_move_file() {
    FILE *f = fopen("temp_test/move_source.txt", "w");
    if (f) {
        fputs("data", f);
        fclose(f);
    }
    
    Intent intent;
    parse_input("move file temp_test/move_source.txt temp_test/move_dest.txt", &intent);
    
    char error_msg[512] = {0};
    ASSERT_TRUE(validate_intent(&intent, error_msg, sizeof(error_msg)));
    
    ASSERT_TRUE(execute_native_api(&intent));
    ASSERT_TRUE(api_file_exists("temp_test/move_dest.txt"));
    ASSERT_FALSE(api_file_exists("temp_test/move_source.txt"));
}

void run_all_tests() {
    setup();
    RUN_TEST(test_full_pipeline_create_folder);
    RUN_TEST(test_full_pipeline_copy_file);
    RUN_TEST(test_full_pipeline_move_file);
    teardown_test_sandbox();
}

TEST_MAIN()
