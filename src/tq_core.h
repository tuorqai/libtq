
//------------------------------------------------------------------------------

#ifndef TQ_CORE_H_INC
#define TQ_CORE_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"

//------------------------------------------------------------------------------

struct libtq_clock_impl
{
    void        (*initialize)(void);
    void        (*terminate)(void);
    float       (*get_time_mediump)(void);
    double      (*get_time_highp)(void);
};

#if defined(_WIN32)
    void libtq_construct_win32_clock(struct libtq_clock_impl *clock);
#endif

#if defined(__unix__)
    void libtq_construct_posix_clock(struct libtq_clock_impl *clock);
#endif

//------------------------------------------------------------------------------

typedef void *libtq_thread;
typedef void *libtq_mutex;

struct libtq_threads_impl
{
    void            (*initialize)(void);
    void            (*terminate)(void);

    void            (*sleep)(double seconds);

    libtq_thread    (*create_thread)(char const *name, int (*func)(void *), void *data);
    void            (*detach_thread)(libtq_thread thread);
    int             (*wait_thread)(libtq_thread thread);

    libtq_mutex     (*create_mutex)(void);
    void            (*destroy_mutex)(libtq_mutex mutex);
    void            (*lock_mutex)(libtq_mutex mutex);
    void            (*unlock_mutex)(libtq_mutex mutex);
};

#if defined(_WIN32)
    void libtq_construct_win32_threads(struct libtq_threads_impl *threads);
#endif

#if defined(__unix__)
    void libtq_construct_posix_threads(struct libtq_threads_impl *threads);
#endif

//------------------------------------------------------------------------------

struct libtq_display_impl
{
    void        (*initialize)(uint32_t, uint32_t, char const *);
    void        (*terminate)(void);
    void        (*present)(void);
    bool        (*process_events)(void);
    void        (*set_size)(uint32_t, uint32_t);
    void        (*set_title)(char const *);
    void        (*set_key_autorepeat_enabled)(bool enabled);
    void        (*set_mouse_cursor_hidden)(bool hidden);
    void        (*show_message_box)(char const *title, char const *message);
};

#if defined(TQ_USE_SDL)
    void libtq_construct_sdl_display(struct libtq_display_impl *display);
#endif

#if defined(TQ_PLATFORM_ANDROID)
    void libtq_construct_android_display(struct libtq_display_impl *display);
#endif

//------------------------------------------------------------------------------

void libtq_initialize_core(void);
void libtq_terminate_core(void);
bool libtq_process_core(void);

void tq_core_get_display_size(int *width, int *height);
void tq_core_set_display_size(int width, int height);
float core_get_display_aspect_ratio(void);

char const *tq_core_get_title(void);
void tq_core_set_title(char const *title);

bool tq_core_is_key_autorepeat_enabled(void);
void tq_core_set_key_autorepeat_enabled(bool enabled);

bool tq_core_is_key_pressed(tq_key key);
bool tq_core_is_mouse_button_pressed(tq_mouse_button mouse_button);

void tq_core_get_mouse_cursor_position(int *x, int *y);

float tq_core_get_time_mediump(void);
double tq_core_get_time_highp(void);
double tq_core_get_delta_time(void);

unsigned int tq_core_get_framerate(void);

void tq_core_on_key_pressed(tq_key key);
void tq_core_on_key_released(tq_key key);

bool core_is_mouse_cursor_hidden(void);
void core_set_mouse_cursor_hidden(bool hidden);

void tq_core_on_mouse_button_pressed(tq_mouse_button mouse_button);
void tq_core_on_mouse_button_released(tq_mouse_button mouse_button);
void tq_core_on_mouse_cursor_moved(int32_t x, int32_t y);
void tq_core_on_mouse_wheel_scrolled(float x_delta, float y_delta);

void tq_core_on_display_resized(int width, int height);

void tq_core_set_key_press_callback(tq_key_callback callback);
void tq_core_set_key_release_callback(tq_key_callback callback);
void tq_core_set_mouse_button_press_callback(tq_mouse_button_callback callback);
void tq_core_set_mouse_button_release_callback(tq_mouse_button_callback callback);
void tq_core_set_mouse_cursor_move_callback(tq_mouse_cursor_callback callback);
void tq_core_set_mouse_wheel_scroll_callback(tq_mouse_wheel_callback callback);

void tq_core_show_message_box(char const *title, char const *message);

//------------------------------------------------------------------------------

void            tq_core_sleep(double seconds);

libtq_thread    tq_core_create_thread(char const *name, int (*func)(void *), void *data);
void            tq_core_detach_thread(libtq_thread thread);
int             tq_core_wait_thread(libtq_thread thread);

libtq_mutex     tq_core_create_mutex(void);
void            tq_core_destroy_mutex(libtq_mutex mutex);
void            tq_core_lock_mutex(libtq_mutex mutex);
void            tq_core_unlock_mutex(libtq_mutex mutex);

//------------------------------------------------------------------------------

#endif // TQ_CORE_H_INC
