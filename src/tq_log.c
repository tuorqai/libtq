
//------------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>

#include "tq_core.h"
#include "tq_log.h"

//------------------------------------------------------------------------------

#define LOG_BUFFER_SIZE 1024
#define LOG_FORMAT(signature_) (signature_" (%9.3f): %s")

//------------------------------------------------------------------------------

void log_generic(int log_level, char const *fmt, ...)
{
    float t = core_get_time_mediump();

    char buffer[LOG_BUFFER_SIZE];
    va_list vp;

    va_start(vp, fmt);
    vsnprintf(buffer, LOG_BUFFER_SIZE, fmt, vp);
    va_end(vp);

    switch (log_level) {
    case LOG_LEVEL_DEBUG:
        fprintf(stdout, LOG_FORMAT("[~]"), t, buffer);
        break;
    case LOG_LEVEL_INFO:
        fprintf(stdout, LOG_FORMAT("[*]"), t, buffer);
        break;
    case LOG_LEVEL_WARNING:
        fprintf(stdout, LOG_FORMAT("[?]"), t, buffer);
        break;
    case LOG_LEVEL_ERROR:
        fprintf(stderr, LOG_FORMAT("[!]"), t, buffer);
        break;
    }

    fflush(stdout);
}

//------------------------------------------------------------------------------
