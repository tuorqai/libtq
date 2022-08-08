
//------------------------------------------------------------------------------

#include "tq_audio.h"
#include "tq_core.h"
#include "tq_graphics.h"
#include "tq_meta.h"

//------------------------------------------------------------------------------

void libtq_initialize(void)
{
    libtq_initialize_core();
    tq_graphics_initialize();
    tq_audio_initialize();
}

void libtq_terminate(void)
{
    tq_audio_terminate();
    tq_graphics_terminate();
    libtq_terminate_core();
}

bool libtq_process(void)
{
    tq_graphics_process();
    tq_audio_process();

    return libtq_process_core();
}

//------------------------------------------------------------------------------
