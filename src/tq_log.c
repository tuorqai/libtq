//------------------------------------------------------------------------------
// Copyright (c) 2021-2022 tuorqai
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//------------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>

#include "tq_core.h"
#include "tq_log.h"
#include "tq_mem.h"

//------------------------------------------------------------------------------

#define FORMAT_MESSAGE(Signature) (Signature " (%9.3f): %s")

//------------------------------------------------------------------------------
// TODO: make this thread-safe

struct libtq_logger
{
    char        *buffer; // FIXME: this never gets deleted
    size_t      buffer_size;
};

static struct libtq_logger logger;

//------------------------------------------------------------------------------

void libtq_log(int level, char const *fmt, ...)
{
    va_list vp;

    va_start(vp, fmt);

    int bytes_required = vsnprintf(logger.buffer, logger.buffer_size, fmt, vp);

    // Not enough space in the buffer, need to reallocate.
    if (bytes_required >= logger.buffer_size) {
        if (logger.buffer_size == 0) {
            logger.buffer_size = 1024;
        }

        while (logger.buffer_size < (bytes_required + 1)) {
            logger.buffer_size *= 2;
        }

        logger.buffer = libtq_realloc(logger.buffer, logger.buffer_size);
        vsnprintf(logger.buffer, logger.buffer_size, fmt, vp);
    }

    va_end(vp);

    float t = libtq_get_time_mediump();

    switch (level) {
    case LIBTQ_LOG_DEBUG:
        fprintf(stdout, FORMAT_MESSAGE("[DBG]"), t, logger.buffer);
        break;
    case LIBTQ_LOG_INFO:
        fprintf(stdout, FORMAT_MESSAGE(""), t, logger.buffer);
        break;
    case LIBTQ_LOG_WARNING:
        fprintf(stdout, FORMAT_MESSAGE("[WRN]"), t, logger.buffer);
        break;
    case LIBTQ_LOG_ERROR:
        fprintf(stderr, FORMAT_MESSAGE("[ERR]"), t, logger.buffer);
        break;
    }

    fflush(stdout);
}

//------------------------------------------------------------------------------
