#ifndef IR_H
#define IR_H

#include <stdbool.h>

// Semantic Operations
typedef enum {
    INTENT_UNKNOWN = 0,
    INTENT_LIST_DIRECTORY,
    INTENT_DELETE_FILE,
    INTENT_DELETE_DIRECTORY,
    INTENT_COPY_FILE,
    INTENT_MOVE_FILE
} OperationIntent;

// Capabilities (Feature Negotiation)
typedef enum {
    CAPABILITY_UNSUPPORTED = 0,
    CAPABILITY_PARTIALLY_SUPPORTED,
    CAPABILITY_SUPPORTED
} CapabilitySupport;

// Safety Risk Levels
typedef enum {
    IR_RISK_SAFE = 0,
    IR_RISK_WARNING,
    IR_RISK_DANGEROUS,
    IR_RISK_CRITICAL
} SafetyRisk;

// Execution IR
typedef struct {
    OperationIntent operation;
    char target[512];
    char destination[512]; // For copy/move
    
    // Semantic Flags
    bool recursive;
    bool force;
    bool show_hidden;
    bool long_format;
    bool quiet;
    
    // Metadata
    char input_shell[64];
    char host_shell[64];
    
} ExecutionIR;

#endif // IR_H
