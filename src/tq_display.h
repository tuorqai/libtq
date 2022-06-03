
#ifndef TQ_DISPLAY_H_INC
#define TQ_DISPLAY_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"

//------------------------------------------------------------------------------

struct display
{
    void (*initialize)(uint32_t, uint32_t, char const *);
    void (*terminate)(void);
    void (*present)(void);
    bool (*process_events)(void);
    void (*set_size)(uint32_t, uint32_t);
    void (*set_title)(char const *);
};

//------------------------------------------------------------------------------

#if defined(TQ_USE_SFML)
    void construct_sf2_display(struct display *display);
#endif

//------------------------------------------------------------------------------

#endif // TQ_DISPLAY_H_INC
