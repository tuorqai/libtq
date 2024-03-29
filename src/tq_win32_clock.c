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

#if defined(TQ_WIN32)

//------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "tq_core.h"
#include "tq_log.h"

//------------------------------------------------------------------------------

struct libtq_win32_clock_priv
{
    double highp_clock_frequency;
    double highp_clock_start;
    float mediump_clock_start;
};

static struct libtq_win32_clock_priv priv;

//------------------------------------------------------------------------------

static void initialize(void)
{
    LARGE_INTEGER perf_clock_frequency;
    QueryPerformanceFrequency(&perf_clock_frequency);

    LARGE_INTEGER perf_clock_count;
    QueryPerformanceCounter(&perf_clock_count);

    priv.highp_clock_frequency = (double) perf_clock_frequency.QuadPart;
    priv.highp_clock_start = (double) perf_clock_count.QuadPart / priv.highp_clock_frequency;
    priv.mediump_clock_start = (float) GetTickCount() / 1000.0f;

    libtq_log(0, "win32_clock is initialized.\n");
}

static void terminate(void)
{
    libtq_log(0, "win32_clock is terminated.\n");
}

static float get_time_mediump(void)
{
    float seconds = (float) GetTickCount() / 1000.0f;
    return seconds - priv.mediump_clock_start;
}

static double get_time_highp(void)
{
    LARGE_INTEGER perf_clock_counter;
    QueryPerformanceCounter(&perf_clock_counter);

    double seconds = (double) perf_clock_counter.QuadPart / priv.highp_clock_frequency;
    return seconds - priv.highp_clock_start;
}

//------------------------------------------------------------------------------

void libtq_construct_win32_clock(struct libtq_clock_impl *clock)
{
    *clock = (struct libtq_clock_impl) {
        .initialize             = initialize,
        .terminate              = terminate,
        .get_time_mediump       = get_time_mediump,
        .get_time_highp         = get_time_highp,
    };
}

//------------------------------------------------------------------------------

#endif // defined(TQ_WIN32)

//------------------------------------------------------------------------------
