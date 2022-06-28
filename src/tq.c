
//------------------------------------------------------------------------------

#include <string.h>

#include "tq_audio.h"
#include "tq_core.h"
#include "tq_graphics.h"

//------------------------------------------------------------------------------
// Declarations

typedef struct tq_priv
{
    struct clock        clock;
    struct display      display;
    tq_renderer_t       renderer;
    struct mixer        mixer;
} tq_priv_t;

//------------------------------------------------------------------------------
// Definitions

static tq_priv_t tq;

//------------------------------------------------------------------------------
// Implementation

void tq_initialize(void)
{
    //----------------------------------
    // Zero out the main struct

    memset(&tq, 0, sizeof(tq_priv_t));

    //----------------------------------
    // Construct platform-dependent modules
    // (Only SFML-based dummy module is used by now)

#if defined(TQ_USE_SFML)
    construct_sf_clock(&tq.clock);
    construct_sf_display(&tq.display);

    #if defined(TQ_USE_OPENGL)
        tq_construct_gl_renderer(&tq.renderer);
    #endif

    construct_sf_mixer(&tq.mixer);
#endif

    //----------------------------------
    // Initialize abstract modules

    core_initialize(&tq.clock, &tq.display);
    graphics_initialize(&tq.renderer);
    audio_initialize(&tq.mixer);
}

void tq_terminate(void)
{
    audio_terminate();
    graphics_terminate();
    core_terminate();
}

bool tq_process(void)
{
    tq_graphics_finish();
    core_present();
    core_keep_up();

    return core_process_events();
}

//------------------------------------------------------------------------------
