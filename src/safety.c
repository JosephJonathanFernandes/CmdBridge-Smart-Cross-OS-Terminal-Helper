#include "safety.h"
#include "utils.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int is_intent_safe(const Intent *intent) {
    if (!intent) return 1;
    
    // Check for destructive actions at root directories
    if (strcmp(intent->action, "delete") == 0) {
        for (int i = 0; i < intent->argc; i++) {
            if (strcmp(intent->args[i], "/") == 0 ||
                strcmp(intent->args[i], "C:\\") == 0 ||
                strcmp(intent->args[i], "c:\\") == 0) {
                return 0; // Dangerous intent
            }
        }
    }
    return 1; // Safe
}

int is_command_safe(const char *cmd) {
    if (!cmd) return 1;

    char *argv[100];
    int argc = 0;
    tokenize_command(cmd, argv, &argc);

    int safe = 1;
    int is_rm = 0;
    int has_r = 0;
    int has_f = 0;
    
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "rm") == 0) is_rm = 1;
        if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "-R") == 0) has_r = 1;
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "-F") == 0) has_f = 1;
        if (strcmp(argv[i], "-rf") == 0 || strcmp(argv[i], "-fr") == 0) {
            has_r = 1;
            has_f = 1;
        }
        
        // Single token dangers
        if (strcmp(argv[i], "Remove-Item") == 0 ||
            strcmp(argv[i], "del") == 0 ||
            strcmp(argv[i], "kill") == 0 ||
            strcmp(argv[i], "Stop-Process") == 0 ||
            strcmp(argv[i], "shutdown") == 0 ||
            strcmp(argv[i], "format") == 0 ||
            strcmp(argv[i], "mkfs") == 0) {
            safe = 0;
            break;
        }
    }

    if (is_rm && (has_r || has_f)) {
        safe = 0;
    }

    free_tokens(argv, argc);
    return safe;
}
