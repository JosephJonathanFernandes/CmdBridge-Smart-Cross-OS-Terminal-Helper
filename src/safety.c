#include "safety.h"
#include "utils.h"
#include "native_api/file_ops.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

const char* get_risk_string(RiskLevel risk) {
    switch (risk) {
        case RISK_SAFE: return "SAFE";
        case RISK_LOW: return "LOW";
        case RISK_MEDIUM: return "MEDIUM";
        case RISK_HIGH: return "HIGH";
        case RISK_CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

int validate_intent(const Intent *intent, char *error_msg, int max_msg_len) {
    if (!intent) {
        snprintf(error_msg, max_msg_len, "Invalid intent structure.");
        return 0;
    }

    // Root Deletion Check
    if (strcmp(intent->action, "delete") == 0) {
        for (int i = 0; i < intent->argc; i++) {
            if (strcmp(intent->args[i], "/") == 0 ||
                strcmp(intent->args[i], "C:\\") == 0 ||
                strcmp(intent->args[i], "c:\\") == 0 ||
                strcmp(intent->args[i], ".") == 0 ||
                strcmp(intent->args[i], "..") == 0) {
                snprintf(error_msg, max_msg_len, "Cannot delete root or special directory '%s'.", intent->args[i]);
                return 0;
            }
        }
    }

    // Self-copy check
    if (strcmp(intent->action, "copy") == 0 && intent->argc >= 2) {
        if (strcmp(intent->args[0], intent->args[1]) == 0) {
            snprintf(error_msg, max_msg_len, "Cannot copy a file onto itself.");
            return 0;
        }
    }

    // Move onto itself check
    if (strcmp(intent->action, "move") == 0 && intent->argc >= 2) {
        if (strcmp(intent->args[0], intent->args[1]) == 0) {
            snprintf(error_msg, max_msg_len, "Source and destination are the same.");
            return 0;
        }
    }

    // Source Exists Check (for copy, move, delete)
    if (strcmp(intent->action, "copy") == 0 || strcmp(intent->action, "move") == 0 || strcmp(intent->action, "delete") == 0) {
        if (intent->argc > 0) {
            if (!api_file_exists(intent->args[0])) {
                // Ignore wildcards for this simple check
                if (strchr(intent->args[0], '*') == NULL && strchr(intent->args[0], '?') == NULL) {
                    snprintf(error_msg, max_msg_len, "Source path '%s' does not exist.", intent->args[0]);
                    return 0;
                }
            }
        }
    }

    return 1;
}

RiskLevel get_intent_risk(const Intent *intent) {
    if (!intent) return RISK_SAFE;
    
    if (strcmp(intent->action, "delete") == 0) {
        for (int i = 0; i < intent->argc; i++) {
            if (strcmp(intent->args[i], "Downloads") == 0 ||
                strcmp(intent->args[i], "Documents") == 0) {
                return RISK_HIGH;
            }
            if (strcmp(intent->args[i], "/") == 0 || strcmp(intent->args[i], "C:\\") == 0 || strcmp(intent->args[i], "c:\\") == 0) {
                return RISK_CRITICAL;
            }
        }
        if (strcmp(intent->object, "folders") == 0) {
            return RISK_MEDIUM; // Recursive delete is medium
        }
        return RISK_LOW; // File delete is low
    }

    if (strcmp(intent->action, "create") == 0) {
        return RISK_SAFE;
    }
    
    if (strcmp(intent->action, "copy") == 0) {
        return RISK_SAFE; // Copying doesn't destroy original
    }
    
    if (strcmp(intent->action, "move") == 0) {
        return RISK_LOW; // Move modifies original location
    }

    return RISK_LOW;
}

void get_intent_consequence(const Intent *intent, char *consequence, int max_len) {
    consequence[0] = '\0';
    if (!intent) return;
    
    if (strcmp(intent->action, "create") == 0 && strcmp(intent->object, "folders") == 0) {
        if (intent->argc > 0) {
            snprintf(consequence, max_len, "- Create a new empty directory at '%s'.", intent->args[0]);
        }
    } else if (strcmp(intent->action, "delete") == 0 && strcmp(intent->object, "folders") == 0) {
        if (intent->argc > 0) {
            snprintf(consequence, max_len, "- Permanently delete the folder '%s' and all of its contents.\n- This cannot be undone.", intent->args[0]);
        }
    } else if (strcmp(intent->action, "copy") == 0 && strcmp(intent->object, "files") == 0) {
        if (intent->argc > 1) {
            snprintf(consequence, max_len, "- Duplicate the file '%s'.\n- Save the duplicate to '%s'.\n- The original file will remain unchanged.", intent->args[0], intent->args[1]);
        }
    } else if (strcmp(intent->action, "move") == 0 && strcmp(intent->object, "files") == 0) {
        if (intent->argc > 1) {
            snprintf(consequence, max_len, "- Remove the file '%s' from its current location.\n- Place it at '%s'.\n- The original file will no longer exist in its old location.", intent->args[0], intent->args[1]);
        }
    } else {
        snprintf(consequence, max_len, "- Execute the specified operation on the system.");
    }
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

        // Metacharacter / Injection checks
        if (strchr(argv[i], ';') || strchr(argv[i], '&') || 
            strchr(argv[i], '|') || strchr(argv[i], '`') || 
            strchr(argv[i], '$') || strchr(argv[i], '>') || 
            strchr(argv[i], '<')) {
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
