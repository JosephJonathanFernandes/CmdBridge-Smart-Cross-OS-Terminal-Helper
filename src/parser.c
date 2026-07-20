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

    char *argv[100];
    int argc = 0;
    tokenize_command(input, argv, &argc);

    for (int i = 0; i < argc; i++) {
        char *token = argv[i];
        
        char token_lower[1024];
        strncpy(token_lower, token, sizeof(token_lower) - 1);
        token_lower[sizeof(token_lower) - 1] = '\0';
        to_lowercase(token_lower);

        if (is_noise_word(token_lower)) {
            continue;
        }

        const char *primary_action = resolve_synonym(token_lower, action_synonyms);
        const char *primary_object = resolve_synonym(token_lower, object_synonyms);

        if (primary_action && intent->action[0] == '\0') {
            strncpy(intent->action, primary_action, MAX_STR_LEN - 1);
        } else if (primary_object && intent->object[0] == '\0' && strcmp(intent->action, "explain") != 0) {
            strncpy(intent->object, primary_object, MAX_STR_LEN - 1);
        } else {
            if (intent->argc < MAX_ARGS) {
                strncpy(intent->args[intent->argc], token, MAX_STR_LEN - 1);
                intent->argc++;
            }
        }
    }

    free_tokens(argv, argc);

    // Intelligent Object Defaults
    if (intent->object[0] == '\0' && intent->action[0] != '\0' && strcmp(intent->action, "explain") != 0) {
        if (strcmp(intent->action, "find") == 0 || strcmp(intent->action, "list") == 0 || strcmp(intent->action, "delete") == 0) {
            strcpy(intent->object, "files");
        } else if (strcmp(intent->action, "create") == 0) {
            strcpy(intent->object, "folders");
        }
    }
}
