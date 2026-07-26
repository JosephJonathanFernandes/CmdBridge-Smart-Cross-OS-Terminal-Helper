#ifndef IR_H
#define IR_H

#include <stdbool.h>

// Semantic Operations
typedef enum {
    INTENT_UNKNOWN = 0,
    INTENT_CHANGE_DIRECTORY,
    INTENT_PRINT_WORKING_DIRECTORY,
    INTENT_LIST_DIRECTORY,
    INTENT_COPY_FILE,
    INTENT_MOVE_FILE,
    INTENT_DELETE_FILE,
    INTENT_DELETE_DIRECTORY,
    INTENT_CREATE_DIRECTORY,
    INTENT_CREATE_FILE,
    INTENT_SEARCH_FILES,
    INTENT_SEARCH_TEXT,
    INTENT_VIEW_TEXT,
    INTENT_VIEW_TEXT_PAGINATED,
    INTENT_VIEW_TEXT_START,
    INTENT_VIEW_TEXT_END,
    INTENT_SET_ENV,
    INTENT_GET_ENV,
    INTENT_LIST_PROCESSES,
    INTENT_KILL_PROCESS
} OperationIntent;

// Capabilities (Feature Negotiation)
typedef enum {
    CAPABILITY_UNSUPPORTED = 0,
    CAPABILITY_PARTIALLY_SUPPORTED,
    CAPABILITY_SUPPORTED
} CapabilitySupport;

// Reason Codes for Explain Mode
typedef enum {
    REASON_DIRECT_MAPPING = 0,
    REASON_APPROXIMATE_TRANSLATION,
    REASON_FLAG_NOT_AVAILABLE,
    REASON_UNSUPPORTED_PERMISSION_MODEL,
    REASON_OS_LIMITATION,
    REASON_UNKNOWN
} ReasonCode;

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
