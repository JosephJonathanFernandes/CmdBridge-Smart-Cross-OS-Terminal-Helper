#ifndef SAFETY_H
#define SAFETY_H

/**
 * Checks if the generated command is potentially dangerous.
 * Returns 1 if safe, 0 if dangerous.
 */
int is_command_safe(const char *cmd);

#endif // SAFETY_H
