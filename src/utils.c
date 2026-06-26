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
