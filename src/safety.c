#include "safety.h"
#include <string.h>
#include <ctype.h>

int is_command_safe(const char *cmd) {
    if (!cmd) return 1; // Empty command is safe

    const char *dangerous_keywords[] = {
        "rm -r",
        "rm -rf",
        "Remove-Item",
        "del",
        "kill",
        "Stop-Process",
        "shutdown",
        "format",
        "mkfs"
    };

    int num_keywords = sizeof(dangerous_keywords) / sizeof(dangerous_keywords[0]);
    
    // We do a simple substring check. 
    // In a more robust version, we would tokenize the command to ensure we match whole words.
    for (int i = 0; i < num_keywords; i++) {
        if (strstr(cmd, dangerous_keywords[i]) != NULL) {
            return 0; // Dangerous
        }
    }

    return 1; // Safe
}
