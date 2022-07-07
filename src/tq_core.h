
//------------------------------------------------------------------------------

#ifndef TQ_CORE_H_INC
#define TQ_CORE_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"

//------------------------------------------------------------------------------

typedef struct tq_clock
{
    void        (*initialize)(void);
    void        (*terminate)(void);
    float       (*get_time_mediump)(void);
    double      (*get_time_highp)(void);
} tq_clock_t;

typedef struct tq_display
{
    void        (*initialize)(uint32_t, uint32_t, char const *);
    void        (*terminate)(void);
    void        (*present)(void);
    bool        (*process_events)(void);
    void        (*set_size)(uint32_t, uint32_t);
    void        (*set_title)(char const *);
    void        (*set_key_autorepeat_enabled)(bool enabled);
    void        (*show_message_box)(char const *title, char const *message);
} tq_display_t;

#if defined(TQ_USE_SDL)
    void tq_construct_sdl_clock(tq_clock_t *clock);
    void tq_construct_sdl_display(tq_display_t *display);
#endif

//------------------------------------------------------------------------------

void tq_core_initialize(void);
void tq_core_terminate(void);
bool tq_core_process(void);

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

typedef void *tq_thread_t;
typedef void *tq_mutex_t;

typedef struct tq_threads_impl
{
    void            (*initialize)(void);
    void            (*terminate)(void);

    void            (*sleep)(double seconds);

    tq_thread_t     (*create_thread)(char const *name, int (*func)(void *), void *data);
    void            (*detach_thread)(tq_thread_t thread);
    int             (*wait_thread)(tq_thread_t thread);

    tq_mutex_t      (*create_mutex)(void);
    void            (*destroy_mutex)(tq_mutex_t mutex);
    void            (*lock_mutex)(tq_mutex_t mutex);
    void            (*unlock_mutex)(tq_mutex_t mutex);
} tq_threads_impl_t;

#if defined(TQ_PLATFORM_WINDOWS)
    void tq_construct_win32_threads(tq_threads_impl_t *impl);
#elif defined(TQ_PLATFORM_LINUX)
    void tq_construct_posix_threads(tq_threads_impl_t *impl);
#endif

void            tq_core_sleep(double seconds);

tq_thread_t     tq_core_create_thread(char const *name, int (*func)(void *), void *data);
void            tq_core_detach_thread(tq_thread_t thread);
int             tq_core_wait_thread(tq_thread_t thread);

tq_mutex_t      tq_core_create_mutex(void);
void            tq_core_destroy_mutex(tq_mutex_t mutex);
void            tq_core_lock_mutex(tq_mutex_t mutex);
void            tq_core_unlock_mutex(tq_mutex_t mutex);

//------------------------------------------------------------------------------

#endif // TQ_CORE_H_INC
