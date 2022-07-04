
//------------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>

#include "tq_core.h"
#include "tq_log.h"

//------------------------------------------------------------------------------

#define TQ_LOG_BUFFER_SIZE 1024
#define TQ_LOG_FORMAT(signature_) (signature_" (%9.3f): %s")

//------------------------------------------------------------------------------

void tq_log_generic(int log_level, char const *fmt, ...)
{
    float t = tq_core_get_time_mediump();

    char buffer[TQ_LOG_BUFFER_SIZE];
    va_list vp;

    va_start(vp, fmt);
    vsnprintf(buffer, TQ_LOG_BUFFER_SIZE, fmt, vp);
    va_end(vp);

    switch (log_level) {
    case TQ_LOG_LEVEL_DEBUG:
        fprintf(stdout, TQ_LOG_FORMAT("[~]"), t, buffer);
        break;
    case TQ_LOG_LEVEL_INFO:
        fprintf(stdout, TQ_LOG_FORMAT("[*]"), t, buffer);
        break;
    case TQ_LOG_LEVEL_WARNING:
        fprintf(stdout, TQ_LOG_FORMAT("[?]"), t, buffer);
        break;
    case TQ_LOG_LEVEL_ERROR:
        fprintf(stderr, TQ_LOG_FORMAT("[!]"), t, buffer);
        break;
    }

    fflush(stdout);
}

//------------------------------------------------------------------------------
