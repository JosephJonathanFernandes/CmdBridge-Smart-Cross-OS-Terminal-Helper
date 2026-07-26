#ifndef ADAPTER_H
#define ADAPTER_H

#include "ir.h"
#include "dictionary.h"

// The scoring system metrics
typedef struct {
    int confidence; // 0 to 100
    int compatibility; // 0 to 100
    int native_score; // 0 to 100
    ReasonCode reason;
    char suggested_alternative[256];
} AdapterScore;

// Represents an adapted native command mapping
typedef struct {
    char native_command[512];
    AdapterScore score;
    char target_shell[32];
    char target_os[32];
} AdaptedCommand;

// Adapts an ExecutionIR to a native string for the specified OS and Shell
bool adapt_ir_to_native(const ExecutionIR* ir, const char* target_os, const char* target_shell, const char* dictionary_dir, AdaptedCommand* out_cmd);

#endif // ADAPTER_H
