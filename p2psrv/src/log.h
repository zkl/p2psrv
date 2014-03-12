#ifndef LOG_H
#define LOG_H

void log_print(const char * format, ...);
void log_alert(const char * format, ...);
void log_error(const char * format, ...);
void log_debug(const char * format, ...);

#define LOG_PRINT log_print
#define LOG_ALERT log_alert
#define LOG_ERROR log_error
#define LOG_DEBUG log_debug

#endif
