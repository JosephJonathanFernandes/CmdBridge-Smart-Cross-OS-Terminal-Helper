#include "adapter.h"
#include <stdio.h>
#include <string.h>

bool adapt_ir_to_native(const ExecutionIR* ir, const char* target_os, const char* target_shell, const char* dictionary_dir, AdaptedCommand* out_cmd) {
    memset(out_cmd, 0, sizeof(AdaptedCommand));
    
    char path[512];
    snprintf(path, sizeof(path), "%s/%s_%s.json", dictionary_dir, target_os, target_shell);
    
    Dictionary dict;
    if (!load_dictionary(path, &dict)) {
        return false;
    }
    
    const CommandMapping* mapping = find_intent_mapping(&dict, ir->operation);
    if (!mapping) {
        return false;
    }
    
    // Start building native command
    char cmd_str[512];
    snprintf(cmd_str, sizeof(cmd_str), "%s", mapping->command);
    
    // Check flags
    bool missing_flag = false;
    char missing_flag_name[32] = {0};

    if (ir->show_hidden) {
        bool found = false;
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "show_hidden") == 0) {
                strcat(cmd_str, " ");
                strcat(cmd_str, mapping->flags[i].flag);
                found = true;
                break;
            }
        }
        if (!found) { missing_flag = true; strcpy(missing_flag_name, "show_hidden"); }
    }
    if (ir->long_format) {
        bool found = false;
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "long_format") == 0) {
                strcat(cmd_str, " ");
                strcat(cmd_str, mapping->flags[i].flag);
                found = true;
                break;
            }
        }
        if (!found) { missing_flag = true; strcpy(missing_flag_name, "long_format"); }
    }
    if (ir->recursive) {
        bool found = false;
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "recursive") == 0) {
                strcat(cmd_str, " ");
                strcat(cmd_str, mapping->flags[i].flag);
                found = true;
                break;
            }
        }
        if (!found) { missing_flag = true; strcpy(missing_flag_name, "recursive"); }
    }
    if (ir->force) {
        bool found = false;
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "force") == 0) {
                strcat(cmd_str, " ");
                strcat(cmd_str, mapping->flags[i].flag);
                found = true;
                break;
            }
        }
        if (!found) { missing_flag = true; strcpy(missing_flag_name, "force"); }
    }
    if (ir->quiet) {
        bool found = false;
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "quiet") == 0) {
                strcat(cmd_str, " ");
                strcat(cmd_str, mapping->flags[i].flag);
                found = true;
                break;
            }
        }
        if (!found) { missing_flag = true; strcpy(missing_flag_name, "quiet"); }
    }
    
    // Build final arguments
    if (strlen(ir->target) > 0) {
        strcat(cmd_str, " ");
        int has_space = (strchr(ir->target, ' ') != NULL);
        if (has_space) strcat(cmd_str, "\"");
        strcat(cmd_str, ir->target);
        if (has_space) strcat(cmd_str, "\"");
    }
    
    if (strlen(ir->destination) > 0) {
        strcat(cmd_str, " ");
        int has_space = (strchr(ir->destination, ' ') != NULL);
        if (has_space) strcat(cmd_str, "\"");
        strcat(cmd_str, ir->destination);
        if (has_space) strcat(cmd_str, "\"");
    }
    
    strncpy(out_cmd->native_command, cmd_str, sizeof(out_cmd->native_command) - 1);
    
    if (missing_flag) {
        out_cmd->score.confidence = 70;
        out_cmd->score.compatibility = 70;
        out_cmd->score.native_score = 70;
        out_cmd->score.reason = REASON_FLAG_NOT_AVAILABLE;
        snprintf(out_cmd->score.suggested_alternative, sizeof(out_cmd->score.suggested_alternative), "No direct flag for '%s'. Check native docs.", missing_flag_name);
    } else {
        out_cmd->score.confidence = 100;
        out_cmd->score.compatibility = 100;
        out_cmd->score.native_score = 100;
        out_cmd->score.reason = REASON_DIRECT_MAPPING;
        strcpy(out_cmd->score.suggested_alternative, "None required.");
    }
    
    strncpy(out_cmd->target_os, target_os, sizeof(out_cmd->target_os) - 1);
    strncpy(out_cmd->target_shell, target_shell, sizeof(out_cmd->target_shell) - 1);
    
    return true;
}
