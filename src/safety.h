#ifndef SAFETY_H
#define SAFETY_H

#include "intent.h"

typedef enum {
    RISK_SAFE,
    RISK_LOW,
    RISK_MEDIUM,
    RISK_HIGH,
    RISK_CRITICAL
} RiskLevel;

/**
 * Validates the intent before execution (exists, self-copy, root deletion, etc).
 * Returns 1 if valid and safe to proceed, 0 if invalid.
 * Fills `error_msg` with the reason if invalid.
 */
int validate_intent(const Intent *intent, char *error_msg, int max_msg_len);

/**
 * Returns a detailed string explaining the consequence of the command
 */
void get_intent_consequence(const Intent *intent, char *consequence, int max_len);

/**
 * Gets the risk level for the parsed intent.
 */
RiskLevel get_intent_risk(const Intent *intent);

/**
 * Converts RiskLevel enum to string.
 */
const char* get_risk_string(RiskLevel risk);

/**
 * Checks if the generated command is potentially dangerous by tokenizing it.
 * Returns 1 if safe, 0 if dangerous.
 */
int is_command_safe(const char *cmd);

#endif // SAFETY_H
