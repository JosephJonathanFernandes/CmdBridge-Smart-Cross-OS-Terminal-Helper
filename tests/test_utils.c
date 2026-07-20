#include "../src/utils.h"
#include "test_framework.h"
#include <string.h>

void test_trim_whitespace() {
    char str1[] = "   hello   ";
    trim_whitespace(str1);
    ASSERT_STR_EQ("hello", str1);
    
    char str2[] = "world";
    trim_whitespace(str2);
    ASSERT_STR_EQ("world", str2);
    
    char str3[] = "   ";
    trim_whitespace(str3);
    ASSERT_STR_EQ("", str3);
}

void test_tokenize_command() {
    char *argv[100];
    int argc = 0;
    
    tokenize_command("copy file \"my resume.pdf\" backup", argv, &argc);
    ASSERT_EQ(4, argc);
    ASSERT_STR_EQ("copy", argv[0]);
    ASSERT_STR_EQ("file", argv[1]);
    ASSERT_STR_EQ("my resume.pdf", argv[2]);
    ASSERT_STR_EQ("backup", argv[3]);
    free_tokens(argv, argc);
    
    argc = 0;
    tokenize_command("  spaces   are    ignored  ", argv, &argc);
    ASSERT_EQ(3, argc);
    ASSERT_STR_EQ("spaces", argv[0]);
    ASSERT_STR_EQ("are", argv[1]);
    ASSERT_STR_EQ("ignored", argv[2]);
    free_tokens(argv, argc);
}

void run_all_tests() {
    RUN_TEST(test_trim_whitespace);
    RUN_TEST(test_tokenize_command);
}

TEST_MAIN()
