#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "ir.h"
#include <stdbool.h>

// Represents a flag mapping for a command
typedef struct {
    char flag[32];
    char semantic_meaning[64];
} FlagMapping;

// Represents a command mapping in a specific shell
typedef struct {
    OperationIntent intent;
    char command[64];
    FlagMapping flags[16];
    int flag_count;
} CommandMapping;

// Represents a loaded dictionary for a specific shell
typedef struct {
    int schema_version;
    char shell[32];
    char os[32];
    
    CommandMapping commands[64];
    int command_count;
} Dictionary;

// Loads a JSON dictionary file from disk
bool load_dictionary(const char* filepath, Dictionary* out_dict);

// Finds a command mapping by raw command string in the dictionary
const CommandMapping* find_command_mapping(const Dictionary* dict, const char* raw_command);

// Finds an intent's native translation in the dictionary
const CommandMapping* find_intent_mapping(const Dictionary* dict, OperationIntent intent);

// Maps a raw intent string (e.g. "INTENT_LIST_DIRECTORY") to enum
OperationIntent parse_intent_string(const char* str);

#endif // DICTIONARY_H
