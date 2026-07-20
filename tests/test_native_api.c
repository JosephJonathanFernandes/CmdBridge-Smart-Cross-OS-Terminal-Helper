#include "../src/native_api/file_ops.h"
#include "test_framework.h"
#include <stdio.h>

void test_native_create_delete() {
    setup_test_sandbox();
    
    // Create folder
    ASSERT_TRUE(api_create_folder("temp_test/new_folder"));
    ASSERT_TRUE(api_file_exists("temp_test/new_folder"));
    
    // Delete folder
    ASSERT_TRUE(api_delete_folder("temp_test/new_folder"));
    ASSERT_FALSE(api_file_exists("temp_test/new_folder"));
    
    teardown_test_sandbox();
}

void test_native_copy_move() {
    setup_test_sandbox();
    
    // Setup file
    FILE *f = fopen("temp_test/source.txt", "w");
    if (f) {
        fputs("hello", f);
        fclose(f);
    }
    
    ASSERT_TRUE(api_file_exists("temp_test/source.txt"));
    
    // Copy
    ASSERT_TRUE(api_copy_file("temp_test/source.txt", "temp_test/dest.txt"));
    ASSERT_TRUE(api_file_exists("temp_test/dest.txt"));
    
    // Move
    ASSERT_TRUE(api_move_file("temp_test/dest.txt", "temp_test/moved.txt"));
    ASSERT_TRUE(api_file_exists("temp_test/moved.txt"));
    ASSERT_FALSE(api_file_exists("temp_test/dest.txt"));
    
    teardown_test_sandbox();
}

void run_all_tests() {
    RUN_TEST(test_native_create_delete);
    RUN_TEST(test_native_copy_move);
}

TEST_MAIN()
