#include "file_ops.h"
#include <string.h>

int execute_native_api(const Intent *intent) {
    if (!intent) return 0;
    
    if (strcmp(intent->action, "create") == 0 && strcmp(intent->object, "folders") == 0) {
        if (intent->argc > 0) {
            return api_create_folder(intent->args[0]);
        }
    }
    
    if (strcmp(intent->action, "delete") == 0 && strcmp(intent->object, "folders") == 0) {
        if (intent->argc > 0) {
            return api_delete_folder(intent->args[0]);
        }
    }
    
    if (strcmp(intent->action, "copy") == 0 && strcmp(intent->object, "files") == 0) {
        if (intent->argc > 1) {
            return api_copy_file(intent->args[0], intent->args[1]);
        }
    }
    
    if (strcmp(intent->action, "move") == 0 && strcmp(intent->object, "files") == 0) {
        if (intent->argc > 1) {
            return api_move_file(intent->args[0], intent->args[1]);
        }
    }

    return 0; // Unsupported API execution
}
