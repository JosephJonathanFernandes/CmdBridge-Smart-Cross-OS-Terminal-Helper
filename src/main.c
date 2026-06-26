#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <process.h>
#endif

#include "intent.h"
#include "parser.h"
#include "commands.h"
#include "os_mapper.h"
#include "safety.h"
#include "utils.h"
#include "logger.h"
#include "explain.h"
#include "history.h"

int main() {
    logger_init(LOG_INFO);
    log_msg(LOG_INFO, "Starting Smart Terminal Assistant (MVP)");

    printf("Smart Terminal Assistant (MVP)\n");
    printf("Type 'exit' to quit.\n");

    CommandTemplate templates[MAX_TEMPLATES];
    int num_templates = load_command_templates(templates);
    if (num_templates == 0) {
        log_msg(LOG_ERROR, "No templates loaded. Exiting.");
        return 1;
    }
    
    ExplainEntry explanations[MAX_EXPLANATIONS];
    int num_explanations = load_explanations(explanations);

    char input[1024];

    while (1) {
        printf("\n> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove trailing newline
        input[strcspn(input, "\r\n")] = '\0';
        trim_whitespace(input);

        if (strlen(input) == 0) continue;
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            break;
        }

        if (strcmp(input, "history") == 0) {
            show_history();
            continue;
        }

        // Log to history file
        log_history(input);

        Intent intent;
        parse_input(input, &intent);
        
        // Uncomment to debug parsing:
        // print_intent(&intent);

        if (intent.action[0] == '\0' || (intent.object[0] == '\0' && strcmp(intent.action, "explain") != 0)) {
            printf("Could not understand command. Please try phrasing differently (e.g. 'find pdf files').\n");
            continue;
        }
        
        if (strcmp(intent.action, "explain") == 0) {
            explain_command(intent.target, explanations, num_explanations);
            continue;
        }

        char mapped_cmd[MAX_CMD_LEN];
        if (map_to_os_command(&intent, templates, num_templates, mapped_cmd)) {
            
            if (!is_command_safe(mapped_cmd)) {
                printf("\nWarning: Potentially destructive operation detected\n");
                printf("Suggested command: %s\n", mapped_cmd);
                printf("Proceed? [y/n]: ");
                
                char choice[10];
                if (fgets(choice, sizeof(choice), stdin) != NULL) {
                    choice[strcspn(choice, "\r\n")] = '\0';
                    to_lowercase(choice);
                    if (strcmp(choice, "y") != 0 && strcmp(choice, "yes") != 0) {
                        printf("Operation cancelled.\n");
                        continue;
                    }
                }
            }
            
            printf("\nSuggested:\n%s\n\n", mapped_cmd);
            printf("[1] Execute\n");
            printf("[2] Copy command\n");
            printf("[3] Cancel\n");
            
            printf("Choice: ");
            char option[10];
            if (fgets(option, sizeof(option), stdin) != NULL) {
                option[strcspn(option, "\r\n")] = '\0';
                
                if (strcmp(option, "1") == 0) {
                    log_msg(LOG_INFO, "Executing: %s", mapped_cmd);
                    printf("\n");
                    
#ifdef _WIN32
                    // The environment PATH or COMSPEC seems to be completely broken in your terminal (which caused system() to fail).
                    // We will forcefully launch PowerShell using its absolute Windows path.
                    const char *ps_path = "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
                    int result = (int)_spawnl(_P_WAIT, ps_path, "powershell.exe", "-NoProfile", "-Command", mapped_cmd, NULL);
                    
                    if (result == -1) {
                        // Fallback in case PowerShell is missing or path is different
                        log_msg(LOG_WARN, "Failed to find PowerShell. Attempting fallback to cmd.exe");
                        result = (int)_spawnl(_P_WAIT, "C:\\Windows\\System32\\cmd.exe", "cmd.exe", "/c", mapped_cmd, NULL);
                    }
#else
                    int result = system(mapped_cmd);
#endif
                    
                    if (result != 0) {
                        log_msg(LOG_WARN, "Command exited with code %d", result);
                    }
                } else if (strcmp(option, "2") == 0) {
                    copy_to_clipboard(mapped_cmd);
                    printf("Copied to clipboard!\n");
                } else {
                    printf("Cancelled.\n");
                }
            }
        } else {
            printf("No matching command template found for action '%s' and object '%s'.\n", intent.action, intent.object);
        }
    }

    printf("Goodbye!\n");
    return 0;
}
