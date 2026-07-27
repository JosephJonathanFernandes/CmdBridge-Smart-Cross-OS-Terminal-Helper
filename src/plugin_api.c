#include "../include/cmdbridge_plugin.h"
#include "ir.h"
#include "logger.h"
#include "adapter.h"
#include "config.h"
#include <string.h>

static void host_log(CB_Context* ctx, CB_LogLevel level, const char* message) {
    (void)ctx;
    LogLevel internal_level = LOG_INFO;
    switch (level) {
        case CB_LOG_DEBUG: internal_level = LOG_DEBUG; break;
        case CB_LOG_INFO: internal_level = LOG_INFO; break;
        case CB_LOG_WARN: internal_level = LOG_WARN; break;
        case CB_LOG_ERROR: internal_level = LOG_ERROR; break;
    }
    log_msg(internal_level, "%s", message);
}

static void host_intent_set_operation(CB_Intent* intent, CB_Operation op) {
    ExecutionIR* ir = (ExecutionIR*)intent;
    if (ir) ir->operation = (OperationIntent)op;
}

static void host_intent_set_target(CB_Intent* intent, const char* target) {
    ExecutionIR* ir = (ExecutionIR*)intent;
    if (ir && target) {
        strncpy(ir->target, target, sizeof(ir->target) - 1);
        ir->target[sizeof(ir->target) - 1] = '\0';
    }
}

static void host_intent_set_destination(CB_Intent* intent, const char* dest) {
    ExecutionIR* ir = (ExecutionIR*)intent;
    if (ir && dest) {
        strncpy(ir->destination, dest, sizeof(ir->destination) - 1);
        ir->destination[sizeof(ir->destination) - 1] = '\0';
    }
}

static void host_intent_set_flag(CB_Intent* intent, const char* flag_name, bool value) {
    ExecutionIR* ir = (ExecutionIR*)intent;
    if (ir && flag_name) {
        if (strcmp(flag_name, "recursive") == 0) ir->recursive = value;
        else if (strcmp(flag_name, "force") == 0) ir->force = value;
        else if (strcmp(flag_name, "show_hidden") == 0) ir->show_hidden = value;
        else if (strcmp(flag_name, "long_format") == 0) ir->long_format = value;
        else if (strcmp(flag_name, "quiet") == 0) ir->quiet = value;
    }
}

static const char* host_intent_get_target(CB_Intent* intent) {
    ExecutionIR* ir = (ExecutionIR*)intent;
    return ir ? ir->target : "";
}

static const char* host_intent_get_destination(CB_Intent* intent) {
    ExecutionIR* ir = (ExecutionIR*)intent;
    return ir ? ir->destination : "";
}

static void host_result_set_command(CB_Result* result, const char* native_cmd) {
    AdaptedCommand* adapted = (AdaptedCommand*)result;
    if (adapted && native_cmd) {
        strncpy(adapted->native_command, native_cmd, sizeof(adapted->native_command) - 1);
        adapted->native_command[sizeof(adapted->native_command) - 1] = '\0';
    }
}

static void host_result_set_message(CB_Result* result, const char* msg) {
    (void)result;
    (void)msg;
}

static void host_result_set_exit_code(CB_Result* result, int code) {
    (void)result;
    (void)code;
}

static const char* host_config_get(CB_Context* ctx, const char* key) {
    (void)ctx;
    return config_get(key);
}

static void host_config_set(CB_Context* ctx, const char* key, const char* value) {
    (void)ctx;
    config_set(key, value);
}

CB_HostAPI g_host_api = {
    .log = host_log,
    .intent_set_operation = host_intent_set_operation,
    .intent_set_target = host_intent_set_target,
    .intent_set_destination = host_intent_set_destination,
    .intent_set_flag = host_intent_set_flag,
    .intent_get_target = host_intent_get_target,
    .intent_get_destination = host_intent_get_destination,
    .result_set_command = host_result_set_command,
    .result_set_message = host_result_set_message,
    .result_set_exit_code = host_result_set_exit_code,
    .config_get = host_config_get,
    .config_set = host_config_set
};
