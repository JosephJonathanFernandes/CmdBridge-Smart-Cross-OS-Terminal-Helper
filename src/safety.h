#ifndef SAFETY_H
#define SAFETY_H

#include "intent.h"

/**
 * Checks if the parsed intent is potentially dangerous.
 * Returns 1 if safe, 0 if dangerous.
 */
int is_intent_safe(const Intent *intent);

/**
 * Checks if the generated command is potentially dangerous by tokenizing it.
 * Returns 1 if safe, 0 if dangerous.
 */
int is_command_safe(const char *cmd);

#endif // SAFETY_H
