
//------------------------------------------------------------------------------

#if defined(unix)

//------------------------------------------------------------------------------

#include "tq_core.h"

//------------------------------------------------------------------------------

static void initialize(void)
{

}

static void terminate(void)
{

}

static float get_time_mediump(void)
{
    return 0.0f;
}

static double get_time_highp(void)
{
    return 0.0;
}

//------------------------------------------------------------------------------

void construct_posix_clock(struct tq_clock *clock)
{
    clock->initialize           = initialize;
    clock->terminate            = terminate;
    clock->get_time_mediump     = get_time_mediump;
    clock->get_time_highp       = get_time_highp;
}

//------------------------------------------------------------------------------

#endif // defined(unix)

//------------------------------------------------------------------------------
