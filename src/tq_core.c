
//------------------------------------------------------------------------------

#include <string.h>

#include "tq_core.h"
#include "tq_graphics.h"
#include "tq_log.h"

//------------------------------------------------------------------------------
// Declarations

typedef struct tq_core
{
    tq_clock_t          clock;
    tq_threads_impl_t   threads;
    tq_display_t        display;

    uint32_t            display_width;
    uint32_t            display_height;
    char                title[256];
    int                 key_autorepeat; /* -1: false, 0: undefined, 1: true */

    double              prev_time;
    double              current_time;
    double              delta_time;

    uint8_t             key_state[32];
    uint8_t             mouse_button_state;
    int32_t             mouse_cursor_x;
    int32_t             mouse_cursor_y;

    tq_key_callback_t            key_press_callback;
    tq_key_callback_t            key_release_callback;
    tq_mouse_button_callback_t   mouse_button_press_callback;
    tq_mouse_button_callback_t   mouse_button_release_callback;
    tq_mouse_cursor_callback_t   mouse_cursor_move_callback;
    tq_mouse_wheel_callback_t    mouse_wheel_scroll_callback;

    unsigned int        framerate;
    unsigned int        framerate_counter;
    double              framerate_time;
} tq_core_t;

//------------------------------------------------------------------------------
// Definitions

static tq_core_t core;

//------------------------------------------------------------------------------

static void set_key_state(tq_key_t key, bool state)
{
    uint8_t index = key / 8;
    uint8_t mask = (1 << (key % 8));

    if (state) {
        core.key_state[index] |= mask;
    } else {
        core.key_state[index] &= ~mask;
    }
}

static bool get_key_state(tq_key_t key)
{
    uint8_t index = key / 8;
    uint8_t mask = (1 << (key % 8));

    return (core.key_state[index] & mask);
}

//------------------------------------------------------------------------------

void tq_core_initialize(void)
{
    /**
     * Construct clock implementation.
     */

#if defined(TQ_USE_SDL)
    tq_construct_sdl_clock(&core.clock);
#else
    #error Invalid configuration. Check your build settings.
#endif

    /**
     * Construct threads implementation.
     */

#if defined(TQ_PLATFORM_WINDOWS)
    tq_construct_win32_threads(&core.threads);
#elif defined(TQ_PLATFORM_UNIX)
    tq_construct_posix_threads(&core.threads);
#else
    #error Invalid configuration. Check your build settings.
#endif

    /**
     * Construct display implementation.
     */

#if defined(TQ_USE_SDL)
    tq_construct_sdl_display(&core.display);
#else
    #error Invalid configuration. Check your build settings.
#endif

    /**
     * Initialization begins here.
     */

    core.clock.initialize();
    core.threads.initialize();

    if (!core.display_width || !core.display_height) {
        core.display_width = 1280;
        core.display_height = 720;
    }

    if (!core.title[0]) {
        strcpy(core.title, "tq library application");
    }

    if (!core.key_autorepeat) {
        core.key_autorepeat = 1;
    }

    core.display.initialize(0, 0, NULL);

    core.current_time = core.clock.get_time_highp();
    core.delta_time = 0.0;

    core.framerate = 60;
    core.framerate_counter = 0;
    core.framerate_time = core.current_time + 1.0;
}

void tq_core_terminate(void)
{
    core.display.terminate();
    core.threads.terminate();
    core.clock.terminate();

    memset(&core, 0, sizeof(tq_core_t));
}

bool tq_core_process(void)
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

uint32_t tq_core_get_display_width(void)
{
    return core.display_width;
}

uint32_t tq_core_get_display_height(void)
{
    return core.display_height;
}

void tq_core_get_display_size(uint32_t *width, uint32_t *height)
{
    *width = core.display_width;
    *height = core.display_height;
}

void tq_core_set_display_size(uint32_t width, uint32_t height)
{
    core.display_width = width;
    core.display_height = height;

    if (core.display.set_size) {
        core.display.set_size(width, height);
    }
}

char const *tq_core_get_title(void)
{
    return core.title;
}

void tq_core_set_title(char const *title)
{
    strncpy(core.title, title, sizeof(core.title) - 1);

    if (core.display.set_title) {
        core.display.set_title(core.title);
    }
}

bool tq_core_is_key_autorepeat_enabled(void)
{
    return (core.key_autorepeat == 1);
}

void tq_core_set_key_autorepeat_enabled(bool enabled)
{
    core.key_autorepeat = (enabled) ? 1 : -1;

    if (core.display.set_key_autorepeat_enabled) {
        core.display.set_key_autorepeat_enabled(enabled);
    }
}

