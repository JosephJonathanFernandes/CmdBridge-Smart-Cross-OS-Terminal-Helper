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
#include "aliases.h"

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

    Alias aliases[MAX_ALIASES];
    int num_aliases = load_aliases(aliases);

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

        if (strncmp(input, "alias ", 6) == 0) {
            char name[MAX_ALIAS_NAME] = {0};
            char value[MAX_ALIAS_VAL] = {0};
            if (sscanf(input + 6, "%[^=]=\"%[^\"]\"", name, value) == 2) {
                if (save_alias(name, value, aliases, &num_aliases)) {
                    printf("Alias '%s' saved successfully!\n", name);
                }
            } else {
                printf("Invalid alias format. Use: alias name=\"value\"\n");
            }
            continue;
        }

        const char *resolved = resolve_alias(input, aliases, num_aliases);
        char process_input[1024];
        strncpy(process_input, resolved, sizeof(process_input) - 1);
        process_input[sizeof(process_input) - 1] = '\0';

        char *segment = process_input;
        char *next_segment = NULL;

        do {
            next_segment = strstr(segment, "&&");
            if (next_segment) {
                *next_segment = '\0';
                next_segment += 2;
            }

            char current_cmd[1024];
            strncpy(current_cmd, segment, sizeof(current_cmd) - 1);
            current_cmd[sizeof(current_cmd) - 1] = '\0';
            trim_whitespace(current_cmd);

            if (strlen(current_cmd) == 0) {
                segment = next_segment;
                continue;
            }

            Intent intent;
            parse_input(current_cmd, &intent);

            if (intent.action[0] == '\0' || (intent.object[0] == '\0' && strcmp(intent.action, "explain") != 0)) {
                // If it fails to parse as natural language, just treat it as a raw OS command to support things like `cd` in aliases
                strcpy(intent.action, "raw");
                strcpy(intent.target, current_cmd);
            }

            if (strcmp(intent.action, "explain") == 0) {
                explain_command(intent.target, explanations, num_explanations);
                segment = next_segment;
                continue;
            }

            char mapped_cmd[MAX_CMD_LEN];
            if (strcmp(intent.action, "raw") == 0) {
                strncpy(mapped_cmd, intent.target, MAX_CMD_LEN - 1);
            } else if (!map_to_os_command(&intent, templates, num_templates, mapped_cmd)) {
                printf("No matching command template found for action '%s' and object '%s'.\n", intent.action, intent.object);
                segment = next_segment;
                continue;
            }

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
                        segment = next_segment;
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
                    const char *ps_path = "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
                    int result = (int)_spawnl(_P_WAIT, ps_path, "powershell.exe", "-NoProfile", "-Command", mapped_cmd, NULL);
                    
                    if (result == -1) {
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
            
            segment = next_segment;
        } while (segment != NULL);
    }

    printf("Goodbye!\n");
    return 0;
}
