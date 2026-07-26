#include "capability.h"
#include <stdio.h>

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
    
    // For now, if we found it, it is supported.
    // In future versions, we can check specific flag support for "PARTIALLY_SUPPORTED"
    return CAPABILITY_SUPPORTED;
}