bool tq_core_is_key_pressed(tq_key_t key)
{
    return get_key_state(key);
}

bool tq_core_is_mouse_button_pressed(tq_mouse_button_t mouse_button)
{
    return (core.mouse_button_state & (1 << mouse_button));
}

int32_t tq_core_get_mouse_cursor_x(void)
{
    return core.mouse_cursor_x;
}

int32_t tq_core_get_mouse_cursor_y(void)
{
    return core.mouse_cursor_y;
}

float tq_core_get_time_mediump(void)
{
    return core.clock.get_time_mediump();
}

double tq_core_get_time_highp(void)
{
    return core.clock.get_time_highp();
}

double tq_core_get_delta_time(void)
{
    return core.delta_time;
}

unsigned int tq_core_get_framerate(void)
{
    return core.framerate;
}

void tq_core_on_key_pressed(tq_key_t key)
{
    set_key_state(key, true);

    if (core.key_press_callback) {
        core.key_press_callback(key);
    }
}

void tq_core_on_key_released(tq_key_t key)
{
    set_key_state(key, false);

    if (core.key_release_callback) {
        core.key_release_callback(key);
    }
}

void tq_core_on_mouse_button_pressed(tq_mouse_button_t mouse_button)
{
    core.mouse_button_state |= (1 << mouse_button);

    if (core.mouse_button_press_callback) {
        core.mouse_button_press_callback(mouse_button,
            core.mouse_cursor_x, core.mouse_cursor_y);
    }
}

void tq_core_on_mouse_button_released(tq_mouse_button_t mouse_button)
{
    core.mouse_button_state &= ~(1 << mouse_button);

    if (core.mouse_button_release_callback) {
        core.mouse_button_release_callback(mouse_button,
            core.mouse_cursor_x, core.mouse_cursor_y);
    }
}

void tq_core_on_mouse_cursor_moved(int32_t x, int32_t y)
{
    core.mouse_cursor_x = x;
    core.mouse_cursor_y = y;

    if (core.mouse_cursor_move_callback) {
        core.mouse_cursor_move_callback(x, y);
    }
}

void tq_core_on_mouse_wheel_scrolled(float delta, int32_t x, int32_t y)
{
    if (core.mouse_wheel_scroll_callback) {
        core.mouse_wheel_scroll_callback(delta, core.mouse_cursor_x, core.mouse_cursor_y);
    }
}

void tq_core_on_display_resized(uint32_t width, uint32_t height)
{
    core.display_width = width;
    core.display_height = height;

    tq_graphics_on_display_resized(width, height);
}

void tq_core_set_key_press_callback(tq_key_callback_t callback)
{
    core.key_press_callback = callback;
}

void tq_core_set_key_release_callback(tq_key_callback_t callback)
{
    core.key_release_callback = callback;
}

void tq_core_set_mouse_button_press_callback(tq_mouse_button_callback_t callback)
{
    core.mouse_button_press_callback = callback;
}

void tq_core_set_mouse_button_release_callback(tq_mouse_button_callback_t callback)
{
    core.mouse_button_release_callback = callback;
}

void tq_core_set_mouse_cursor_move_callback(tq_mouse_cursor_callback_t callback)
{
    core.mouse_cursor_move_callback = callback;
}

void tq_core_set_mouse_wheel_scroll_callback(tq_mouse_wheel_callback_t callback)
{
    core.mouse_wheel_scroll_callback = callback;
}

void tq_core_show_message_box(char const *title, char const *message)
{
    core.display.show_message_box(title, message);
}

//------------------------------------------------------------------------------
// Threads & mutexes

void tq_core_sleep(double seconds)
{
    core.threads.sleep(seconds);
}

tq_thread_t tq_core_create_thread(char const *name, int (*func)(void *), void *data)
{
    return core.threads.create_thread(name, func, data);
}

void tq_core_detach_thread(tq_thread_t thread)
{
    core.threads.detach_thread(thread);
}

int tq_core_wait_thread(tq_thread_t thread)
{
    return core.threads.wait_thread(thread);
}

tq_mutex_t tq_core_create_mutex(void)
{
    return core.threads.create_mutex();
}

void tq_core_destroy_mutex(tq_mutex_t mutex)
{
    core.threads.destroy_mutex(mutex);
}

void tq_core_lock_mutex(tq_mutex_t mutex)
{
    core.threads.lock_mutex(mutex);
}

void tq_core_unlock_mutex(tq_mutex_t mutex)
{
    core.threads.unlock_mutex(mutex);
}

//------------------------------------------------------------------------------
