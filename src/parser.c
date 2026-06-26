#include "parser.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

const char *ACTIONS[] = {
    "find", "show", "create", "delete", "move", "copy", "compress", "search", "list", "explain", NULL
};

const char *OBJECTS[] = {
    "files", "file", "folder", "processes", "text", "disk", NULL
};

int is_in_list(const char *word, const char **list) {
    for (int i = 0; list[i] != NULL; i++) {
        if (strcmp(word, list[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void parse_input(const char *input, Intent *intent) {
    init_intent(intent);
    if (!input || strlen(input) == 0) return;

    char buffer[1024];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    to_lowercase(buffer);

    char input_copy[1024];
    strncpy(input_copy, buffer, sizeof(input_copy) - 1);

    char target_buffer[MAX_STR_LEN] = "";

    // Tokenize by space
    char *token = strtok(buffer, " ");
    while (token != NULL) {
        if (is_in_list(token, ACTIONS) && intent->action[0] == '\0') {
            strncpy(intent->action, token, MAX_STR_LEN - 1);
        } else if (is_in_list(token, OBJECTS) && intent->object[0] == '\0') {
            strncpy(intent->object, token, MAX_STR_LEN - 1);
        } else {
            // Append to target
            if (strlen(target_buffer) > 0) {
                strcat(target_buffer, " ");
            }
            strncat(target_buffer, token, MAX_STR_LEN - strlen(target_buffer) - 1);
        }
        token = strtok(NULL, " ");
    }

    // Check if the action is "explain"
    if (strcmp(intent->action, "explain") == 0) {
        // For explain, the rest of the string is the target command
        char *target_start = strstr(input_copy, "explain");
        if (target_start) {
            target_start += strlen("explain");
            while (*target_start == ' ') target_start++; // skip spaces
            strncpy(intent->target, target_start, sizeof(intent->target) - 1);
        }
        return; // Skip object matching for 'explain'
    }

    trim_whitespace(target_buffer);
    strncpy(intent->target, target_buffer, MAX_STR_LEN - 1);
}
