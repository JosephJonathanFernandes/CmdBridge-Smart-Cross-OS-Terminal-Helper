#include "config.h"
#include <string.h>
#include <stdlib.h>

#define MAX_CONFIG_ENTRIES 128

typedef struct {
    char key[64];
    char value[256];
} ConfigEntry;

static ConfigEntry g_config[MAX_CONFIG_ENTRIES];
static int g_config_count = 0;

void config_init(void) {
    g_config_count = 0;
}

const char* config_get(const char* key) {
    if (!key) return NULL;
    for (int i = 0; i < g_config_count; i++) {
        if (strcmp(g_config[i].key, key) == 0) {
            return g_config[i].value;
        }
    }
    return NULL;
}

void config_set(const char* key, const char* value) {
    if (!key || !value) return;
    
    for (int i = 0; i < g_config_count; i++) {
        if (strcmp(g_config[i].key, key) == 0) {
            strncpy(g_config[i].value, value, sizeof(g_config[i].value) - 1);
            g_config[i].value[sizeof(g_config[i].value) - 1] = '\0';
            return;
        }
    }
    
    if (g_config_count < MAX_CONFIG_ENTRIES) {
        strncpy(g_config[g_config_count].key, key, sizeof(g_config[g_config_count].key) - 1);
        g_config[g_config_count].key[sizeof(g_config[g_config_count].key) - 1] = '\0';
        
        strncpy(g_config[g_config_count].value, value, sizeof(g_config[g_config_count].value) - 1);
        g_config[g_config_count].value[sizeof(g_config[g_config_count].value) - 1] = '\0';
        
        g_config_count++;
    }
}

void config_cleanup(void) {
    g_config_count = 0;
}
