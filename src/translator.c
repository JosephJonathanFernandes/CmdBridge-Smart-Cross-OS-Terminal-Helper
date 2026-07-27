#include "translator.h"
#include "dictionary.h"
#include "logger.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>

#define MAX_DICTIONARIES 4
static Dictionary loaded_dicts[MAX_DICTIONARIES];
static int dict_count = 0;

bool translator_init(const char* dictionary_dir) {
    dict_count = 0;
    char path[512];
    
    // Load known dictionaries
    snprintf(path, sizeof(path), "%s/linux_bash.json", dictionary_dir);
    if (load_dictionary(path, &loaded_dicts[dict_count])) dict_count++;
    
    snprintf(path, sizeof(path), "%s/windows_cmd.json", dictionary_dir);
    if (load_dictionary(path, &loaded_dicts[dict_count])) dict_count++;
    
    snprintf(path, sizeof(path), "%s/windows_powershell.json", dictionary_dir);
    if (load_dictionary(path, &loaded_dicts[dict_count])) dict_count++;
    
    log_msg(LOG_INFO, "Loaded %d dictionaries.", dict_count);
    return dict_count > 0;
}

void translator_cleanup() {
    // Nothing dynamic allocated inside Dictionary struct right now, just zero count
    dict_count = 0;
}

bool translate_input_to_ir(const char* raw_input, ExecutionIR* out_ir) {
    memset(out_ir, 0, sizeof(ExecutionIR));
    out_ir->operation = INTENT_UNKNOWN;
    
    char *argv[100];
    int argc = 0;
    tokenize_command(raw_input, argv, &argc);
    
    if (argc == 0) {
        free_tokens(argv, argc);
        return false;
    }
    
    char *token = argv[0];
    
    // Find matching command across all dictionaries
    const CommandMapping* matched_cmd = NULL;
    const Dictionary* matched_dict = NULL;
    
    for (int i = 0; i < dict_count; i++) {
        matched_cmd = find_command_mapping(&loaded_dicts[i], token);
        if (matched_cmd) {
            matched_dict = &loaded_dicts[i];
            break;
        }
    }
    
    if (matched_cmd && matched_dict) {
        out_ir->operation = matched_cmd->intent;
        strncpy(out_ir->input_shell, matched_dict->shell, sizeof(out_ir->input_shell) - 1);
        
        // Parse the rest for targets and flags
        for (int arg_idx = 1; arg_idx < argc; arg_idx++) {
            token = argv[arg_idx];
            if (token[0] == '-' || token[0] == '/') { // Flag
                for (int f = 0; f < matched_cmd->flag_count; f++) {
                    if (strcmp(matched_cmd->flags[f].flag, token) == 0) {
                        const char* meaning = matched_cmd->flags[f].semantic_meaning;
                        if (strcmp(meaning, "show_hidden") == 0) out_ir->show_hidden = true;
                        else if (strcmp(meaning, "long_format") == 0) out_ir->long_format = true;
                        else if (strcmp(meaning, "force") == 0) out_ir->force = true;
                        else if (strcmp(meaning, "recursive") == 0) out_ir->recursive = true;
                        else if (strcmp(meaning, "quiet") == 0) out_ir->quiet = true;
                    }
                }
            } else { // Target
                if (strlen(out_ir->target) == 0) {
                    strncpy(out_ir->target, token, sizeof(out_ir->target) - 1);
                } else if (strlen(out_ir->destination) == 0) {
                    strncpy(out_ir->destination, token, sizeof(out_ir->destination) - 1);
                }
            }
        }
        
        // Set default target to "." if empty (e.g. for ls)
        if (strlen(out_ir->target) == 0 && out_ir->operation == INTENT_LIST_DIRECTORY) {
            strcpy(out_ir->target, ".");
        }
        
        free_tokens(argv, argc);
        return true;
    }
    
    free_tokens(argv, argc);
    return false;
}
