#include <stdio.h>
#include <assert.h>
#include "safety.h"

void test_safe_commands() {
    assert(is_command_safe("ls -la") == 1);
    assert(is_command_safe("mkdir tests") == 1);
    assert(is_command_safe("echo 'hello'") == 1);
}

void test_dangerous_commands() {
    assert(is_command_safe("rm -rf /") == 0);
    assert(is_command_safe("Remove-Item -Recurse") == 0);
    assert(is_command_safe("format C:") == 0);
}

int main() {
    printf("Running Safety Tests...\n");
    test_safe_commands();
    test_dangerous_commands();
    printf("All Safety Tests Passed!\n");
    return 0;
}
