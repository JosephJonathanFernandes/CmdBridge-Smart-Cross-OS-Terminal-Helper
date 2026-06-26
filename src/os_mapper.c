#include "os_mapper.h"
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

int map_to_os_command(const Intent *intent, const CommandTemplate *templates, int num_templates, char *out_cmd) {
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
#elif __APPLE__
    raw_cmd = matched_template->mac_cmd;
#elif __linux__
    raw_cmd = matched_template->linux_cmd;
#else
    // Fallback to linux
    raw_cmd = matched_template->linux_cmd;
#endif

    strcpy(out_cmd, raw_cmd);

    if (intent->target[0] != '\0') {
        replace_placeholder(out_cmd, "{TARGET}", intent->target);
    }
    
    // Simplification for MVP: If there's a {DEST} placeholder but we don't have it parsed in intent, 
    // we'll just leave it or replace it with empty if not used. 
    // In a real scenario we would parse DEST as well.
    // For now we assume TARGET contains everything (e.g. "report.txt documents") for move/copy.
    // Let's handle a naive split for TARGET if {DEST} is needed.
    if (strstr(out_cmd, "{DEST}") != NULL) {
        char temp_target[MAX_STR_LEN];
        strcpy(temp_target, intent->target);
        
        char *space_pos = strchr(temp_target, ' ');
        if (space_pos != NULL) {
            *space_pos = '\0'; // Split
            char *dest = space_pos + 1;
            
            // Re-replace {TARGET} with the first part
            strcpy(out_cmd, raw_cmd);
            replace_placeholder(out_cmd, "{TARGET}", temp_target);
            replace_placeholder(out_cmd, "{DEST}", dest);
        } else {
            // Missing dest
            replace_placeholder(out_cmd, "{DEST}", ".");
        }
    }

    return 1;
}
