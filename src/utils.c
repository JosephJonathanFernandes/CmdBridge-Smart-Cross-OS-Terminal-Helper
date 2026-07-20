#include "utils.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void trim_whitespace(char *str) {
    if (!str) return;

    // Trim trailing whitespace
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }

    // Trim leading whitespace
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

void to_lowercase(char *str) {
    if (!str) return;
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

void copy_to_clipboard(const char *text) {
    if (!text) return;
    
    // Write text to a temporary file
    const char *tmp_file = ".clipboard.tmp";
    FILE *f = fopen(tmp_file, "w");
    if (!f) return;
    
    // Don't add a trailing newline so it pastes cleanly
    fprintf(f, "%s", text);
    fclose(f);

    // Call OS specific clipboard utility via system()
#ifdef _WIN32
    system("clip < .clipboard.tmp");
#elif __APPLE__
    system("pbcopy < .clipboard.tmp");
#elif __linux__
    system("xclip -selection clipboard < .clipboard.tmp");
#else
    // Fallback if none exist
#endif

    // Remove the temporary file
    remove(tmp_file);
}

int validate_input_whitelist(const char *input) {
    if (!input) return 0;
    for (int i = 0; input[i]; i++) {
        char c = input[i];
        if (!(isalnum((unsigned char)c) || c == '_' || c == '.' || c == '-' || c == '/' || c == '\\' || c == ' ')) {
            return 0; // Invalid character found
        }
    }
    return 1;
}

void tokenize_command(const char *cmd, char **argv, int *argc) {
    *argc = 0;
    if (!cmd) return;
    
    char *buffer = strdup(cmd);
    if (!buffer) return;
    
    char *p = buffer;
    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        
        char *token_start = p;
        char quote = 0;
        char *write_ptr = token_start;
        
        while (*p) {
            if (*p == '\\' && *(p+1)) {
                p++;
                *write_ptr++ = *p++;
            } else if (quote == 0 && (*p == '"' || *p == '\'')) {
                quote = *p++;
            } else if (quote != 0 && *p == quote) {
                quote = 0;
                p++;
            } else if (quote == 0 && isspace((unsigned char)*p)) {
                break;
            } else {
                *write_ptr++ = *p++;
            }
        }
        
        int ended_at_space = isspace((unsigned char)*p);
        if (*p) p++;
        *write_ptr = '\0';
        
        argv[*argc] = strdup(token_start);
        (*argc)++;
        
        if (!ended_at_space) {
            break;
        }
    }
    
    argv[*argc] = NULL;
    free(buffer);
}

void free_tokens(char **argv, int argc) {
    for (int i = 0; i < argc; i++) {
        free(argv[i]);
        argv[i] = NULL;
    }
}
