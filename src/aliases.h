#ifndef ALIASES_H
#define ALIASES_H

#define MAX_ALIASES 50
#define MAX_ALIAS_NAME 50
#define MAX_ALIAS_VAL 256

typedef struct {
    char name[MAX_ALIAS_NAME];
    char value[MAX_ALIAS_VAL];
} Alias;

/**
 * Loads aliases from data/aliases.txt.
 * Returns the number of loaded aliases.
 */
int load_aliases(Alias *aliases);

/**
 * Saves a new alias to data/aliases.txt and updates the in-memory array.
 */
int save_alias(const char *name, const char *value, Alias *aliases, int *num_aliases);

/**
 * Resolves an input string against loaded aliases.
 * Returns the mapped value if found, otherwise returns the original input.
 */
const char* resolve_alias(const char *input, Alias *aliases, int num_aliases);

#endif // ALIASES_H
