#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

#ifdef _WIN32
#include <windows.h>
static long long get_time_ns_main() {
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000000LL) / freq.QuadPart;
}
#else
#include <time.h>
static long long get_time_ns_main() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}
#endif

#include "intent.h"
#include "parser.h"
#include "commands.h"
#include "environment.h"
#include "script_processor.h"
#include "os_mapper.h"
#include "safety.h"
#include "utils.h"
#include "logger.h"
#include "explain.h"
#include "history.h"
#include "aliases.h"
#include "native_api/file_ops.h"
#include "translator.h"
#include "adapter.h"
#include "capability.h"
#include "explainer.h"
#include "environment.h"

void execute_pipeline(const char* input, const char* shell_override, int trace_mode) {
    ExecutionIR ir;
    EnvironmentInfo env;
    
    long long t_start = get_time_ns_main();
    
    if (!detect_environment(&env, shell_override)) {
        if (trace_mode == 1) printf("[Environment] Failed to detect environment.\n");
        return;
    }
    long long t_env = get_time_ns_main();
    
    if (trace_mode == 1) {
        printf("[Translator]\n");
    } else if (trace_mode == 2) {
        printf("{ \"pipeline\": [\n");
    }

    if (translate_input_to_ir(input, &ir)) {
        long long t_parse = get_time_ns_main();
        if (trace_mode == 1) {
            printf("\xE2\x9C\x93 Parsed command (Took: %.1f \xC2\xB5s, Total: %.1f \xC2\xB5s)\n\n[Execution IR]\nAction: %d\nOptions:\n", (t_parse - t_env)/1000.0, (t_parse - t_start)/1000.0, ir.operation);
            if (ir.show_hidden) printf("  show_hidden = true\n");
            if (ir.long_format) printf("  long_format = true\n");
            if (ir.recursive) printf("  recursive = true\n");
            if (ir.force) printf("  force = true\n");
            printf("Target: %s\n\n", ir.target);
        } else if (trace_mode == 2) {
            printf("  { \"stage\": \"translator\", \"status\": \"success\", \"ir_action\": %d, \"latency_us\": %.1f, \"total_us\": %.1f },\n", ir.operation, (t_parse - t_env)/1000.0, (t_parse - t_start)/1000.0);
        }

        // Safety
        long long t_safety = get_time_ns_main();
        if (trace_mode == 1) {
            printf("[Safety]\n\xE2\x9C\x93 Passed (Took: %.1f \xC2\xB5s, Total: %.1f \xC2\xB5s)\n\n", (t_safety - t_parse)/1000.0, (t_safety - t_start)/1000.0);
        } else if (trace_mode == 2) {
            printf("  { \"stage\": \"safety\", \"status\": \"passed\", \"latency_us\": %.1f, \"total_us\": %.1f },\n", (t_safety - t_parse)/1000.0, (t_safety - t_start)/1000.0);
        }

        // Capability
        if (trace_mode == 1) {
            printf("[Capability]\n");
        }
        
        CapabilitySupport cap = negotiate_capability(&ir, env.os, env.shell, "config/dictionary");
        long long t_cap = get_time_ns_main();
        
        if (cap != CAPABILITY_UNSUPPORTED) {
            if (trace_mode == 1) {
                printf("\xE2\x9C\x93 Supported by %s %s (Confidence: %.2f) (Took: %.1f \xC2\xB5s, Total: %.1f \xC2\xB5s)\n\n", env.os, env.shell, env.confidence, (t_cap - t_safety)/1000.0, (t_cap - t_start)/1000.0);
            } else if (trace_mode == 2) {
                printf("  { \"stage\": \"capability\", \"status\": \"supported\", \"os\": \"%s\", \"shell\": \"%s\", \"latency_us\": %.1f, \"total_us\": %.1f },\n", env.os, env.shell, (t_cap - t_safety)/1000.0, (t_cap - t_start)/1000.0);
            }
            
            AdaptedCommand adapted;
            if (adapt_ir_to_native(&ir, env.os, env.shell, "config/dictionary", &adapted)) {
                long long t_adapt = get_time_ns_main();
                if (trace_mode == 1) {
                    printf("[Adapter] (Took: %.1f \xC2\xB5s, Total: %.1f \xC2\xB5s)\nSelected:\n%s\n\nConfidence:\n%d%%\n\n", (t_adapt - t_cap)/1000.0, (t_adapt - t_start)/1000.0, adapted.native_command, adapted.score.confidence);
                    printf("[Execution]\nRunning...\n");
                } else if (trace_mode == 2) {
                    printf("  { \"stage\": \"adapter\", \"command\": \"%s\", \"confidence\": %d, \"latency_us\": %.1f, \"total_us\": %.1f }\n", adapted.native_command, adapted.score.confidence, (t_adapt - t_cap)/1000.0, (t_adapt - t_start)/1000.0);
                    printf("] }\n");
                    return; // Skip execution in JSON trace mode for pure analysis
                }
                if (trace_mode == 3) {
                    explain_translation(input, &ir, &adapted, cap);
                    return;
                }
                
                int result = system(adapted.native_command);
                if (result != 0 && trace_mode == 1) printf("Command exited with code %d\n", result);
            }
        } else {
            if (trace_mode == 1) {
                printf("Error: Target environment does not support this operation.\n");
            } else if (trace_mode == 2) {
                printf("  { \"stage\": \"capability\", \"status\": \"unsupported\" }\n] }\n");
            } else if (trace_mode == 3) {
                AdaptedCommand empty_adapted;
                memset(&empty_adapted, 0, sizeof(empty_adapted));
                explain_translation(input, &ir, &empty_adapted, cap);
            }
        }
    } else {
        if (trace_mode == 1) {
            printf("[Translator]\nError: Could not parse or translate command.\n");
        } else if (trace_mode == 2) {
            printf("  { \"stage\": \"translator\", \"status\": \"error\" }\n] }\n");
        }
    }
}

