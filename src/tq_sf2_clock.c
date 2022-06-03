//------------------------------------------------------------------------------

#if defined(TQ_USE_SFML)

//------------------------------------------------------------------------------

#include <SFML/System.h>

#include "tq_clock.h"

//------------------------------------------------------------------------------

static sfClock *clock;

//------------------------------------------------------------------------------

static void sf2_clock_initialize(void)
{
    clock = sfClock_create();
}

static void sf2_clock_terminate(void)
{
    sfClock_destroy(clock);
}

static float sf2_clock_get_time_mediump(void)
{
    return sfTime_asMilliseconds(sfClock_getElapsedTime(clock)) / 1000.0f;
}

static double sf2_clock_get_time_highp(void)
{
    return sfClock_getElapsedTime(clock).microseconds / 1000000.0;
}

//------------------------------------------------------------------------------

void construct_sf2_clock(struct clock *clock)
{
    *clock = (struct clock) {
        .initialize         = sf2_clock_initialize,
        .terminate          = sf2_clock_terminate,
        .get_time_mediump   = sf2_clock_get_time_mediump,
        .get_time_highp     = sf2_clock_get_time_highp,
    };
}

//------------------------------------------------------------------------------

#endif // defined(TQ_USE_SFML)

//------------------------------------------------------------------------------