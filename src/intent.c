#include "intent.h"
#include <stdio.h>
#include <string.h>

void init_intent(Intent *intent) {
    if (!intent) return;
    intent->action[0] = '\0';
    intent->object[0] = '\0';
    intent->argc = 0;
    for (int i = 0; i < MAX_ARGS; i++) {
        intent->args[i][0] = '\0';
    }
}

void print_intent(const Intent *intent) {
    if (!intent) return;
    printf("\nParsed Intent:\n");
    printf("ACTION: %s\n", intent->action[0] ? intent->action : "(none)");
    printf("OBJECT: %s\n", intent->object[0] ? intent->object : "(none)");
    for (int i = 0; i < intent->argc; i++) {
        printf("ARG[%d]: %s\n", i, intent->args[i]);
    }
}
