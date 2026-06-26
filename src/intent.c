#include "intent.h"
#include <stdio.h>
#include <string.h>

void init_intent(Intent *intent) {
    if (!intent) return;
    intent->action[0] = '\0';
    intent->object[0] = '\0';
    intent->target[0] = '\0';
}

void print_intent(const Intent *intent) {
    if (!intent) return;
    printf("\nParsed Intent:\n");
    printf("ACTION: %s\n", intent->action[0] ? intent->action : "(none)");
    printf("OBJECT: %s\n", intent->object[0] ? intent->object : "(none)");
    printf("TARGET: %s\n", intent->target[0] ? intent->target : "(none)");
}
