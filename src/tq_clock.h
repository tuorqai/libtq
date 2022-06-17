
//------------------------------------------------------------------------------

#ifndef TQ_CLOCK_H_INC
#define TQ_CLOCK_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"

//------------------------------------------------------------------------------

struct clock
{
    void (*initialize)(void);
    void (*terminate)(void);
    float (*get_time_mediump)(void);
    double (*get_time_highp)(void);
};

//------------------------------------------------------------------------------

#if defined(TQ_USE_SFML)
    void construct_sf_clock(struct clock *clock);
#endif

//------------------------------------------------------------------------------

#endif // TQ_CLOCK_H_INC

//------------------------------------------------------------------------------
