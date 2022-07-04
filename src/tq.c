
//------------------------------------------------------------------------------
// tq library :3
//------------------------------------------------------------------------------

#include <string.h>

#include "tq_audio.h"
#include "tq_core.h"
#include "tq_graphics.h"

//------------------------------------------------------------------------------

void tq_initialize(void)
{
    tq_core_initialize();
    tq_graphics_initialize();
    tq_audio_initialize();
}

void tq_terminate(void)
{
    tq_audio_terminate();
    tq_graphics_terminate();
    tq_core_terminate();
}

bool tq_process(void)
{
    tq_graphics_finish();
    tq_core_present();
    tq_core_keep_up();

    return tq_core_process_events();
}

//------------------------------------------------------------------------------
