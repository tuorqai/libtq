
//------------------------------------------------------------------------------

#include "tq_audio.h"
#include "tq_core.h"
#include "tq_graphics.h"
#include "tq_meta.h"

//------------------------------------------------------------------------------

void libtq_initialize(void)
{
    libtq_initialize_core();
    libtq_initialize_graphics();
    libtq_initialize_audio();
}

void libtq_terminate(void)
{
    libtq_terminate_audio();
    libtq_terminate_graphics();
    libtq_terminate_core();
}

bool libtq_process(void)
{
    libtq_process_graphics();
    libtq_process_audio();

    return libtq_process_core();
}

//------------------------------------------------------------------------------
