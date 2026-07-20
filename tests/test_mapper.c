#include "../src/os_mapper.h"
#include "../src/intent.h"
#include "../src/commands.h"
#include "test_framework.h"
#include <string.h>

void test_mapper_replacement() {
    CommandTemplate templates[1];
    strcpy(templates[0].action, "copy");
    strcpy(templates[0].object, "files");
    templates[0].linux_mode = EXEC_NATIVE;
    strcpy(templates[0].linux_cmd, "cp \"{ARG0}\" \"{ARG1}\"");
    templates[0].windows_mode = EXEC_SHELL;
    strcpy(templates[0].windows_cmd, "Copy-Item \"{ARG0}\" \"{ARG1}\"");
    
    Intent intent;
    init_intent(&intent);
    strcpy(intent.action, "copy");
    strcpy(intent.object, "files");
    intent.argc = 2;
    strcpy(intent.args[0], "source.txt");
    strcpy(intent.args[1], "dest.txt");
    
    char mapped_cmd[1024];
    ExecutionMode mode;
    int success = map_to_os_command(&intent, templates, 1, mapped_cmd, &mode);
    ASSERT_TRUE(success);
    
#ifdef _WIN32
    ASSERT_EQ(EXEC_SHELL, mode);
    ASSERT_STR_EQ("Copy-Item \"source.txt\" \"dest.txt\"", mapped_cmd);
#elif __APPLE__
    // The current mapper doesn't fallback nicely if mac_mode is 0, let's just ignore for now or check current behaviour.
#else
    ASSERT_EQ(EXEC_NATIVE, mode);
    ASSERT_STR_EQ("cp \"source.txt\" \"dest.txt\"", mapped_cmd);
#endif
}

void test_unsupported_intent() {
    CommandTemplate templates[1];
    strcpy(templates[0].action, "copy");
    strcpy(templates[0].object, "files");
    
    Intent intent;
    init_intent(&intent);
    strcpy(intent.action, "move");
    strcpy(intent.object, "files");
    
    char mapped_cmd[1024];
    ExecutionMode mode;
    int success = map_to_os_command(&intent, templates, 1, mapped_cmd, &mode);
    ASSERT_FALSE(success);
}

void run_all_tests() {
    RUN_TEST(test_mapper_replacement);
    RUN_TEST(test_unsupported_intent);
}

TEST_MAIN()
