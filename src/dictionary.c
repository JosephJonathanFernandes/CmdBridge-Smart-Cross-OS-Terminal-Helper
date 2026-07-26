#include "dictionary.h"
#include "logger.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

OperationIntent parse_intent_string(const char* str) {
    if (strcmp(str, "INTENT_CHANGE_DIRECTORY") == 0) return INTENT_CHANGE_DIRECTORY;
    if (strcmp(str, "INTENT_PRINT_WORKING_DIRECTORY") == 0) return INTENT_PRINT_WORKING_DIRECTORY;
    if (strcmp(str, "INTENT_LIST_DIRECTORY") == 0) return INTENT_LIST_DIRECTORY;
    if (strcmp(str, "INTENT_COPY_FILE") == 0) return INTENT_COPY_FILE;
    if (strcmp(str, "INTENT_MOVE_FILE") == 0) return INTENT_MOVE_FILE;
    if (strcmp(str, "INTENT_DELETE_FILE") == 0) return INTENT_DELETE_FILE;
    if (strcmp(str, "INTENT_DELETE_DIRECTORY") == 0) return INTENT_DELETE_DIRECTORY;
    if (strcmp(str, "INTENT_CREATE_DIRECTORY") == 0) return INTENT_CREATE_DIRECTORY;
    if (strcmp(str, "INTENT_CREATE_FILE") == 0) return INTENT_CREATE_FILE;
    if (strcmp(str, "INTENT_SEARCH_FILES") == 0) return INTENT_SEARCH_FILES;
    if (strcmp(str, "INTENT_SEARCH_TEXT") == 0) return INTENT_SEARCH_TEXT;
    if (strcmp(str, "INTENT_VIEW_TEXT") == 0) return INTENT_VIEW_TEXT;
    if (strcmp(str, "INTENT_VIEW_TEXT_PAGINATED") == 0) return INTENT_VIEW_TEXT_PAGINATED;
    if (strcmp(str, "INTENT_VIEW_TEXT_START") == 0) return INTENT_VIEW_TEXT_START;
    if (strcmp(str, "INTENT_VIEW_TEXT_END") == 0) return INTENT_VIEW_TEXT_END;
    if (strcmp(str, "INTENT_SET_ENV") == 0) return INTENT_SET_ENV;
    if (strcmp(str, "INTENT_GET_ENV") == 0) return INTENT_GET_ENV;
    if (strcmp(str, "INTENT_LIST_PROCESSES") == 0) return INTENT_LIST_PROCESSES;
    if (strcmp(str, "INTENT_KILL_PROCESS") == 0) return INTENT_KILL_PROCESS;
    return INTENT_UNKNOWN;
}

static char* read_file_content(const char* filepath) {
    FILE *f = fopen(filepath, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *data = malloc(length + 1);
    if (data) {
        fread(data, 1, length, f);
        data[length] = '\0';
    }
    fclose(f);
    return data;
}

bool load_dictionary(const char* filepath, Dictionary* out_dict) {
    char *json_data = read_file_content(filepath);
    if (!json_data) {
        log_msg(LOG_ERROR, "Failed to read dictionary file: %s", filepath);
        return false;
    }
    
    cJSON *json = cJSON_Parse(json_data);
    free(json_data);
    if (!json) {
        log_msg(LOG_ERROR, "Failed to parse JSON in dictionary: %s", filepath);
        return false;
    }
    
    cJSON *version = cJSON_GetObjectItemCaseSensitive(json, "schema_version");
    cJSON *shell = cJSON_GetObjectItemCaseSensitive(json, "shell");
    cJSON *os = cJSON_GetObjectItemCaseSensitive(json, "os");
    
    if (cJSON_IsNumber(version)) out_dict->schema_version = version->valueint;
    if (cJSON_IsString(shell)) strncpy(out_dict->shell, shell->valuestring, sizeof(out_dict->shell)-1);
    if (cJSON_IsString(os)) strncpy(out_dict->os, os->valuestring, sizeof(out_dict->os)-1);
    
    out_dict->command_count = 0;
    
    cJSON *commands = cJSON_GetObjectItemCaseSensitive(json, "commands");
    if (cJSON_IsObject(commands)) {
        cJSON *command_item = NULL;
        cJSON_ArrayForEach(command_item, commands) {
            if (out_dict->command_count >= 64) break;
            
            CommandMapping *mapping = &out_dict->commands[out_dict->command_count++];
            
            cJSON *intent_str = cJSON_GetObjectItemCaseSensitive(command_item, "intent");
            cJSON *cmd_str = cJSON_GetObjectItemCaseSensitive(command_item, "command");
            
            if (cJSON_IsString(intent_str)) mapping->intent = parse_intent_string(intent_str->valuestring);
            if (cJSON_IsString(cmd_str)) strncpy(mapping->command, cmd_str->valuestring, sizeof(mapping->command)-1);
            
            mapping->flag_count = 0;
            cJSON *flags = cJSON_GetObjectItemCaseSensitive(command_item, "flags");
            if (cJSON_IsObject(flags)) {
                cJSON *flag_item = NULL;
                cJSON_ArrayForEach(flag_item, flags) {
                    if (mapping->flag_count >= 16) break;
                    if (cJSON_IsString(flag_item)) {
                        FlagMapping *fm = &mapping->flags[mapping->flag_count++];
                        strncpy(fm->flag, flag_item->string, sizeof(fm->flag)-1); // key
                        strncpy(fm->semantic_meaning, flag_item->valuestring, sizeof(fm->semantic_meaning)-1); // value
                    }
                }
            }
        }
    }
    
    cJSON_Delete(json);
    return true;
}

const CommandMapping* find_command_mapping(const Dictionary* dict, const char* raw_command) {
    for (int i = 0; i < dict->command_count; i++) {
        if (strcmp(dict->commands[i].command, raw_command) == 0) {
            return &dict->commands[i];
        }
    }
    return NULL;
}

const CommandMapping* find_intent_mapping(const Dictionary* dict, OperationIntent intent) {
    for (int i = 0; i < dict->command_count; i++) {
        if (dict->commands[i].intent == intent) {
            return &dict->commands[i];
        }
    }
    return NULL;
}
