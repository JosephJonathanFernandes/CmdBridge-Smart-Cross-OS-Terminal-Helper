#include "parser.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
    const char *primary;
    const char *synonyms[10];
} SynonymGroup;

SynonymGroup action_synonyms[] = {
    {"find", {"search", "locate", NULL}},
    {"delete", {"remove", "del", "rm", "erase", NULL}},
    {"list", {"show", "display", "ls", "dir", NULL}},
    {"create", {"make", "new", "mkdir", NULL}},
    {"explain", {NULL}},
    {NULL, {NULL}}
};

SynonymGroup object_synonyms[] = {
    {"files", {"file", "documents", "docs", NULL}},
    {"folders", {"folder", "directory", "directories", "dir", "dirs", NULL}},
    {"processes", {"process", "tasks", "apps", NULL}},
    {NULL, {NULL}}
};

const char *noise_words[] = {"my", "the", "a", "an", "all", "some", NULL};

const char* resolve_synonym(const char* word, SynonymGroup* groups) {
    for (int i = 0; groups[i].primary != NULL; i++) {
        if (strcmp(word, groups[i].primary) == 0) return groups[i].primary;
        for (int j = 0; groups[i].synonyms[j] != NULL; j++) {
            if (strcmp(word, groups[i].synonyms[j]) == 0) return groups[i].primary;
        }
    }
    return NULL;
}

int is_noise_word(const char *word) {
    for (int i = 0; noise_words[i] != NULL; i++) {
        if (strcmp(word, noise_words[i]) == 0) return 1;
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
        if (is_noise_word(token)) {
            token = strtok(NULL, " ");
            continue;
        }

        const char *primary_action = resolve_synonym(token, action_synonyms);
        const char *primary_object = resolve_synonym(token, object_synonyms);

        if (primary_action && intent->action[0] == '\0') {
            strncpy(intent->action, primary_action, MAX_STR_LEN - 1);
        } else if (primary_object && intent->object[0] == '\0') {
            strncpy(intent->object, primary_object, MAX_STR_LEN - 1);
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

    // Intelligent Object Defaults
    if (intent->object[0] == '\0' && intent->action[0] != '\0' && strcmp(intent->action, "explain") != 0) {
        if (strcmp(intent->action, "find") == 0 || strcmp(intent->action, "list") == 0 || strcmp(intent->action, "delete") == 0) {
            strcpy(intent->object, "files");
        } else if (strcmp(intent->action, "create") == 0) {
            strcpy(intent->object, "folders");
        }
    }

    trim_whitespace(target_buffer);
    strncpy(intent->target, target_buffer, MAX_STR_LEN - 1);
}
