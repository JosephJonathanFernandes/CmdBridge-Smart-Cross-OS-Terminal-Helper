#include "explain.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int load_explanations(ExplainEntry *entries) {
    int count = 0;
    
    // Fallback path logic similar to commands.c
    const char *config_path = "config/explain.conf";
    FILE *file = fopen(config_path, "r");
    if (!file) {
        config_path = "../config/explain.conf";
        file = fopen(config_path, "r");
    }

    if (!file) {
        log_msg(LOG_ERROR, "Failed to open explain configuration file");
        return 0;
    }

    char line[512];
    while (fgets(line, sizeof(line), file) && count < MAX_EXPLANATIONS) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0 || line[0] == '#') continue;

        char *token = strtok(line, "|");
        char *desc = strtok(NULL, "|");

        if (token && desc) {
            strncpy(entries[count].token, token, MAX_TOKEN_LEN - 1);
            strncpy(entries[count].description, desc, MAX_DESC_LEN - 1);
            count++;
        }
    }
    fclose(file);
    log_msg(LOG_INFO, "Loaded %d explanations from %s", count, config_path);
    return count;
}

void explain_command(int argc, char args[][256], ExplainEntry *entries, int num_entries) {
    if (argc == 0) return;
    
    printf("\nExplanation:\n");

    for (int arg_idx = 0; arg_idx < argc; arg_idx++) {
        char *token = args[arg_idx];
        int found = 0;
        
        // Search dictionary
        for (int i = 0; i < num_entries; i++) {
            if (strcmp(token, entries[i].token) == 0) {
                printf("%-10s -> %s\n", token, entries[i].description);
                found = 1;
                break;
            }
        }
        
        // Dynamic fallback
        if (!found) {
            if (token[0] == '-' || token[0] == '/') {
                printf("%-10s -> unknown flag\n", token);
            } else if (strchr(token, '*') != NULL || strchr(token, '"') != NULL) {
                printf("%-10s -> target parameter / pattern\n", token);
            } else {
                printf("%-10s -> target path or argument\n", token);
            }
        }
    }
}
