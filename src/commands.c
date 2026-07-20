#include "commands.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static ExecutionMode parse_mode(const char *mode_str) {
    if (mode_str) {
        if (strcmp(mode_str, "NATIVE") == 0) {
            return EXEC_NATIVE;
        } else if (strcmp(mode_str, "API") == 0) {
            return EXEC_API;
        }
    }
    return EXEC_SHELL;
}

int load_command_templates(CommandTemplate *templates) {
    int count = 0;
    
    // Try to open relative to project root first, then fallback to relative from build directory
    const char *config_path = "config/commands.conf";
    FILE *file = fopen(config_path, "r");
    if (!file) {
        config_path = "../config/commands.conf";
        file = fopen(config_path, "r");
    }

    if (!file) {
        log_msg(LOG_ERROR, "Failed to open command configuration file in config/ or ../config/");
        return 0;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file) && count < MAX_TEMPLATES) {
        // Remove trailing newline
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0 || line[0] == '#') continue;

        // Format: category|example|action|object|linux_mode|linux|win_mode|windows|mac_mode|mac
        char *token = strtok(line, "|");
        if (token) strncpy(templates[count].category, token, sizeof(templates[count].category) - 1);
        
        token = strtok(NULL, "|");
        if (token) strncpy(templates[count].example, token, sizeof(templates[count].example) - 1);
        
        token = strtok(NULL, "|");
        if (token) strncpy(templates[count].action, token, sizeof(templates[count].action) - 1);
        
        token = strtok(NULL, "|");
        if (token) strncpy(templates[count].object, token, sizeof(templates[count].object) - 1);
        
        token = strtok(NULL, "|");
        templates[count].linux_mode = parse_mode(token);
        token = strtok(NULL, "|");
        if (token) strncpy(templates[count].linux_cmd, token, sizeof(templates[count].linux_cmd) - 1);
        
        token = strtok(NULL, "|");
        templates[count].windows_mode = parse_mode(token);
        token = strtok(NULL, "|");
        if (token) strncpy(templates[count].windows_cmd, token, sizeof(templates[count].windows_cmd) - 1);
        
        token = strtok(NULL, "|");
        templates[count].mac_mode = parse_mode(token);
        token = strtok(NULL, "|");
        if (token) strncpy(templates[count].mac_cmd, token, sizeof(templates[count].mac_cmd) - 1);

        log_msg(LOG_DEBUG, "Loaded command template: [%s] %s %s", templates[count].category, templates[count].action, templates[count].object);
        count++;
    }

    fclose(file);
    log_msg(LOG_INFO, "Loaded %d command templates from %s", count, config_path);
    return count;
}
