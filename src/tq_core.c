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

#include <string.h>

#include "tq_core.h"
#include "tq_graphics.h"
#include "tq_log.h"
#include "tq_math.h"

//------------------------------------------------------------------------------
// Declarations

typedef struct tq_core
{
    struct libtq_clock_impl     clock;
    struct libtq_threads_impl   threads;
    struct libtq_display_impl   display;

    int                 display_width;
    int                 display_height;
    float               display_aspect_ratio;

    char                title[256];
    int                 key_autorepeat; /* -1: false, 0: undefined, 1: true */

    double              prev_time;
    double              current_time;
    double              delta_time;

    uint8_t             key_state[32];
    bool                mouse_cursor_hidden;
    uint8_t             mouse_button_state;
    int32_t             mouse_cursor_x;
    int32_t             mouse_cursor_y;

    tq_key_callback              key_press_callback;
    tq_key_callback              key_release_callback;
    tq_mouse_button_callback     mouse_button_press_callback;
    tq_mouse_button_callback     mouse_button_release_callback;
    tq_mouse_cursor_callback     mouse_cursor_move_callback;
    tq_mouse_wheel_callback      mouse_wheel_scroll_callback;

    unsigned int        framerate;
    unsigned int        framerate_counter;
    double              framerate_time;
} tq_core_t;

//------------------------------------------------------------------------------
// Definitions

static tq_core_t core;

//------------------------------------------------------------------------------

static void set_key_state(tq_key key, bool state)
{
    uint8_t index = key / 8;
    uint8_t mask = (1 << (key % 8));

    if (state) {
        core.key_state[index] |= mask;
    } else {
        core.key_state[index] &= ~mask;
    }
}

static bool get_key_state(tq_key key)
{
    uint8_t index = key / 8;
    uint8_t mask = (1 << (key % 8));

    return (core.key_state[index] & mask);
}

//------------------------------------------------------------------------------

void tq_initialize_core(void)
{
    /**
     * Construct clock, thread and display implementations.
     */

    #if defined(TQ_WIN32)
        libtq_construct_win32_clock(&core.clock);
        libtq_construct_win32_threads(&core.threads);
        libtq_construct_win32_display(&core.display);
    #elif defined(TQ_LINUX) || defined(TQ_EMSCRIPTEN)
        libtq_construct_posix_clock(&core.clock);
        libtq_construct_posix_threads(&core.threads);
        libtq_construct_sdl_display(&core.display);
    #elif defined(TQ_ANDROID)
        libtq_construct_posix_clock(&core.clock);
        libtq_construct_posix_threads(&core.threads);
        libtq_construct_android_display(&core.display);
    #endif

    /**
     * Initialization begins here.
     */

    core.clock.initialize();
    core.threads.initialize();

    libtq_log(0, "tq library version " TQ_VERSION "\n");

    if (!core.display_width || !core.display_height) {
        core.display_width = 1280;
        core.display_height = 720;
        core.display_aspect_ratio = 1280.0f / 720.0f;
    }

    if (!core.title[0]) {
        strcpy(core.title, "tq library application");
    }

    if (!core.key_autorepeat) {
        core.key_autorepeat = 1;
    }

    core.display.initialize();

    core.current_time = core.clock.get_time_highp();
    core.delta_time = 0.0;

    core.framerate = 60;
    core.framerate_counter = 0;
    core.framerate_time = core.current_time + 1.0;
}

void tq_terminate_core(void)
{
    core.display.terminate();
    core.threads.terminate();
    core.clock.terminate();

    memset(&core, 0, sizeof(tq_core_t));
}

bool tq_process_core(void)
{
    core.display.present();

    core.prev_time = core.current_time;
    core.current_time = core.clock.get_time_highp();
    core.delta_time = core.current_time - core.prev_time;

    if (core.framerate_time < core.current_time) {
        core.framerate = core.framerate_counter;
        core.framerate_counter = 0;
        core.framerate_time = core.framerate_time + 1.0;
    }

    core.framerate_counter++;

    return core.display.process_events();
}