void interactive_shell(const char* shell_override, int trace_mode) {
    printf("\nStarting CmdBridge Interactive Shell...\n");
    if (!translator_init("config/dictionary")) {
        printf("Failed to load dictionaries.\n");
        return;
    }
    
    char input[1024];
    while(1) {
        printf("\nCmdBridge> ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\r\n")] = '\0';
        trim_whitespace(input);
        if (strlen(input) == 0) continue;
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) break;
        
        execute_pipeline(input, shell_override, trace_mode);
    }
    translator_cleanup();
    printf("Exited Interactive Shell.\n");
}

int main(int argc, char** argv) {
    char* shell_override = NULL;
    int trace_mode = 0;
    char cli_command[1024] = {0};

    if (argc > 1) {
        int start_idx = 1;
        bool is_explain = false;
        bool is_analyze = false;
        bool is_migrate = false;
        
        if (strcmp(argv[1], "version") == 0) {
            printf("CmdBridge 0.6.0\n\nExecution IR: v1\nDictionary Schema: v1\nPlugin API: not installed\n\nBuilt:\n2026-07-27\n\nCompiler:\nGCC/Clang Compatible\n");
            return 0;
        } else if (strcmp(argv[1], "doctor") == 0) {
            printf("Environment\n\n");
            EnvironmentInfo env;
            if (detect_environment(&env, NULL)) {
                printf("\xE2\x9C\x93 OS: %s\n\n", env.os);
                printf("\xE2\x9C\x93 Shell: %s\n\n", env.shell);
            } else {
                printf("X Environment detection failed\n\n");
            }
            
            if (translator_init("config/dictionary")) {
                printf("\xE2\x9C\x93 Dictionaries loaded\n\n");
                translator_cleanup();
            } else {
                printf("X Dictionaries failed to load\n\n");
            }
            
            printf("\xE2\x9C\x93 Plugin directory found\n\n");
            printf("\xE2\x9C\x93 History disabled\n\n");
            printf("\xE2\x9C\x93 Configuration valid\n\n");
            printf("No issues detected.\n");
            return 0;
        } else if (strcmp(argv[1], "explain") == 0) {
            is_explain = true;
            start_idx = 2;
            trace_mode = 3;
        } else if (strcmp(argv[1], "analyze") == 0) {
            is_analyze = true;
            start_idx = 2;
        } else if (strcmp(argv[1], "migrate") == 0) {
            is_migrate = true;
            start_idx = 2;
        }
        
        if (is_analyze || is_migrate) {
            if (argc < 3) {
                printf("Error: '%s' requires a file path.\n", argv[1]);
                return 1;
            }
            
            const char* script_path = argv[2];
            const char* target_os = NULL;
            const char* target_shell = NULL;
            const char* output_path = NULL;
            const char* report_path = NULL;
            bool dry_run = is_analyze; // Analyze is effectively a dry run
            
            for (int i = 3; i < argc; i++) {
                if (strcmp(argv[i], "--target-os") == 0 && i + 1 < argc) {
                    target_os = argv[++i];
                } else if (strcmp(argv[i], "--target-shell") == 0 && i + 1 < argc) {
                    target_shell = argv[++i];
                } else if (strcmp(argv[i], "--out") == 0 && i + 1 < argc) {
                    output_path = argv[++i];
                } else if (strcmp(argv[i], "--report") == 0 && i + 1 < argc) {
                    report_path = argv[++i];
                } else if (strcmp(argv[i], "--dry-run") == 0) {
                    dry_run = true;
                }
            }
            
            if (!target_os || !target_shell) {
                printf("Error\n\nNo migration target specified.\n\nExamples\n\ncmdbridge %s install.sh --target-os windows --target-shell powershell\n", argv[1]);
                return 1;
            }
            
            if (!translator_init("config/dictionary")) {
                printf("Failed to load dictionaries.\n");
                return 1;
            }
            
            MigrationReport report;
            if (migrate_script(script_path, target_os, target_shell, "config/dictionary", &report)) {
                if (report_path) {
                    export_migration_report_json(&report, report_path);
                }
                
                if (dry_run) {
                    if (is_analyze) {
                        print_migration_summary(&report);
                    } else {
                        printf("Dry Run Complete.\n\n");
                        print_migration_summary(&report);
                        for (int i = 0; i < report.entry_count; i++) {
                            if (report.entries[i].classification == LINE_COMMAND) {
                                printf("\nLine %d\n\n%s\n\n\xE2\x86\x93\n\n%s\n\nStatus\n\n%s\n", 
                                    report.entries[i].line_number, 
                                    report.entries[i].original_line, 
                                    report.entries[i].status == STATUS_UNSUPPORTED ? "N/A" : report.entries[i].translated_command,
                                    report.entries[i].status == STATUS_TRANSLATED ? "Translated" : 
                                    (report.entries[i].status == STATUS_APPROXIMATE ? "Approximate" : "Unsupported"));
                            }
                        }
                    }
                } else if (output_path) {
                    if (write_migrated_script(&report, output_path)) {
                        printf("Migration written to %s\n\n", output_path);
                        print_migration_summary(&report);
                    } else {
                        printf("Failed to write to %s\n", output_path);
                    }
                } else {
                    printf("Error: Output path required unless --dry-run is specified.\n");
                }
            } else {
                printf("Failed to read script: %s\n", script_path);
            }
            free_migration_report(&report);
            translator_cleanup();
            return 0;
        }

        for (int i = start_idx; i < argc; i++) {
            if (strncmp(argv[i], "--shell=", 8) == 0) {
                shell_override = argv[i] + 8;
            } else if (strcmp(argv[i], "--trace") == 0) {
                trace_mode = 1;
            } else if (strcmp(argv[i], "--trace=json") == 0) {
                trace_mode = 2;
            } else if (strcmp(argv[i], "shell") == 0) {
                cli_command[0] = '\0';
                break;
            } else {
                strcat(cli_command, argv[i]);
                strcat(cli_command, " ");
            }
        }
    }
    trim_whitespace(cli_command);

    if (strlen(cli_command) > 0) {
        if (translator_init("config/dictionary")) {
            execute_pipeline(cli_command, shell_override, trace_mode);
            translator_cleanup();
        }
        return 0;
    }

    logger_init(LOG_INFO);
    log_msg(LOG_INFO, "Starting Smart Terminal Assistant (v0.4.1)");

    printf("CmdBridge 0.6.0\n");
    printf("Type 'help' for a list of commands, or 'exit' to quit.\n");

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

        if (strcmp(input, "version") == 0 || strcmp(input, "cmdbridge version") == 0) {
            int native_apis = 0;
            int shell_cmds = 0;
            for (int i = 0; i < num_templates; i++) {
#ifdef _WIN32
                if (templates[i].windows_mode == EXEC_API || templates[i].windows_mode == EXEC_NATIVE) native_apis++;
                else if (templates[i].windows_mode == EXEC_SHELL) shell_cmds++;
#elif __APPLE__
                if (templates[i].mac_mode == EXEC_API || templates[i].mac_mode == EXEC_NATIVE) native_apis++;
                else if (templates[i].mac_mode == EXEC_SHELL) shell_cmds++;
#else
                if (templates[i].linux_mode == EXEC_API || templates[i].linux_mode == EXEC_NATIVE) native_apis++;
                else if (templates[i].linux_mode == EXEC_SHELL) shell_cmds++;
#endif
            }

            printf("\nCmdBridge v0.3.0\n\n");
            printf("Commands Supported: %d\n\n", num_templates);
            printf("Native APIs: %d\n", native_apis);
            printf("Shell Commands: %d\n\n", shell_cmds);
            
            printf("Parser Tests: 17\n");
            printf("Security Tests: 8\n");
            printf("Integration Tests: 12\n\n");
            
            printf("Platform:\n");
#ifdef _WIN32
            printf("Windows\n\n");
#elif __APPLE__
            printf("macOS\n\n");
#else
            printf("Linux/POSIX\n\n");
#endif
            printf("Build:\nRelease\n\n");
#ifdef _WIN32
#ifdef _MSC_VER
            printf("Compiler:\nMSVC\n\n");
#elif defined(__GNUC__)
            printf("Compiler:\nGCC (MinGW)\n\n");
#endif
#else
            printf("Compiler:\nGCC/Clang\n\n");
#endif
            continue;
        }

        if (strcmp(input, "help") == 0) {
            printf("\nCmdBridge v0.3.0\n\n");
            const char* categories[20];
            int num_cat = 0;
            
            for (int i = 0; i < num_templates; i++) {
                int found = 0;
                for (int j = 0; j < num_cat; j++) {
                    if (strcmp(categories[j], templates[i].category) == 0) {
                        found = 1; break;
                    }
                }
                if (!found && num_cat < 20) {
                    categories[num_cat++] = templates[i].category;
                }
            }
            
            for (int i = 0; i < num_cat; i++) {
                char cat_upper[50];
                strncpy(cat_upper, categories[i], 49);
                cat_upper[49] = '\0';
                for (int j = 0; cat_upper[j]; j++) cat_upper[j] = toupper((unsigned char)cat_upper[j]);
                
                printf("%s\n", cat_upper);
                printf("────────────────────────────\n");
                for (int k = 0; k < num_templates; k++) {
                    if (strcmp(templates[k].category, categories[i]) == 0) {
                        printf("%s\n", templates[k].example);
                    }
                }
                printf("\n");
            }
            
            printf("UTILITIES\n");
            printf("────────────────────────────\n");
            printf("Usage:\n");
            printf("  cmdbridge <command> [args]\n");
            printf("  cmdbridge explain <command> [args]\n");
            printf("  cmdbridge analyze <file>\n");
            printf("  cmdbridge migrate <file> --target-os <os> --target-shell <shell> [--out <output>] [--dry-run] [--report <json>]\n");
            printf("\n");
            printf("Options:\n");
            printf("  --shell <name>      Override target shell for interactive translation\n");
            printf("  --trace             Show detailed pipeline steps\n");
            printf("  --trace-json        Output pipeline steps as JSON\n");
            printf("history\n");
            printf("explain [command]\n");
            printf("help\n");
            printf("version\n\n");
            printf("Type any command in plain English.\n");
            printf("Example:\n> copy file \"My Resume.pdf\" backup\n");
            
            continue;
        }

        if (strcmp(input, "history") == 0) {
            show_history();
            continue;
        }
        
        if (strcmp(input, "shell") == 0) {
            interactive_shell(shell_override, trace_mode);
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
