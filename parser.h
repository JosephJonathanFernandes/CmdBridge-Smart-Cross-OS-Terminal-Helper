#ifndef PARSER_H
#define PARSER_H

#include "intent.h"

/**
 * Parses a natural language string into an Intent struct.
 * Populates intent->action, intent->object, and intent->target.
 */
void parse_input(const char *input, Intent *intent);

#endif // PARSER_H