//------------------------------------------------------------------------------

/**
 * API entry: tq_get_display_size()
 */
tq_vec2i tq_get_display_size(void)
{
    return (tq_vec2i) {
        .x = core.display_width,
        .y = core.display_height,
    };
}

/**
 * API entry: tq_set_display_size()
 */
void tq_set_display_size(tq_vec2i size)
{
    core.display_width = size.x;
    core.display_height = size.y;
    core.display_aspect_ratio = (float) size.x / (float) size.y;

    if (core.display.set_size) {
        core.display.set_size(size.x, size.y);
    }
}

/**
 * API entry: tq_get_title()
 */
char const *tq_get_title(void)
{
    return core.title;
}

/**
 * API entry: tq_set_title()
 */
void tq_set_title(char const *title)
{
    strncpy(core.title, title, sizeof(core.title) - 1);

    if (core.display.set_title) {
        core.display.set_title(core.title);
    }
}

//------------------------------------------------------------------------------

/**
 * API entry: tq_is_key_autorepeat_enabled()
 */
bool tq_is_key_autorepeat_enabled(void)
{
    return (core.key_autorepeat == 1);
}

/**
 * API entry: tq_set_key_autorepeat_enabled()
 */
void tq_set_key_autorepeat_enabled(bool enabled)
{
    core.key_autorepeat = (enabled) ? 1 : -1;

    if (core.display.set_key_autorepeat_enabled) {
        core.display.set_key_autorepeat_enabled(enabled);
    }
}

/**
 * API entry: tq_is_key_pressed()
 */
bool tq_is_key_pressed(tq_key key)
{
    return get_key_state(key);
}

/**
 * API entry: tq_on_key_pressed()
 */
void tq_on_key_pressed(tq_key_callback callback)
{
    core.key_press_callback = callback;
}

/**
 * API entry: tq_on_key_released()
 */
void tq_on_key_released(tq_key_callback callback)
{
    core.key_release_callback = callback;
}

//------------------------------------------------------------------------------

/**
 * API entry: tq_is_mouse_cursor_hidden()
 */
bool tq_is_mouse_cursor_hidden(void)
{
    return core.mouse_cursor_hidden;
}

/**
 * API entry: tq_set_mouse_cursor_hidden()
 */
void tq_set_mouse_cursor_hidden(bool hidden)
{
    core.display.set_mouse_cursor_hidden(hidden);
    core.mouse_cursor_hidden = hidden;
}

/**
 * API entry: tq_is_mouse_button_pressed()
 */
bool tq_is_mouse_button_pressed(tq_mouse_button mouse_button)
{
    return (core.mouse_button_state & (1 << mouse_button));
}

/**
 * API entry: tq_get_mouse_cursor_position()
 */
tq_vec2i tq_get_mouse_cursor_position(void)
{
    return (tq_vec2i) {
        .x = core.mouse_cursor_x,
        .y = core.mouse_cursor_y,
    };
}

/**
 * API entry: tq_on_mouse_button_pressed()
 */
void tq_on_mouse_button_pressed(tq_mouse_button_callback callback)
{
    core.mouse_button_press_callback = callback;
}

/**
 * API entry: tq_on_mouse_button_released()
 */
void tq_on_mouse_button_released(tq_mouse_button_callback callback)
{
    core.mouse_button_release_callback = callback;
}

/**
 * API entry: tq_on_mouse_cursor_moved()
 */
void tq_on_mouse_cursor_moved(tq_mouse_cursor_callback callback)
{
    core.mouse_cursor_move_callback = callback;
}

/**
 * API entry: tq_on_mouse_wheel_scrolled()
 */
void tq_on_mouse_wheel_scrolled(tq_mouse_wheel_callback callback)
{
    core.mouse_wheel_scroll_callback = callback;
}

//------------------------------------------------------------------------------

/**
 * API entry: tq_get_time_mediump()
 */
float tq_get_time_mediump(void)
{
    return core.clock.get_time_mediump();
}

/**
 * API entry: tq_get_time_highp()
 */
