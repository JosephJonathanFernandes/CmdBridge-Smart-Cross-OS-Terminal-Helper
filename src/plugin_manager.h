#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <stdbool.h>
#include "ir.h"
#include "../include/cmdbridge_plugin.h"

// Define a struct to hold loaded plugin information
typedef struct {
    void* os_handle;        // HMODULE on Windows, void* on POSIX
    CB_Plugin* plugin;      // The plugin structure returned by cb_register_plugin
    bool disabled;          // True if the plugin misbehaved
} LoadedPlugin;

bool plugin_manager_init(const char* plugins_dir);
void plugin_manager_cleanup(void);

// Returns the first active plugin that claims the raw input, or NULL if none
CB_Plugin* plugin_manager_get_claiming_plugin(const char* raw_input);

// Lists plugins to standard output (for 'cmdbridge plugins' command)
void plugin_manager_print_registry(void);

#endif // PLUGIN_MANAGER_H
