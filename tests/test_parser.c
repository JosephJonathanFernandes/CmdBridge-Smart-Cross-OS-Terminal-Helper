#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "parser.h"

void test_parse_find() {
    Intent intent;
    parse_input("find pdf files", &intent);
    
    assert(strcmp(intent.action, "find") == 0);
    assert(strcmp(intent.object, "files") == 0);
    assert(strcmp(intent.target, "pdf") == 0);
}

void test_parse_create() {
    Intent intent;
    parse_input("create folder projects", &intent);
    
    assert(strcmp(intent.action, "create") == 0);
    assert(strcmp(intent.object, "folder") == 0);
    assert(strcmp(intent.target, "projects") == 0);
}

int main() {
    printf("Running Parser Tests...\n");
    test_parse_find();
    test_parse_create();
    printf("All Parser Tests Passed!\n");
    return 0;
}
