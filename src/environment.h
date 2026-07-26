#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

typedef struct {
    char os[32];
    char shell[32];
    float confidence;
} EnvironmentInfo;

/**
 * Detects the runtime OS and shell environment.
 * Layered approach:
 * 1. CLI override (confidence 1.0)
 * 2. Parent process (confidence 0.9)
 * 3. Env heuristics (confidence 0.6 - 0.8)
 * 4. Fallback (confidence 0.2)
 *
 * @param env The struct to fill
 * @param cli_shell_override Optional override. Pass NULL to use auto-detection.
 * @return 1 on success, 0 on error
 */
int detect_environment(EnvironmentInfo* env, const char* cli_shell_override);

#endif // ENVIRONMENT_H
