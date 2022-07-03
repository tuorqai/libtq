
//------------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>

#include "tq_error.h"
#include "tq_log.h"

//------------------------------------------------------------------------------

void tq_error(char const *fmt, ...)
{
    va_list ap;
    char buffer[1024];

    va_start(ap, fmt);
    vsnprintf(buffer, 1024, fmt, ap);
    va_end(ap);

    log_error("------------------------------------------------------------\n");
    log_error("    tq library critical error:\n");
    log_error("    %s\n", buffer);
    log_error("------------------------------------------------------------\n");

    abort();
}

//------------------------------------------------------------------------------
