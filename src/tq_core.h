
//------------------------------------------------------------------------------

#ifndef TQ_CORE_H_INC
#define TQ_CORE_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"

//------------------------------------------------------------------------------

typedef struct tq_clock
{
    void (*initialize)(void);
    void (*terminate)(void);
    float (*get_time_mediump)(void);
    double (*get_time_highp)(void);
} tq_clock_t;

typedef struct tq_display
{
    void (*initialize)(uint32_t, uint32_t, char const *);
    void (*terminate)(void);
    void (*present)(void);
    bool (*process_events)(void);
    void (*set_size)(uint32_t, uint32_t);
    void (*set_title)(char const *);
    void (*set_key_autorepeat_enabled)(bool enabled);
    void (*show_message_box)(char const *title, char const *message);
} tq_display_t;

#if defined(TQ_USE_SDL)
    void tq_construct_sdl_clock(tq_clock_t *clock);
    void tq_construct_sdl_display(tq_display_t *display);
#endif

//------------------------------------------------------------------------------

void tq_core_initialize(void);
void tq_core_terminate(void);

void tq_core_present(void);
void tq_core_keep_up(void);
bool tq_core_process_events(void);

uint32_t tq_core_get_display_width(void);
uint32_t tq_core_get_display_height(void);
void tq_core_get_display_size(uint32_t *width, uint32_t *height);
void tq_core_set_display_size(uint32_t width, uint32_t height);

char const *tq_core_get_title(void);
void tq_core_set_title(char const *title);

bool tq_core_is_key_autorepeat_enabled(void);
void tq_core_set_key_autorepeat_enabled(bool enabled);

bool tq_core_is_key_pressed(tq_key_t key);
bool tq_core_is_mouse_button_pressed(tq_mouse_button_t mouse_button);

int32_t tq_core_get_mouse_cursor_x(void);
int32_t tq_core_get_mouse_cursor_y(void);

float tq_core_get_time_mediump(void);
double tq_core_get_time_highp(void);
double tq_core_get_delta_time(void);

unsigned int tq_core_get_framerate(void);

void tq_core_on_key_pressed(tq_key_t key);
void tq_core_on_key_released(tq_key_t key);

void tq_core_on_mouse_button_pressed(tq_mouse_button_t mouse_button);
void tq_core_on_mouse_button_released(tq_mouse_button_t mouse_button);
void tq_core_on_mouse_cursor_moved(int32_t x, int32_t y);
void tq_core_on_mouse_wheel_scrolled(float delta, int32_t x, int32_t y);

void tq_core_on_display_resized(uint32_t width, uint32_t height);

void tq_core_set_key_press_callback(tq_key_callback_t callback);
void tq_core_set_key_release_callback(tq_key_callback_t callback);
void tq_core_set_mouse_button_press_callback(tq_mouse_button_callback_t callback);
void tq_core_set_mouse_button_release_callback(tq_mouse_button_callback_t callback);
void tq_core_set_mouse_cursor_move_callback(tq_mouse_cursor_callback_t callback);
void tq_core_set_mouse_wheel_scroll_callback(tq_mouse_wheel_callback_t callback);

void tq_core_show_message_box(char const *title, char const *message);

//------------------------------------------------------------------------------

#endif // TQ_CORE_H_INC
