#ifndef CMDBRIDGE_PLUGIN_H
#define CMDBRIDGE_PLUGIN_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CMDBRIDGE_API_VERSION 1

// Opaque handles
typedef struct CB_Context_t CB_Context;
typedef struct CB_Intent_t CB_Intent;
typedef struct CB_Result_t CB_Result;

// Public Operations Enum
typedef enum {
    CB_OPERATION_UNKNOWN = 0,
    CB_OPERATION_CHANGE_DIRECTORY,
    CB_OPERATION_PRINT_WORKING_DIRECTORY,
    CB_OPERATION_LIST_DIRECTORY,
    CB_OPERATION_COPY_FILE,
    CB_OPERATION_MOVE_FILE,
    CB_OPERATION_DELETE_FILE,
    CB_OPERATION_DELETE_DIRECTORY,
    CB_OPERATION_CREATE_DIRECTORY,
    CB_OPERATION_CREATE_FILE,
    CB_OPERATION_SEARCH_FILES,
    CB_OPERATION_SEARCH_TEXT,
    CB_OPERATION_VIEW_TEXT,
    CB_OPERATION_VIEW_TEXT_PAGINATED,
    CB_OPERATION_VIEW_TEXT_START,
    CB_OPERATION_VIEW_TEXT_END,
    CB_OPERATION_SET_ENV,
    CB_OPERATION_GET_ENV,
    CB_OPERATION_LIST_PROCESSES,
    CB_OPERATION_KILL_PROCESS,
    CB_OPERATION_PRINT_TEXT,
    CB_OPERATION_CUSTOM_PLUGIN
} CB_Operation;

// Log Levels
typedef enum {
    CB_LOG_DEBUG = 0,
    CB_LOG_INFO,
    CB_LOG_WARN,
    CB_LOG_ERROR
} CB_LogLevel;

// Host API (Passed to the plugin during init)
typedef struct {
    void (*log)(CB_Context* ctx, CB_LogLevel level, const char* message);
    
    void (*intent_set_operation)(CB_Intent* intent, CB_Operation op);
    void (*intent_set_target)(CB_Intent* intent, const char* target);
    void (*intent_set_destination)(CB_Intent* intent, const char* dest);
    void (*intent_set_flag)(CB_Intent* intent, const char* flag_name, bool value);
    const char* (*intent_get_target)(CB_Intent* intent);
    const char* (*intent_get_destination)(CB_Intent* intent);
    
    void (*result_set_command)(CB_Result* result, const char* native_cmd);
    void (*result_set_message)(CB_Result* result, const char* msg);
    void (*result_set_exit_code)(CB_Result* result, int code);
    
    // Configuration
    const char* (*config_get)(CB_Context* ctx, const char* key);
    void (*config_set)(CB_Context* ctx, const char* key, const char* value);
} CB_HostAPI;

// Capabilities
typedef struct {
    const char* name;
    uint32_t priority;
} CB_Capability;

// Plugin Definition
typedef struct {
    uint32_t struct_size; // MUST be set to sizeof(CB_Plugin)
    
    // Metadata
    const char* name;
    const char* version;
    const char* author;
    const char* license;
    const char* description;
    
    // Capabilities
    const CB_Capability* claims;
    uint32_t num_claims;
    
    // Lifecycle
    int (*init)(CB_Context* ctx, const CB_HostAPI* host_api);
    void (*cleanup)(CB_Context* ctx);
    
    // Returns true if the plugin wishes to handle this raw command
    bool (*claims_command)(CB_Context* ctx, const char* raw_command);
    
    // Translates the raw command into the opaque CB_Intent 
    int (*translate)(CB_Context* ctx, const char* raw_command, CB_Intent* intent);
    
    // Executes the intent. Called only if the intent passes Host safety checks.
    int (*execute)(CB_Context* ctx, CB_Intent* intent, CB_Result* result);
} CB_Plugin;

// Plugin Export
#ifdef _WIN32
  #define CB_PLUGIN_EXPORT __declspec(dllexport)
#else
  #define CB_PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

// The single required export for any plugin
CB_PLUGIN_EXPORT CB_Plugin* cb_register_plugin(uint32_t host_api_version);

#ifdef __cplusplus
}
#endif

#endif // CMDBRIDGE_PLUGIN_H
