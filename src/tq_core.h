
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
    void        (*initialize)(void);
    void        (*terminate)(void);
    void        (*present)(void);
    bool        (*process_events)(void);
    void        (*set_size)(uint32_t, uint32_t);
    void        (*set_title)(char const *);
    void        (*set_key_autorepeat_enabled)(bool enabled);
    void        (*set_mouse_cursor_hidden)(bool hidden);
    void        (*show_message_box)(char const *title, char const *message);
    void        *(*get_gl_proc_addr)(char const *name);
    bool        (*check_gl_ext)(char const *name);
};

#if defined(_WIN32)
    void libtq_construct_win32_display(struct libtq_display_impl *display);
#endif

#if defined(TQ_USE_SDL)
    void libtq_construct_sdl_display(struct libtq_display_impl *display);
#endif

#if defined(TQ_PLATFORM_ANDROID)
    void libtq_construct_android_display(struct libtq_display_impl *display);
#endif

//------------------------------------------------------------------------------

void            libtq_initialize_core(void);
void            libtq_terminate_core(void);
bool            libtq_process_core(void);

void            libtq_get_display_size(int *width, int *height);
void            libtq_set_display_size(int width, int height);
float           libtq_get_display_aspect_ratio(void);

char const      *libtq_get_title(void);
void            libtq_set_title(char const *title);

bool            libtq_is_key_autorepeat_enabled(void);
void            libtq_set_key_autorepeat_enabled(bool enabled);

bool            libtq_is_key_pressed(tq_key key);
bool            libtq_is_mouse_button_pressed(tq_mouse_button mouse_button);

void            libtq_get_mouse_cursor_position(int *x, int *y);

float           libtq_get_time_mediump(void);
double          libtq_get_time_highp(void);
double          libtq_get_delta_time(void);

int             libtq_get_framerate(void);

void            libtq_on_key_pressed(tq_key key);
void            libtq_on_key_released(tq_key key);

bool            libtq_is_mouse_cursor_hidden(void);
void            libtq_set_mouse_cursor_hidden(bool hidden);

void            libtq_on_mouse_button_pressed(tq_mouse_button mouse_button);
void            libtq_on_mouse_button_released(tq_mouse_button mouse_button);
void            libtq_on_mouse_cursor_moved(int32_t x, int32_t y);
void            libtq_on_mouse_wheel_scrolled(float x_delta, float y_delta);

void            libtq_on_display_resize(int width, int height);

void            libtq_on_focus_gain(void);
void            libtq_on_focus_loss(void);

void            libtq_set_key_press_callback(tq_key_callback callback);
void            libtq_set_key_release_callback(tq_key_callback callback);
void            libtq_set_mousebutton_press_callback(tq_mouse_button_callback callback);
void            libtq_set_mousebutton_release_callback(tq_mouse_button_callback callback);
void            libtq_set_mousecursor_move_callback(tq_mouse_cursor_callback callback);
void            libtq_set_mousewheel_scroll_callback(tq_mouse_wheel_callback callback);

void            libtq_show_msgbox(char const *title, char const *message);

void            libtq_sleep(double seconds);

libtq_thread    libtq_create_thread(char const *name, int (*func)(void *), void *data);
void            libtq_detach_thread(libtq_thread thread);
int             libtq_wait_thread(libtq_thread thread);

libtq_mutex     libtq_create_mutex(void);
void            libtq_destroy_mutex(libtq_mutex mutex);
void            libtq_lock_mutex(libtq_mutex mutex);
void            libtq_unlock_mutex(libtq_mutex mutex);

void            *libtq_get_gl_proc_addr(char const *name);
bool            libtq_check_gl_ext(char const *name);

//------------------------------------------------------------------------------

#endif // TQ_CORE_H_INC
