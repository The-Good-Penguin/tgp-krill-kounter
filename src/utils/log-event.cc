#include <glib.h>

#include <cstdarg>
#include <iostream>

#include "log-event.hh"

static int currentLevel = LOG_INFO;

void LogEventInit(const char* pName, int logLevel)
{
    openlog(pName, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
    currentLevel = logLevel;
}

void LogEventDeinit(void)
{
    closelog();
}

void LogEventFunction(int logLevel, const char* pFunc, const int line, const char* pFormat, ...)
{
    if (G_UNLIKELY(logLevel <= currentLevel))
    {
        va_list args;
        va_start(args, pFormat);

        char message[4096];

        vsnprintf(message, sizeof(message), pFormat, args);

        va_end(args);

        syslog(logLevel, "[%s:%d]: %s", pFunc, line, message);

    }
}