
//------------------------------------------------------------------------------

#include <string.h>

#include "tq_core.h"
#include "tq_graphics.h"

//------------------------------------------------------------------------------

static struct clock     const *clock;
static struct display   const *display;

static uint32_t     display_width;
static uint32_t     display_height;
static char         title[256];
static int          key_autorepeat; /* -1: false, 0: undefined, 1: true */

static double       prev_time;
static double       current_time;
static double       delta_time;

static uint8_t      key_state[32];
static uint8_t      mouse_button_state;
static int32_t      mouse_cursor_x;
static int32_t      mouse_cursor_y;

static tq_key_callback_t            key_press_callback;
static tq_key_callback_t            key_release_callback;
static tq_mouse_button_callback_t   mouse_button_press_callback;
static tq_mouse_button_callback_t   mouse_button_release_callback;
static tq_mouse_cursor_callback_t   mouse_cursor_move_callback;
static tq_mouse_wheel_callback_t    mouse_wheel_scroll_callback;

static unsigned int framerate;
static unsigned int framerate_counter;
static double       framerate_time;

//------------------------------------------------------------------------------

static void set_key_state(tq_key_t key, bool state)
{
    uint8_t index = key / 8;
    uint8_t mask = (1 << (key % 8));

    if (state) {
        key_state[index] |= mask;
    } else {
        key_state[index] &= ~mask;
    }
}

static bool get_key_state(tq_key_t key)
{
    uint8_t index = key / 8;
    uint8_t mask = (1 << (key % 8));

    return (key_state[index] & mask);
}

//------------------------------------------------------------------------------

void core_initialize(struct clock const *clock_, struct display const *display_)
{
    clock = clock_;
    display = display_;

    clock->initialize();

    if (!display_width || !display_height) {
        display_width = 1280;
        display_height = 720;
    }

    if (!title[0]) {
        strcpy(title, "tq library application");
    }

    if (!key_autorepeat) {
        key_autorepeat = 1;
    }

    display->initialize(display_width, display_height, title);

    current_time = clock->get_time_highp();
    delta_time = 0.0;

    key_press_callback = NULL;
    key_release_callback = NULL;
    mouse_button_press_callback = NULL;
    mouse_button_release_callback = NULL;
    mouse_cursor_move_callback = NULL;
    mouse_wheel_scroll_callback = NULL;

    framerate = 60;
    framerate_counter = 0;
    framerate_time = current_time + 1.0;
}

void core_terminate(void)
{
    display->terminate();
    clock->terminate();
}

void core_present(void)
{
    display->present();
}

void core_keep_up(void)
{
    prev_time = current_time;
    current_time = clock->get_time_highp();
    delta_time = current_time - prev_time;

    if (framerate_time < current_time) {
        framerate = framerate_counter;
        framerate_counter = 0;
        framerate_time = framerate_time + 1.0;
    }

    framerate_counter++;
}

bool core_process_events(void)
{
    return display->process_events();
}

uint32_t core_get_display_width(void)
{
    return display_width;
}

uint32_t core_get_display_height(void)
{
    return display_height;
}

void core_get_display_size(uint32_t *width, uint32_t *height)
{
    *width = display_width;
    *height = display_height;
}

void core_set_display_size(uint32_t width, uint32_t height)
{
    display_width = width;
    display_height = height;

    if (display) {
        display->set_size(display_width, display_height);
    }
}

char const *core_get_title(void)
{
    return title;
}

void core_set_title(char const *title_)
{
    strncpy(title, title_, sizeof(title) - 1);

    if (display) {
        display->set_title(title);
    }
}

bool tq_core_is_key_autorepeat_enabled(void)
{
    return (key_autorepeat == 1);
}

void tq_core_set_key_autorepeat_enabled(bool enabled)
{
    key_autorepeat = (enabled) ? 1 : -1;

    if (display) {
        display->set_key_autorepeat_enabled(enabled);
    }
}

bool core_is_key_pressed(tq_key_t key)
{
    return get_key_state(key);
}

bool core_is_mouse_button_pressed(tq_mouse_button_t mouse_button)
{
    return (mouse_button_state & (1 << mouse_button));
}

int32_t core_get_mouse_cursor_x(void)
{
    return mouse_cursor_x;
}

int32_t core_get_mouse_cursor_y(void)
{
    return mouse_cursor_y;
}

float core_get_time_mediump(void)
{
    return clock->get_time_mediump();
}

double core_get_time_highp(void)
{
    return clock->get_time_highp();
}

double core_get_delta_time(void)
{
    return delta_time;
}

unsigned int core_get_framerate(void)
{
    return framerate;
}

void core_on_key_pressed(tq_key_t key)
{
    set_key_state(key, true);

    if (key_press_callback) {
        key_press_callback(key);
    }
}

void core_on_key_released(tq_key_t key)
{
    set_key_state(key, false);

    if (key_release_callback) {
        key_release_callback(key);
    }
}

void core_on_mouse_button_pressed(tq_mouse_button_t mouse_button)
{
    mouse_button_state |= (1 << mouse_button);

    if (mouse_button_press_callback) {
        mouse_button_press_callback(mouse_button, mouse_cursor_x, mouse_cursor_y);
    }
}

void core_on_mouse_button_released(tq_mouse_button_t mouse_button)
{
    mouse_button_state &= ~(1 << mouse_button);

    if (mouse_button_release_callback) {
        mouse_button_release_callback(mouse_button, mouse_cursor_x, mouse_cursor_y);
    }
}

void core_on_mouse_cursor_moved(int32_t x, int32_t y)
{
    mouse_cursor_x = x;
    mouse_cursor_y = y;

    if (mouse_cursor_move_callback) {
        mouse_cursor_move_callback(x, y);
    }
}

void tq_core_on_mouse_wheel_scrolled(float delta, int32_t x, int32_t y)
{
    if (mouse_wheel_scroll_callback) {
        mouse_wheel_scroll_callback(delta, x, y);
    }
}

void tq_core_on_display_resized(uint32_t width, uint32_t height)
{
    display_width = width;
    display_height = height;

    tq_graphics_on_display_resized(width, height);
}

void tq_core_set_key_press_callback(tq_key_callback_t callback)
{
    key_press_callback = callback;
}

void tq_core_set_key_release_callback(tq_key_callback_t callback)
{
    key_release_callback = callback;
}

void tq_core_set_mouse_button_press_callback(tq_mouse_button_callback_t callback)
{
    mouse_button_press_callback = callback;
}

void tq_core_set_mouse_button_release_callback(tq_mouse_button_callback_t callback)
{
    mouse_button_release_callback = callback;
}

void tq_core_set_mouse_cursor_move_callback(tq_mouse_cursor_callback_t callback)
{
    mouse_cursor_move_callback = callback;
}

void tq_core_set_mouse_wheel_scroll_callback(tq_mouse_wheel_callback_t callback)
{
    mouse_wheel_scroll_callback = callback;
}

//------------------------------------------------------------------------------
