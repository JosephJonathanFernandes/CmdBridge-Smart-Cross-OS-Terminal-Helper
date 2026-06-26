#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_CMD_LEN 256
#define MAX_TEMPLATES 50

typedef struct {
    char action[50];
    char object[50];
    char linux_cmd[MAX_CMD_LEN];
    char windows_cmd[MAX_CMD_LEN];
    char mac_cmd[MAX_CMD_LEN];
} CommandTemplate;

/**
 * Loads predefined command templates.
 * Returns the number of loaded templates.
 */
int load_command_templates(CommandTemplate *templates);

#endif // COMMANDS_H
