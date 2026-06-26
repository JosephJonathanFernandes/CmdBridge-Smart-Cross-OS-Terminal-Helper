#include "commands.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int load_command_templates(CommandTemplate *templates) {
    int count = 0;
    
    // In a real application, the path should be read from .env
    // We hardcode the path relative to CWD for the MVP
    const char *config_path = "config/commands.conf";
    FILE *file = fopen(config_path, "r");
    if (!file) {
        log_msg(LOG_ERROR, "Failed to open command configuration file: %s", config_path);
        return 0;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file) && count < MAX_TEMPLATES) {
        // Remove trailing newline
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0 || line[0] == '#') continue;

        // Simple split by '|'
        // Format: action|object|linux|windows|mac
        char *token = strtok(line, "|");
        if (token) strncpy(templates[count].action, token, sizeof(templates[count].action) - 1);
        
        token = strtok(NULL, "|");
        if (token) strncpy(templates[count].object, token, sizeof(templates[count].object) - 1);
        
        token = strtok(NULL, "|");
        if (token) strncpy(templates[count].linux_cmd, token, sizeof(templates[count].linux_cmd) - 1);
        
        token = strtok(NULL, "|");
        if (token) strncpy(templates[count].windows_cmd, token, sizeof(templates[count].windows_cmd) - 1);
        
        token = strtok(NULL, "|");
        if (token) strncpy(templates[count].mac_cmd, token, sizeof(templates[count].mac_cmd) - 1);

        log_msg(LOG_DEBUG, "Loaded command template: %s %s", templates[count].action, templates[count].object);
        count++;
    }

    fclose(file);
    log_msg(LOG_INFO, "Loaded %d command templates from %s", count, config_path);
    return count;
}
