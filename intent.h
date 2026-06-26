#ifndef INTENT_H
#define INTENT_H

#define MAX_STR_LEN 256

typedef struct {
    char action[MAX_STR_LEN];
    char object[MAX_STR_LEN];
    char target[MAX_STR_LEN];
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