double tq_get_time_highp(void)
{
    return core.clock.get_time_highp();
}

/**
 * API entry: tq_get_delta_time()
 */
double tq_get_delta_time(void)
{
    return core.delta_time;
}

/**
 * API entry: tq_get_framerate()
 */
int tq_get_framerate(void)
{
    return core.framerate;
}

//------------------------------------------------------------------------------

float libtq_get_display_aspect_ratio(void)
{
    return core.display_aspect_ratio;
}

void libtq_on_key_pressed(tq_key key)
{
    set_key_state(key, true);

    if (core.key_press_callback) {
        core.key_press_callback(key);
    }
}

void libtq_on_key_released(tq_key key)
{
    set_key_state(key, false);

    if (core.key_release_callback) {
        core.key_release_callback(key);
    }
}

void libtq_on_mouse_button_pressed(tq_mouse_button mouse_button)
{
    core.mouse_button_state |= (1 << mouse_button);

    if (core.mouse_button_press_callback) {
        tq_vec2i cursor = {core.mouse_cursor_x, core.mouse_cursor_y};
        core.mouse_button_press_callback(cursor, mouse_button);
    }
}

void libtq_on_mouse_button_released(tq_mouse_button mouse_button)
{
    core.mouse_button_state &= ~(1 << mouse_button);

    if (core.mouse_button_release_callback) {
        tq_vec2i cursor = {core.mouse_cursor_x, core.mouse_cursor_y};
        core.mouse_button_release_callback(cursor, mouse_button);
    }
}

void libtq_on_mouse_cursor_moved(int32_t x, int32_t y)
{
    tq_vec2i canvas_coord = tq_conv_display_coord((tq_vec2i) { x, y });

    core.mouse_cursor_x = canvas_coord.x;
    core.mouse_cursor_y = canvas_coord.y;

    if (core.mouse_cursor_move_callback) {
        tq_vec2i cursor = {core.mouse_cursor_x, core.mouse_cursor_y};
        core.mouse_cursor_move_callback(cursor);
    }
}

void libtq_on_mouse_wheel_scrolled(float x_delta, float y_delta)
{
    if (core.mouse_wheel_scroll_callback) {
        tq_vec2i cursor = {core.mouse_cursor_x, core.mouse_cursor_y};
        tq_vec2f wheel = {x_delta, y_delta};
        core.mouse_wheel_scroll_callback(cursor, wheel);
    }
}

void libtq_on_display_resize(int width, int height)
{
    core.display_width = width;
    core.display_height = height;
    core.display_aspect_ratio = (float) width / (float) height;
}

void libtq_on_focus_gain(void)
{
}

void libtq_on_focus_loss(void)
{
}

void libtq_show_msgbox(char const *title, char const *message)
{
    core.display.show_message_box(title, message);
}

//------------------------------------------------------------------------------
// Threads & mutexes

void libtq_sleep(double seconds)
{
    core.threads.sleep(seconds);
}

libtq_thread libtq_create_thread(char const *name, int (*func)(void *), void *data)
{
    return core.threads.create_thread(name, func, data);
}

void libtq_detach_thread(libtq_thread thread)
{
    core.threads.detach_thread(thread);
}

int libtq_wait_thread(libtq_thread thread)
{
    return core.threads.wait_thread(thread);
}

libtq_mutex libtq_create_mutex(void)
{
    return core.threads.create_mutex();
}

void libtq_destroy_mutex(libtq_mutex mutex)
{
    core.threads.destroy_mutex(mutex);
}

void libtq_lock_mutex(libtq_mutex mutex)
{
    core.threads.lock_mutex(mutex);
}

void libtq_unlock_mutex(libtq_mutex mutex)
{
    core.threads.unlock_mutex(mutex);
}

void *libtq_get_gl_proc_addr(char const *name)
{
    return core.display.get_gl_proc_addr(name);
}

bool libtq_check_gl_ext(char const *name)
{
    return core.display.check_gl_ext(name);
}

//------------------------------------------------------------------------------
