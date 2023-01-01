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
#include "tq_mem.h"
#include "tq_text.h"

#if defined(EMSCRIPTEN)
#   include <emscripten.h>
#endif

//------------------------------------------------------------------------------

void tq_initialize(void)
{
    libtq_initialize_core();
    tq_initialize_graphics();
    tq_initialize_audio();
}

void tq_terminate(void)
{
    tq_terminate_audio();
    tq_terminate_graphics();
    libtq_terminate_core();
}

bool tq_process(void)
{
    tq_process_graphics();
    tq_process_audio();

    return libtq_process_core();
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
    // Temporary.

#if defined(EMSCRIPTEN)
    emscripten_set_main_loop_arg(main_loop, callback, 0, 1);
#else
    while (tq_process()) {
        callback();
    }

    tq_terminate();
    exit(0);
#endif
}

//------------------------------------------------------------------------------
// Core

//----------------------------------------------------------
// Display

tq_vec2i tq_get_display_size(void)
{
    tq_vec2i size;
    libtq_get_display_size(&size.x, &size.y);

    return size;
}

void tq_set_display_size(tq_vec2i size)
{
    libtq_set_display_size(size.x, size.y);
}

char const *tq_get_title(void)
{
    return libtq_get_title();
}

void tq_set_title(char const *title)
{
    libtq_set_title(title);
}

//----------------------------------------------------------
// Keyboard

bool tq_is_key_autorepeat_enabled(void)
{
    return libtq_is_key_autorepeat_enabled();
}

void tq_set_key_autorepeat_enabled(bool enabled)
{
    libtq_set_key_autorepeat_enabled(enabled);
}

bool tq_is_key_pressed(tq_key key)
{
    return libtq_is_key_pressed(key);
}

bool tq_is_mouse_button_pressed(tq_mouse_button mouse_button)
{
    return libtq_is_mouse_button_pressed(mouse_button);
}

void tq_on_key_pressed(tq_key_callback callback)
{
    libtq_set_key_press_callback(callback);
}

void tq_on_key_released(tq_key_callback callback)
{
    libtq_set_key_release_callback(callback);
}

//----------------------------------------------------------
// Mouse

bool tq_is_mouse_cursor_hidden(void)
{
    return libtq_is_mouse_cursor_hidden();
}

void tq_set_mouse_cursor_hidden(bool hidden)
{
    libtq_set_mouse_cursor_hidden(hidden);
}

tq_vec2i tq_get_mouse_cursor_position(void)
{
    tq_vec2i position;
    libtq_get_mouse_cursor_position(&position.x, &position.y);

    return position;
}

void tq_on_mouse_button_pressed(tq_mouse_button_callback callback)
{
    libtq_set_mousebutton_press_callback(callback);
}

void tq_on_mouse_button_released(tq_mouse_button_callback callback)
{
    libtq_set_mousebutton_release_callback(callback);
}

void tq_on_mouse_cursor_moved(tq_mouse_cursor_callback callback)
{
    libtq_set_mousecursor_move_callback(callback);
}

void tq_on_mouse_wheel_scrolled(tq_mouse_wheel_callback callback)
{
    libtq_set_mousewheel_scroll_callback(callback);
}

//----------------------------------------------------------
// Time

float tq_get_time_mediump(void)
{
    return libtq_get_time_mediump();
}

double tq_get_time_highp(void)
{
    return libtq_get_time_highp();
}

double tq_get_delta_time(void)
{
    return libtq_get_delta_time();
}

//----------------------------------------------------------
// Stats

int tq_get_framerate(void)
{
    return libtq_get_framerate();
}

//------------------------------------------------------------------------------
