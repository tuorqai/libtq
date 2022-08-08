
//------------------------------------------------------------------------------

#if defined(__unix__)

//------------------------------------------------------------------------------

#if defined(__linux__)
    #define _POSIX_C_SOURCE 199309L
#endif

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

    log_info("posix_clock: initialize()\n");
}

static void terminate(void)
{
    log_info("posix_clock: terminate()\n");
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

#endif // defined(__unix__)

//------------------------------------------------------------------------------
