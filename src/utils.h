#ifndef UTILS_H
#define UTILS_H

/**
 * Trims leading and trailing whitespace from a string.
 */
void trim_whitespace(char *str);

/**
 * Converts a string to lowercase.
 */
void to_lowercase(char *str);

/**
 * Copies text to the system clipboard securely using a temporary file.
 */
void copy_to_clipboard(const char *text);

#endif // UTILS_H
