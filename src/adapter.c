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
    
    // Add flags back
    if (ir->show_hidden) {
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "show_hidden") == 0) {
                strcat(cmd_str, " ");
                strcat(cmd_str, mapping->flags[i].flag);
                break;
            }
        }
    }
    if (ir->recursive) {
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "recursive") == 0) {
                strcat(cmd_str, " ");
                strcat(cmd_str, mapping->flags[i].flag);
                break;
            }
        }
    }
    if (ir->force) {
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "force") == 0) {
                strcat(cmd_str, " ");
                strcat(cmd_str, mapping->flags[i].flag);
                break;
            }
        }
    }
    
    // Append targets
    if (strlen(ir->target) > 0) {
        strcat(cmd_str, " ");
        strcat(cmd_str, ir->target);
    }
    if (strlen(ir->destination) > 0) {
        strcat(cmd_str, " ");
        strcat(cmd_str, ir->destination);
    }
    
    strncpy(out_cmd->native_command, cmd_str, sizeof(out_cmd->native_command) - 1);
    out_cmd->score.confidence = 100;
    out_cmd->score.compatibility = 100;
    out_cmd->score.native_score = 100;
    
    strncpy(out_cmd->target_os, target_os, sizeof(out_cmd->target_os) - 1);
    strncpy(out_cmd->target_shell, target_shell, sizeof(out_cmd->target_shell) - 1);
    
    return true;
}
