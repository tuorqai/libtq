
#ifndef TQ_DISPLAY_H_INC
#define TQ_DISPLAY_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"

//------------------------------------------------------------------------------

typedef struct display
{
    void (*initialize)(uint32_t, uint32_t, char const *);
    void (*terminate)(void);
    void (*present)(void);
    bool (*process_events)(void);
    void (*set_size)(uint32_t, uint32_t);
    void (*set_title)(char const *);
    void (*set_key_autorepeat_enabled)(bool enabled);
    void (*show_message_box)(char const *title, char const *message);
} tq_display_t;

//------------------------------------------------------------------------------

#if defined(TQ_USE_SDL)
    void construct_sdl_display(tq_display_t *display);
#endif

//------------------------------------------------------------------------------

#endif // TQ_DISPLAY_H_INC
