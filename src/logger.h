#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

/**
 * Initializes the logger with the specified level.
 */
void logger_init(LogLevel level);

/**
 * Logs a message at the specified level.
 */
void log_msg(LogLevel level, const char *format, ...);

#endif // LOGGER_H
