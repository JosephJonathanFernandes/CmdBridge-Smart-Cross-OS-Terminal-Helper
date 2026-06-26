#ifndef EXPLAIN_H
#define EXPLAIN_H

#define MAX_EXPLANATIONS 100
#define MAX_TOKEN_LEN 50
#define MAX_DESC_LEN 200

typedef struct {
    char token[MAX_TOKEN_LEN];
    char description[MAX_DESC_LEN];
} ExplainEntry;

/**
 * Loads the explanation dictionary from config/explain.conf.
 * Returns the number of loaded explanations.
 */
int load_explanations(ExplainEntry *entries);

/**
 * Parses and explains a terminal command token by token.
 */
void explain_command(const char *cmd, ExplainEntry *entries, int num_entries);

#endif // EXPLAIN_H
