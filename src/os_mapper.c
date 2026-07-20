#include "os_mapper.h"
#include "utils.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>

void replace_placeholder(char *str, const char *placeholder, const char *replacement) {
    char buffer[1024];
    char *insert_point = &buffer[0];
    const char *tmp = str;
    size_t len_placeholder = strlen(placeholder);
    size_t len_replacement = strlen(replacement);

    while (1) {
        const char *p = strstr(tmp, placeholder);
        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;
        memcpy(insert_point, replacement, len_replacement);
        insert_point += len_replacement;
        tmp = p + len_placeholder;
    }
    strcpy(str, buffer);
}

int map_to_os_command(const Intent *intent, const CommandTemplate *templates, int num_templates, char *out_cmd, ExecutionMode *out_mode) {
    if (!intent || !templates || !out_cmd) return 0;
    out_cmd[0] = '\0';

    const CommandTemplate *matched_template = NULL;

    for (int i = 0; i < num_templates; i++) {
        if (strcmp(intent->action, templates[i].action) == 0 &&
            strcmp(intent->object, templates[i].object) == 0) {
            matched_template = &templates[i];
            break;
        }
    }

    if (!matched_template) {
        return 0; // Not found
    }

    const char *raw_cmd = NULL;

#ifdef _WIN32
    raw_cmd = matched_template->windows_cmd;
    if (out_mode) *out_mode = matched_template->windows_mode;
#elif __APPLE__
    raw_cmd = matched_template->mac_cmd;
    if (out_mode) *out_mode = matched_template->mac_mode;
#elif __linux__
    raw_cmd = matched_template->linux_cmd;
    if (out_mode) *out_mode = matched_template->linux_mode;
#else
    // Fallback to linux
    raw_cmd = matched_template->linux_cmd;
    if (out_mode) *out_mode = matched_template->linux_mode;
#endif

    strcpy(out_cmd, raw_cmd);
    
    // Windows SHELL execution must strictly validate input to prevent injection
#ifdef _WIN32
    if (*out_mode == EXEC_SHELL) {
        for (int i = 0; i < intent->argc; i++) {
            if (!validate_input_whitelist(intent->args[i])) {
                log_msg(LOG_WARN, "Input validation failed: invalid characters in argument");
                return 0;
            }
        }
    }
#endif

    // Replace ARG0 to ARG9
    for (int i = 0; i < intent->argc && i < MAX_ARGS; i++) {
        char placeholder[16];
        sprintf(placeholder, "{ARG%d}", i);
        if (strstr(out_cmd, placeholder)) {
            replace_placeholder(out_cmd, placeholder, intent->args[i]);
        }
    }

    return 1;
}
