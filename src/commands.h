#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_CMD_LEN 256
#define MAX_TEMPLATES 50

typedef enum {
    EXEC_SHELL,
    EXEC_NATIVE,
    EXEC_API
} ExecutionMode;

typedef struct {
    char action[50];
    char object[50];
    ExecutionMode linux_mode;
    char linux_cmd[MAX_CMD_LEN];
    ExecutionMode windows_mode;
    char windows_cmd[MAX_CMD_LEN];
    ExecutionMode mac_mode;
    char mac_cmd[MAX_CMD_LEN];
} CommandTemplate;

/**
 * Loads predefined command templates.
 * Returns the number of loaded templates.
 */
int load_command_templates(CommandTemplate *templates);

#endif // COMMANDS_H
