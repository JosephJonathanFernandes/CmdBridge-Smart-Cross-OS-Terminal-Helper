#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static LogLevel current_level = LOG_INFO;

void logger_init(LogLevel level) {
    current_level = level;
}

void log_msg(LogLevel level, const char *format, ...) {
    if (level < current_level) {
        return;
    }

    const char *level_str = "INFO";
    switch (level) {
        case LOG_DEBUG: level_str = "DEBUG"; break;
        case LOG_INFO:  level_str = "INFO";  break;
        case LOG_WARN:  level_str = "WARN";  break;
        case LOG_ERROR: level_str = "ERROR"; break;
    }

    time_t now;
    time(&now);
    struct tm *tm_info = localtime(&now);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    printf("[%s] [%s] ", time_buf, level_str);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}
