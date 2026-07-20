#ifndef OS_MAPPER_H
#define OS_MAPPER_H

#include "intent.h"
#include "commands.h"

/**
 * Maps the parsed intent to the specific OS command.
 * Replaces placeholders like {ARG0} with the actual target from Intent.
 * Returns 1 on success, 0 if no matching template was found.
 */
int map_to_os_command(const Intent *intent, const CommandTemplate *templates, int num_templates, char *out_cmd, ExecutionMode *out_mode);

#endif // OS_MAPPER_H
