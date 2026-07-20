#ifndef INTENT_H
#define INTENT_H

#define MAX_STR_LEN 256
#define MAX_ARGS 10

typedef struct {
    char action[MAX_STR_LEN];
    char object[MAX_STR_LEN];
    char args[MAX_ARGS][MAX_STR_LEN];
    int argc;
} Intent;

/**
 * Initializes an intent struct with empty strings.
 */
void init_intent(Intent *intent);

/**
 * Prints the parsed intent for debugging purposes.
 */
void print_intent(const Intent *intent);

#endif // INTENT_H
