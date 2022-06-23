
//------------------------------------------------------------------------------

#include <string.h>

#include "tq_audio.h"
#include "tq_core.h"
#include "tq_graphics.h"
#include "tq_meta.h"

//------------------------------------------------------------------------------
// Declarations

typedef struct tq_meta_priv
{
    struct clock        clock;
    struct display      display;
    tq_renderer_t       renderer;
    struct mixer        mixer;
} tq_meta_priv_t;

//------------------------------------------------------------------------------
// Definitions

static tq_meta_priv_t meta;

//------------------------------------------------------------------------------
// Implementation

void meta_initialize(void)
{
    //----------------------------------
    // Zero out the main struct

    memset(&meta, 0, sizeof(tq_meta_priv_t));

    //----------------------------------
    // Construct platform-dependent modules
    // (Only SFML-based dummy module is used by now)

#if defined(TQ_USE_SFML)
    construct_sf_clock(&meta.clock);
    construct_sf_display(&meta.display);

    #if defined(TQ_USE_OPENGL)
        tq_construct_gl_renderer(&meta.renderer);
    #endif

    construct_sf_mixer(&meta.mixer);
#endif

    //----------------------------------
    // Initialize abstract modules

    core_initialize(&meta.clock, &meta.display);
    graphics_initialize(&meta.renderer);
    audio_initialize(&meta.mixer);
}

void meta_terminate(void)
{
    audio_terminate();
    graphics_terminate();
    core_terminate();
}

bool meta_process(void)
{
    tq_graphics_finish();
    core_present();
    core_keep_up();

    return core_process_events();
}

//------------------------------------------------------------------------------
