
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

void libtq_initialize_core(void)
{
    /**
     * Construct clock, thread and display implementations.
     */

    #if defined(TQ_PLATFORM_DESKTOP)
        #if defined(_WIN32)
            libtq_construct_win32_clock(&core.clock);
            libtq_construct_win32_threads(&core.threads);
        #elif defined(unix)
            libtq_construct_posix_clock(&core.clock);
            libtq_construct_posix_threads(&core.threads);
        #endif

        libtq_construct_sdl_display(&core.display);
    #elif defined(TQ_PLATFORM_ANDROID)
        libtq_construct_posix_clock(&core.clock);
        libtq_construct_posix_threads(&core.threads);
        libtq_construct_android_display(&core.display);
    #endif

    /**
     * Initialization begins here.
     */

    core.clock.initialize();
    core.threads.initialize();

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

    core.display.initialize(0, 0, NULL);

    core.current_time = core.clock.get_time_highp();
    core.delta_time = 0.0;

    core.framerate = 60;
    core.framerate_counter = 0;
    core.framerate_time = core.current_time + 1.0;
}

void libtq_terminate_core(void)
{
    core.display.terminate();
    core.threads.terminate();
    core.clock.terminate();

    memset(&core, 0, sizeof(tq_core_t));
}

bool libtq_process_core(void)
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

void libtq_get_display_size(int *width, int *height)
{
    *width = core.display_width;
    *height = core.display_height;
}

void libtq_set_display_size(int width, int height)
{
    core.display_width = width;
    core.display_height = height;
    core.display_aspect_ratio = (float) width / (float) height;

    if (core.display.set_size) {
        core.display.set_size(width, height);
    }
}

float libtq_get_display_aspect_ratio(void)
{
    return core.display_aspect_ratio;
}

char const *libtq_get_title(void)
{
    return core.title;
}

void libtq_set_title(char const *title)
{
    strncpy(core.title, title, sizeof(core.title) - 1);

    if (core.display.set_title) {
        core.display.set_title(core.title);
    }
}

bool libtq_is_key_autorepeat_enabled(void)
{
    return (core.key_autorepeat == 1);
}

void libtq_set_key_autorepeat_enabled(bool enabled)
{
    core.key_autorepeat = (enabled) ? 1 : -1;

    if (core.display.set_key_autorepeat_enabled) {
        core.display.set_key_autorepeat_enabled(enabled);
    }
}

bool libtq_is_key_pressed(tq_key key)
{
    return get_key_state(key);
}

bool libtq_is_mouse_button_pressed(tq_mouse_button mouse_button)
{
    return (core.mouse_button_state & (1 << mouse_button));
}

void libtq_get_mouse_cursor_position(int *x, int *y)
{
    *x = core.mouse_cursor_x;
    *y = core.mouse_cursor_y;
}

float libtq_get_time_mediump(void)
{
    return core.clock.get_time_mediump();
}

double libtq_get_time_highp(void)
{
    return core.clock.get_time_highp();
}

double libtq_get_delta_time(void)
{
    return core.delta_time;
}

int libtq_get_framerate(void)
{
    return core.framerate;
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

bool libtq_is_mouse_cursor_hidden(void)
{
    return core.mouse_cursor_hidden;
}

void libtq_set_mouse_cursor_hidden(bool hidden)
{
    core.display.set_mouse_cursor_hidden(hidden);
    core.mouse_cursor_hidden = hidden;
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
    graphics_conv_display_coord_to_canvas_coord(x, y, &core.mouse_cursor_x, &core.mouse_cursor_y);

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

void libtq_set_key_press_callback(tq_key_callback callback)
{
    core.key_press_callback = callback;
}

void libtq_set_key_release_callback(tq_key_callback callback)
{
    core.key_release_callback = callback;
}

void libtq_set_mousebutton_press_callback(tq_mouse_button_callback callback)
{
    core.mouse_button_press_callback = callback;
}

void libtq_set_mousebutton_release_callback(tq_mouse_button_callback callback)
{
    core.mouse_button_release_callback = callback;
}

void libtq_set_mousecursor_move_callback(tq_mouse_cursor_callback callback)
{
    core.mouse_cursor_move_callback = callback;
}

void libtq_set_mousewheel_scroll_callback(tq_mouse_wheel_callback callback)
{
    core.mouse_wheel_scroll_callback = callback;
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

//------------------------------------------------------------------------------
