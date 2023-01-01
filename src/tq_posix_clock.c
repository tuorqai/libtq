//------------------------------------------------------------------------------
// Copyright (c) 2021-2023 tuorqai
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

#if defined(TQ_LINUX) || defined(TQ_ANDROID) || defined(TQ_EMSCRIPTEN)

//------------------------------------------------------------------------------

#define _POSIX_C_SOURCE 199309L
#include <time.h>

#include "tq_core.h"
#include "tq_log.h"

//------------------------------------------------------------------------------

struct posix_clock_priv
{
    unsigned long long start_mediump;
    double start_highp;
};

static struct posix_clock_priv priv;

//------------------------------------------------------------------------------

static void initialize(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    priv.start_mediump = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
    priv.start_highp = (double) ts.tv_sec + (ts.tv_nsec / 1.0e9);

    libtq_log(0, "posix_clock: initialize()\n");
}

static void terminate(void)
{
    libtq_log(0, "posix_clock: terminate()\n");
}

static float get_time_mediump(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    unsigned long long msec = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
    return (msec - priv.start_mediump) / 1000.0f;
}

static double get_time_highp(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (double) ts.tv_sec + (ts.tv_nsec / 1.0e9) - priv.start_highp;
}

//------------------------------------------------------------------------------

void libtq_construct_posix_clock(struct libtq_clock_impl *clock)
{
    *clock = (struct libtq_clock_impl) {
        .initialize             = initialize,
        .terminate              = terminate,
        .get_time_mediump       = get_time_mediump,
        .get_time_highp         = get_time_highp,
    };
}

//------------------------------------------------------------------------------

#endif // defined(TQ_LINUX) || defined(TQ_ANDROID) || defined(TQ_EMSCRIPTEN)

//------------------------------------------------------------------------------
