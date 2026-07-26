#include "capability.h"
#include <stdio.h>
#include <string.h>

CapabilitySupport negotiate_capability(const ExecutionIR* ir, const char* target_os, const char* target_shell, const char* dictionary_dir) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s_%s.json", dictionary_dir, target_os, target_shell);
    
    Dictionary dict;
    if (!load_dictionary(path, &dict)) {
        return CAPABILITY_UNSUPPORTED;
    }
    
    const CommandMapping* mapping = find_intent_mapping(&dict, ir->operation);
    if (!mapping) {
        return CAPABILITY_UNSUPPORTED;
    }
    
    // Check flags for partial support
    bool fully_supported = true;

    if (ir->show_hidden) {
        bool found = false;
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "show_hidden") == 0) {
                found = true; break;
            }
        }
        if (!found) fully_supported = false;
    }
    if (ir->long_format) {
        bool found = false;
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "long_format") == 0) {
                found = true; break;
            }
        }
        if (!found) fully_supported = false;
    }
    if (ir->recursive) {
        bool found = false;
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "recursive") == 0) {
                found = true; break;
            }
        }
        if (!found) fully_supported = false;
    }
    if (ir->force) {
        bool found = false;
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "force") == 0) {
                found = true; break;
            }
        }
        if (!found) fully_supported = false;
    }
    if (ir->quiet) {
        bool found = false;
        for (int i = 0; i < mapping->flag_count; i++) {
            if (strcmp(mapping->flags[i].semantic_meaning, "quiet") == 0) {
                found = true; break;
            }
        }
        if (!found) fully_supported = false;
    }

    if (!fully_supported) {
        return CAPABILITY_PARTIALLY_SUPPORTED;
    }

    return CAPABILITY_SUPPORTED;
}
