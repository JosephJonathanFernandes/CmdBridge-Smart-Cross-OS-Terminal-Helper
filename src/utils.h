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

/**
 * Validates that input contains only safe characters (alphanumeric, space, _, -, ., /, \).
 * Returns 1 if valid, 0 if invalid.
 */
int validate_input_whitelist(const char *input);

/**
 * Tokenizes a command string respecting quotes and escaped characters.
 * Dynamically allocates strings in argv. Caller must free them using free_tokens.
 */
void tokenize_command(const char *cmd, char **argv, int *argc);

/**
 * Frees tokens allocated by tokenize_command.
 */
void free_tokens(char **argv, int argc);

#endif // UTILS_H
