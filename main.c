#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "intent.h"
#include "parser.h"
#include "commands.h"
#include "os_mapper.h"
#include "safety.h"
#include "utils.h"

int main() {
    printf("Smart Terminal Assistant (MVP)\n");
    printf("Type 'exit' to quit.\n");

    CommandTemplate templates[MAX_TEMPLATES];
    int num_templates = load_command_templates(templates);
    printf("Loaded %d command templates.\n", num_templates);

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

        Intent intent;
        parse_input(input, &intent);
        
        // Uncomment to debug parsing:
        // print_intent(&intent);

        if (intent.action[0] == '\0' || intent.object[0] == '\0') {
            printf("Could not understand command. Please try phrasing differently (e.g. 'find pdf files').\n");
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
            
            // Print OS specific command prefix if we want to be fancy
#ifdef _WIN32
            printf("Windows: %s\n", mapped_cmd);
#elif __APPLE__
            printf("macOS: %s\n", mapped_cmd);
#elif __linux__
            printf("Linux: %s\n", mapped_cmd);
#else
            printf("OS-mapped: %s\n", mapped_cmd);
#endif

            // Note: We don't execute it, we just output it as per the project goal.
        } else {
            printf("No matching command template found for action '%s' and object '%s'.\n", intent.action, intent.object);
        }
    }

    printf("Goodbye!\n");
    return 0;
}
