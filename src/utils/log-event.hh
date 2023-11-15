#ifndef _LOG_EVENT_H
#define _LOG_EVENT_H

#include <stdio.h>
#include <syslog.h>

#define LOG_EVENT(level, format, ...) LogEventFunction(level, __func__, __LINE__, format, ##__VA_ARGS__)

void LogEventInit(const char* pName, int logLevel);
void LogEventDeinit(void);
void LogEventFunction(int logLevel, const char* pFunc, const int line, const char* pFormat, ...);

#endif /*_LOG_EVENT_H */
