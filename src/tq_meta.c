
//------------------------------------------------------------------------------

#include "tq_audio.h"
#include "tq_core.h"
#include "tq_graphics.h"
#include "tq_meta.h"

//------------------------------------------------------------------------------

static struct clock     clock;
static struct display   display;
static struct renderer  renderer;
static struct mixer     mixer;

//------------------------------------------------------------------------------

void meta_initialize(void)
{
    //----------------------------------
    // Construct platform-dependent modules
    // (Only SFML-based dummy module is used by now)

#if defined(TQ_USE_SFML)

    construct_sf_clock(&clock);
    construct_sf_display(&display);
    construct_sf_renderer(&renderer);
    construct_sf_mixer(&mixer);

#endif

    //----------------------------------
    // Initialize abstract modules

    core_initialize(&clock, &display);
    graphics_initialize(&renderer);
    audio_initialize(&mixer);
}

void meta_terminate(void)
{
    audio_terminate();
    graphics_terminate();
    core_terminate();
}

bool meta_process(void)
{
    graphics_flush();
    core_present();
    core_keep_up();

    return core_process_events();
}

//------------------------------------------------------------------------------
