#include "../src/parser.h"
#include "test_framework.h"

void test_quoted_filenames() {
    Intent intent;
    parse_input("copy file \"my resume.pdf\" backup", &intent);
    ASSERT_STR_EQ("copy", intent.action);
    ASSERT_STR_EQ("files", intent.object);
    ASSERT_EQ(2, intent.argc);
    ASSERT_STR_EQ("my resume.pdf", intent.args[0]);
    ASSERT_STR_EQ("backup", intent.args[1]);
}

void test_escaped_quotes() {
    Intent intent;
    parse_input("create folder \"folder\\\"name\"", &intent);
    ASSERT_STR_EQ("create", intent.action);
    ASSERT_STR_EQ("folders", intent.object);
    ASSERT_EQ(1, intent.argc);
    ASSERT_STR_EQ("folder\"name", intent.args[0]);
}

void test_multiple_arguments() {
    Intent intent;
    parse_input("move file a.txt b.txt c.txt destination", &intent);
    ASSERT_STR_EQ("move", intent.action);
    ASSERT_STR_EQ("files", intent.object);
    ASSERT_EQ(4, intent.argc);
    ASSERT_STR_EQ("a.txt", intent.args[0]);
    ASSERT_STR_EQ("b.txt", intent.args[1]);
    ASSERT_STR_EQ("c.txt", intent.args[2]);
    ASSERT_STR_EQ("destination", intent.args[3]);
}

void test_empty_input() {
    Intent intent;
    parse_input("", &intent);
    ASSERT_STR_EQ("", intent.action);
    ASSERT_STR_EQ("", intent.object);
    ASSERT_EQ(0, intent.argc);
    
    parse_input("   ", &intent);
    // Even if it parses spaces, argc should be 0 because they are stripped.
    // wait, tokenize_command trims spaces.
    ASSERT_STR_EQ("", intent.action);
    ASSERT_STR_EQ("", intent.object);
    ASSERT_EQ(0, intent.argc);
}

void test_invalid_syntax() {
    Intent intent;
    parse_input("blablabla", &intent);
    // Unrecognized words are treated as arguments, action remains empty
    ASSERT_STR_EQ("", intent.action);
    ASSERT_STR_EQ("", intent.object);
    ASSERT_EQ(1, intent.argc);
    ASSERT_STR_EQ("blablabla", intent.args[0]);
}

void test_mixed_casing() {
    Intent intent;
    parse_input("CoPy FiLe A.txt", &intent);
    ASSERT_STR_EQ("copy", intent.action);
    ASSERT_STR_EQ("files", intent.object);
    ASSERT_EQ(1, intent.argc);
    // Arguments should retain original casing
    ASSERT_STR_EQ("A.txt", intent.args[0]);
}

void run_all_tests() {
    RUN_TEST(test_quoted_filenames);
    RUN_TEST(test_escaped_quotes);
    RUN_TEST(test_multiple_arguments);
    RUN_TEST(test_empty_input);
    RUN_TEST(test_invalid_syntax);
    RUN_TEST(test_mixed_casing);
}

TEST_MAIN()
