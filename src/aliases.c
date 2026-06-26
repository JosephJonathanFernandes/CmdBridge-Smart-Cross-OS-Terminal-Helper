#include "aliases.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>

static const char *ALIASES_FILE = "data/aliases.txt";

int load_aliases(Alias *aliases) {
    int count = 0;
    
    FILE *f = fopen(ALIASES_FILE, "r");
    if (!f) {
        f = fopen("../data/aliases.txt", "r");
    }

    if (!f) {
        log_msg(LOG_INFO, "No aliases file found. Starting empty.");
        return 0;
    }

    char line[512];
    while (fgets(line, sizeof(line), f) && count < MAX_ALIASES) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        char *name = strtok(line, "=");
        char *val = strtok(NULL, "");

        if (name && val) {
            strncpy(aliases[count].name, name, MAX_ALIAS_NAME - 1);
            strncpy(aliases[count].value, val, MAX_ALIAS_VAL - 1);
            count++;
        }
    }
    fclose(f);
    log_msg(LOG_INFO, "Loaded %d aliases.", count);
    return count;
}

int save_alias(const char *name, const char *value, Alias *aliases, int *num_aliases) {
    if (*num_aliases >= MAX_ALIASES) {
        log_msg(LOG_WARN, "Maximum number of aliases reached.");
        return 0;
    }

    FILE *f = fopen(ALIASES_FILE, "a");
    if (!f) {
        f = fopen("../data/aliases.txt", "a");
    }

    if (!f) {
        log_msg(LOG_ERROR, "Failed to open aliases file for saving.");
        return 0;
    }

    fprintf(f, "%s=%s\n", name, value);
    fclose(f);

    strncpy(aliases[*num_aliases].name, name, MAX_ALIAS_NAME - 1);
    strncpy(aliases[*num_aliases].value, value, MAX_ALIAS_VAL - 1);
    (*num_aliases)++;
    
    return 1;
}

const char* resolve_alias(const char *input, Alias *aliases, int num_aliases) {
    for (int i = 0; i < num_aliases; i++) {
        if (strcmp(input, aliases[i].name) == 0) {
            return aliases[i].value;
        }
    }
    return input;
}
