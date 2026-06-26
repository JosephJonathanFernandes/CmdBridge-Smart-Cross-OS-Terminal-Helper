#include "history.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>

static const char *HISTORY_FILE = "data/history.txt";

void log_history(const char *input) {
    if (!input || strlen(input) == 0) return;
    
    // Check if it's the history command itself, we don't need to log "history"
    if (strcmp(input, "history") == 0 || strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
        return;
    }

    FILE *f = fopen(HISTORY_FILE, "a");
    if (!f) {
        // Fallback for when running in build/
        f = fopen("../data/history.txt", "a");
    }
    
    if (f) {
        fprintf(f, "%s\n", input);
        fclose(f);
    } else {
        log_msg(LOG_WARN, "Failed to write to history file.");
    }
}

void show_history(void) {
    FILE *f = fopen(HISTORY_FILE, "r");
    if (!f) {
        f = fopen("../data/history.txt", "r");
    }

    if (!f) {
        printf("No history found.\n");
        return;
    }

    printf("\nCommand History:\n");
    char line[1024];
    int count = 1;
    while (fgets(line, sizeof(line), f)) {
        printf("%d. %s", count++, line);
    }
    fclose(f);
    printf("\n");
}
