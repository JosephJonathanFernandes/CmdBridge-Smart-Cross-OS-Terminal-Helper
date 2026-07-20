#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
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
#include "native_api/file_ops.h"

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
                strncpy(intent.args[0], current_cmd, MAX_STR_LEN - 1);
                intent.argc = 1;
            }

            if (strcmp(intent.action, "explain") == 0) {
                explain_command(intent.argc, intent.args, explanations, num_explanations);
                segment = next_segment;
                continue;
            }

            char mapped_cmd[MAX_CMD_LEN];
            ExecutionMode mode = EXEC_SHELL;
            
            if (strcmp(intent.action, "raw") == 0) {
                strncpy(mapped_cmd, intent.args[0], MAX_CMD_LEN - 1);
            } else if (!map_to_os_command(&intent, templates, num_templates, mapped_cmd, &mode)) {
                printf("No matching command template found for action '%s' and object '%s'.\n", intent.action, intent.object);
                segment = next_segment;
                continue;
            }

            char error_msg[512] = {0};
            if (strcmp(intent.action, "raw") != 0 && !validate_intent(&intent, error_msg, sizeof(error_msg))) {
                printf("\nValidation Failed:\n%s\n", error_msg);
                segment = next_segment;
                continue;
            }

            RiskLevel risk = RISK_SAFE;
            if (strcmp(intent.action, "raw") != 0) {
                risk = get_intent_risk(&intent);
                if (!is_command_safe(mapped_cmd) && risk < RISK_HIGH) {
                    risk = RISK_HIGH;
                }
            } else if (!is_command_safe(mapped_cmd)) {
                risk = RISK_HIGH;
            }

            char consequence[512] = {0};
            get_intent_consequence(&intent, consequence, sizeof(consequence));

            printf("\n──────────────────────────────\n");
            printf("✓ I understood\n\n");
            
            if (strcmp(intent.action, "raw") == 0) {
                printf("Action:\nRaw OS Command\n\n");
                printf("Target:\n%s\n\n", intent.args[0]);
            } else {
                printf("Action:\n%s %s\n\n", intent.action, intent.object);
                if (intent.argc > 0) {
                    printf("Source:\n%s\n\n", intent.args[0]);
                }
                if (intent.argc > 1) {
                    printf("Destination:\n%s\n\n", intent.args[1]);
                }
            }
            
            printf("Platform:\n");
#ifdef _WIN32
            printf("Windows\n\n");
#elif __APPLE__
            printf("macOS\n\n");
#else
            printf("Linux/POSIX\n\n");
#endif

            printf("Method:\n");
            if (mode == EXEC_API) {
                printf("Native API (No shell used)\n\n");
            } else if (mode == EXEC_NATIVE) {
                printf("POSIX API (execvp)\n\n");
            } else {
                printf("System Shell\n\n");
            }

            printf("Risk:\n%s\n\n", get_risk_string(risk));

            printf("Will do:\n%s\n", consequence);
            printf("──────────────────────────────\n\n");

            printf("Proceed? [y/n/d]: ");
            
            char choice[10];
            if (fgets(choice, sizeof(choice), stdin) != NULL) {
                choice[strcspn(choice, "\r\n")] = '\0';
                to_lowercase(choice);
                
                if (strcmp(choice, "d") == 0 || strcmp(choice, "dry run") == 0 || strcmp(choice, "dryrun") == 0) {
                    printf("\n--- DRY RUN MODE ---\n");
                    printf("Would execute:\n");
                    if (mode == EXEC_API) {
                        printf("C API Native Function Call\n");
                    } else {
                        printf("%s\n", mapped_cmd);
                    }
                    printf("Nothing executed.\n");
                } else if (strcmp(choice, "y") == 0 || strcmp(choice, "yes") == 0) {
                    log_msg(LOG_INFO, "Executing command with mode %d", mode);
                    printf("\n");
                    
                    int result = -1;
                    if (mode == EXEC_API) {
                        result = execute_native_api(&intent) ? 0 : 1;
                    } 
#ifdef _WIN32
                    else {
                        const char *ps_path = "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
                        result = (int)_spawnl(_P_WAIT, ps_path, "powershell.exe", "-NoProfile", "-Command", mapped_cmd, NULL);
                        
                        if (result == -1) {
                            log_msg(LOG_WARN, "Failed to find PowerShell. Attempting fallback to cmd.exe");
                            result = (int)_spawnl(_P_WAIT, "C:\\Windows\\System32\\cmd.exe", "cmd.exe", "/c", mapped_cmd, NULL);
                        }
                    }
#else
                    else if (mode == EXEC_NATIVE) {
                        char *argv[100];
                        int exec_argc = 0;
                        tokenize_command(mapped_cmd, argv, &exec_argc);
                        
                        pid_t pid = fork();
                        if (pid == 0) {
                            execvp(argv[0], argv);
                            perror("execvp failed");
                            exit(1);
                        } else if (pid > 0) {
                            int status;
                            waitpid(pid, &status, 0);
                            if (WIFEXITED(status)) {
                                result = WEXITSTATUS(status);
                            }
                        }
                        free_tokens(argv, exec_argc);
                    } else {
                        result = system(mapped_cmd);
                    }
#endif
                    
                    if (result != 0) {
                        log_msg(LOG_WARN, "Command exited with code %d", result);
                    }
                } else {
                    printf("Operation cancelled.\n");
                }
            }
            
            segment = next_segment;
        } while (segment != NULL);
    }

    printf("Goodbye!\n");
    return 0;
}
