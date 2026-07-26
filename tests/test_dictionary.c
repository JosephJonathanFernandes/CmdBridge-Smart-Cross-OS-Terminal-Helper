#include "dictionary.h"
#include "test_framework.h"
#include <stdio.h>

void test_dictionary_loading() {
    Dictionary dict;
    bool success = load_dictionary("../config/dictionary/windows_cmd.json", &dict);
    ASSERT_TRUE(success);
    ASSERT_EQ(1, dict.schema_version);
    ASSERT_STR_EQ("cmd", dict.shell);
    
    const CommandMapping* mapping = find_intent_mapping(&dict, INTENT_LIST_DIRECTORY);
    ASSERT_TRUE(mapping != NULL);
    ASSERT_STR_EQ("dir", mapping->command);
}

void run_all_tests() {
    RUN_TEST(test_dictionary_loading);
}

TEST_MAIN()
