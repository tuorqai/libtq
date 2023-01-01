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

#if defined(TQ_WIN32)
    void libtq_construct_win32_clock(struct libtq_clock_impl *clock);
#endif

#if defined(TQ_LINUX) || defined(TQ_ANDROID) || defined(TQ_EMSCRIPTEN)
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

#if defined(TQ_WIN32)
    void libtq_construct_win32_threads(struct libtq_threads_impl *threads);
#endif

#if defined(TQ_LINUX) || defined(TQ_ANDROID) || defined(TQ_EMSCRIPTEN)
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

#if defined(TQ_WIN32)
    void libtq_construct_win32_display(struct libtq_display_impl *display);
#endif

#if defined(TQ_LINUX) || defined(TQ_EMSCRIPTEN)
    void libtq_construct_sdl_display(struct libtq_display_impl *display);
#endif

#if defined(TQ_ANDROID)
    void libtq_construct_android_display(struct libtq_display_impl *display);
#endif

//------------------------------------------------------------------------------

void            tq_initialize_core(void);
void            tq_terminate_core(void);
bool            tq_process_core(void);

float           libtq_get_display_aspect_ratio(void);

void            libtq_on_key_pressed(tq_key key);
void            libtq_on_key_released(tq_key key);

void            libtq_on_mouse_button_pressed(tq_mouse_button mouse_button);
void            libtq_on_mouse_button_released(tq_mouse_button mouse_button);
void            libtq_on_mouse_cursor_moved(int32_t x, int32_t y);
void            libtq_on_mouse_wheel_scrolled(float x_delta, float y_delta);

void            libtq_on_display_resize(int width, int height);

void            libtq_on_focus_gain(void);
void            libtq_on_focus_loss(void);

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
