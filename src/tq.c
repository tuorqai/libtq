//------------------------------------------------------------------------------
// Copyright (c) 2021-2023 tuorqai
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//------------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "tq_audio.h"
#include "tq_core.h"
#include "tq_graphics.h"
#include "tq_log.h"
#include "tq_mem.h"
#include "tq_text.h"

#if defined(EMSCRIPTEN)
#   include <emscripten.h>
#endif

//------------------------------------------------------------------------------

typedef enum tq_status {
    TQ_STATUS_ZERO,
    TQ_STATUS_READY,
} tq_status;

//------------------------------------------------------------------------------

static tq_status status = TQ_STATUS_ZERO;

//------------------------------------------------------------------------------

void tq_initialize(void)
{
    if (status == TQ_STATUS_READY) {
        libtq_log(LIBTQ_LOG_WARNING, "tq_initialize is called more than once. Ignoring.\n");
        return;
    }

    tq_initialize_core();
    tq_initialize_graphics();
    tq_initialize_audio();

    status = TQ_STATUS_READY;
}

void tq_terminate(void)
{
    if (status == TQ_STATUS_ZERO) {
        return;
    }

    tq_terminate_audio();
    tq_terminate_graphics();
    tq_terminate_core();

    status = TQ_STATUS_ZERO;
}

bool tq_process(void)
{
    tq_process_graphics();
    tq_process_audio();

    return tq_process_core();
}

#if defined(EMSCRIPTEN)
void main_loop(void *callback_pointer)
{
    if (tq_process()) {
        tq_loop_callback callback = callback_pointer;
        callback();
    } else {
        emscripten_cancel_main_loop();
    }
}
#endif

void tq_run(tq_loop_callback callback)
{
#if defined(EMSCRIPTEN)
    emscripten_set_main_loop_arg(main_loop, callback, 0, 1);
#else
    while (tq_process()) {
        callback();
    }

    tq_terminate();
    exit(EXIT_SUCCESS);
#endif
}

//------------------------------------------------------------------------------
