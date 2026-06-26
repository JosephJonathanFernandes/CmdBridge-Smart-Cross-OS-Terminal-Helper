#ifndef HISTORY_H
#define HISTORY_H

/**
 * Appends a raw command input to the history file.
 */
void log_history(const char *input);

/**
 * Reads and prints the history file to the terminal.
 */
void show_history(void);

#endif // HISTORY_H
