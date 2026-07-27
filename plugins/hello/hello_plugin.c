#include "cmdbridge_plugin.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const CB_HostAPI* g_host = NULL;

static int hello_init(CB_Context* ctx, const CB_HostAPI* host_api) {
    g_host = host_api;
    g_host->log(ctx, CB_LOG_INFO, "[hello_plugin] Initialized successfully.");
    return 0;
}

static void hello_cleanup(CB_Context* ctx) {
    g_host->log(ctx, CB_LOG_INFO, "[hello_plugin] Cleaning up.");
}

static bool hello_claims(CB_Context* ctx, const char* raw_command) {
    (void)ctx;
    return (strncmp(raw_command, "hello plugin", 12) == 0);
}

static int hello_translate(CB_Context* ctx, const char* raw_command, CB_Intent* intent) {
    g_host->log(ctx, CB_LOG_DEBUG, "[hello_plugin] Translating command.");
    g_host->intent_set_operation(intent, CB_OPERATION_CUSTOM_PLUGIN);
    g_host->intent_set_target(intent, "hello plugin executed");
    return 1;
}

static int hello_execute(CB_Context* ctx, CB_Intent* intent, CB_Result* result) {
    g_host->log(ctx, CB_LOG_INFO, "[hello_plugin] Hello from the CmdBridge Plugin Ecosystem!");
    g_host->result_set_exit_code(result, 0);
    return 1;
}

static const CB_Capability hello_caps[] = {
    {"hello", 100}
};

static CB_Plugin g_hello_plugin = {
    .struct_size = sizeof(CB_Plugin),
    .name = "Hello Plugin",
    .version = "1.0.0",
    .author = "CmdBridge Core Team",
    .license = "MIT",
    .description = "A demonstration plugin showcasing the CmdBridge Plugin API.",
    .claims = hello_caps,
    .num_claims = 1,
    .init = hello_init,
    .cleanup = hello_cleanup,
    .claims_command = hello_claims,
    .translate = hello_translate,
    .execute = hello_execute
};

CB_PLUGIN_EXPORT CB_Plugin* cb_register_plugin(uint32_t host_api_version) {
    if (host_api_version != CMDBRIDGE_API_VERSION) {
        return NULL;
    }
    return &g_hello_plugin;
}
