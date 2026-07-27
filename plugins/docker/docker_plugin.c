#include "cmdbridge_plugin.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const CB_HostAPI* g_host = NULL;

static int docker_init(CB_Context* ctx, const CB_HostAPI* host_api) {
    g_host = host_api;
    g_host->log(ctx, CB_LOG_INFO, "[docker_plugin] Initialized.");
    return 0;
}

static void docker_cleanup(CB_Context* ctx) {
    (void)ctx;
}

static bool docker_claims(CB_Context* ctx, const char* raw_command) {
    (void)ctx;
    // Simple check if it starts with "docker "
    return (strncmp(raw_command, "docker ", 7) == 0);
}

static int docker_translate(CB_Context* ctx, const char* raw_command, CB_Intent* intent) {
    g_host->log(ctx, CB_LOG_DEBUG, "[docker_plugin] Translating docker command.");
    
    // In a real plugin, we would parse "docker ps", "docker run", etc.
    g_host->intent_set_operation(intent, CB_OPERATION_CUSTOM_PLUGIN);
    
    // We pass the raw command as the target for execution
    g_host->intent_set_target(intent, raw_command);
    return 1;
}

static int docker_execute(CB_Context* ctx, CB_Intent* intent, CB_Result* result) {
    const char* target = g_host->intent_get_target(intent);
    g_host->log(ctx, CB_LOG_INFO, "[docker_plugin] Executing Docker command natively...");
    
    // Directly run the docker command
    int exit_code = system(target);
    g_host->result_set_exit_code(result, exit_code);
    return 1;
}

static const CB_Capability docker_caps[] = {
    {"docker", 100}
};

static CB_Plugin g_docker_plugin = {
    .struct_size = sizeof(CB_Plugin),
    .name = "Docker Plugin",
    .version = "1.0.0",
    .author = "CmdBridge Core Team",
    .license = "MIT",
    .description = "Translates and routes docker commands.",
    .claims = docker_caps,
    .num_claims = 1,
    .init = docker_init,
    .cleanup = docker_cleanup,
    .claims_command = docker_claims,
    .translate = docker_translate,
    .execute = docker_execute
};

CB_PLUGIN_EXPORT CB_Plugin* cb_register_plugin(uint32_t host_api_version) {
    if (host_api_version != CMDBRIDGE_API_VERSION) {
        return NULL;
    }
    return &g_docker_plugin;
}
