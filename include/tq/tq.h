//------------------------------------------------------------------------------
// Copyright (c) 2021-2022 tuorqai
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
// tq library: main and only public header
//------------------------------------------------------------------------------

#ifndef TQ_PUBLIC_H_INC
#define TQ_PUBLIC_H_INC

//------------------------------------------------------------------------------
// Standard headers

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Compiler-dependent macros

#if defined(__cplusplus)
#   define TQ_EXPORT    extern "C"
#else
#   define TQ_EXPORT    extern
#endif

#if defined(__GNUC__)
#   define TQ_API       __attribute__((visibility("default")))
#   define TQ_CALL
#elif defined(_MSC_VER)
#   if defined(TQ_BUILD)
#       define TQ_API   __declspec(dllexport)
#   else
#       define TQ_API   __declspec(dllimport)
#   endif
#   define TQ_CALL      __cdecl
#else
#   define TQ_API
#   define TQ_CALL
#endif

//------------------------------------------------------------------------------
// Basic constants

/* This value is returned if there is a failure while loading or allocating a resource. */
#define TQ_INVALID_HANDLE       (-1)

//------------------------------------------------------------------------------
// Basic typedefs

/* Opaque handle to any resource (sound, texture, etc). */
typedef int32_t tq_handle_t;

//------------------------------------------------------------------------------
// Math-related things

/* Two-dimensional vector. */
typedef struct tq_vec2
{
    float x;
    float y;
} tq_vec2_t;

//------------------------------------------------------------------------------
// Meta

/* Initialize the library. */
TQ_EXPORT TQ_API void TQ_CALL tq_initialize(void);

/* Terminate the library and release all resources. */
TQ_EXPORT TQ_API void TQ_CALL tq_terminate(void);

/* This function should be called every frame. */
TQ_EXPORT TQ_API bool TQ_CALL tq_process(void);

//------------------------------------------------------------------------------
// Core

/* Keyboard keys. */
typedef enum tq_key
{
    TQ_KEY_0,
    TQ_KEY_1,
    TQ_KEY_2,
    TQ_KEY_3,
    TQ_KEY_4,
    TQ_KEY_5,
    TQ_KEY_6,
    TQ_KEY_7,
    TQ_KEY_8,
    TQ_KEY_9,
    TQ_KEY_A,
    TQ_KEY_B,
    TQ_KEY_C,
    TQ_KEY_D,
    TQ_KEY_E,
    TQ_KEY_F,
    TQ_KEY_G,
    TQ_KEY_H,
    TQ_KEY_I,
    TQ_KEY_J,
    TQ_KEY_K,
    TQ_KEY_L,
    TQ_KEY_M,
    TQ_KEY_N,
    TQ_KEY_O,
    TQ_KEY_P,
    TQ_KEY_Q,
    TQ_KEY_R,
    TQ_KEY_S,
    TQ_KEY_T,
    TQ_KEY_U,
    TQ_KEY_V,
    TQ_KEY_W,
    TQ_KEY_X,
    TQ_KEY_Y,
    TQ_KEY_Z,
    TQ_KEY_GRAVE,
    TQ_KEY_APOSTROPHE,
    TQ_KEY_MINUS,
    TQ_KEY_EQUAL,
    TQ_KEY_LBRACKET,
    TQ_KEY_RBRACKET,
    TQ_KEY_COMMA,
    TQ_KEY_PERIOD,
    TQ_KEY_SEMICOLON,
    TQ_KEY_SLASH,
    TQ_KEY_BACKSLASH,
    TQ_KEY_SPACE,
    TQ_KEY_ESCAPE,
    TQ_KEY_BACKSPACE,
    TQ_KEY_TAB,
    TQ_KEY_ENTER,
    TQ_KEY_F1,
    TQ_KEY_F2,
    TQ_KEY_F3,
    TQ_KEY_F4,
    TQ_KEY_F5,
    TQ_KEY_F6,
    TQ_KEY_F7,
    TQ_KEY_F8,
    TQ_KEY_F9,
    TQ_KEY_F10,
    TQ_KEY_F11,
    TQ_KEY_F12,
    TQ_KEY_UP,
    TQ_KEY_DOWN,
    TQ_KEY_LEFT,
    TQ_KEY_RIGHT,
    TQ_KEY_LSHIFT,
    TQ_KEY_RSHIFT,
    TQ_KEY_LCTRL,
    TQ_KEY_RCTRL,
    TQ_KEY_LALT,
    TQ_KEY_RALT,
    TQ_KEY_LSUPER,
    TQ_KEY_RSUPER,
    TQ_KEY_MENU,
    TQ_KEY_PGUP,
    TQ_KEY_PGDN,
    TQ_KEY_HOME,
    TQ_KEY_END,
    TQ_KEY_INSERT,
    TQ_KEY_DELETE,
    TQ_KEY_PRINTSCREEN,
    TQ_KEY_PAUSE,
    TQ_KEY_CAPSLOCK,
    TQ_KEY_SCROLLLOCK,
    TQ_KEY_NUMLOCK,
    TQ_KEY_KP_0,
    TQ_KEY_KP_1,
    TQ_KEY_KP_2,
    TQ_KEY_KP_3,
    TQ_KEY_KP_4,
    TQ_KEY_KP_5,
    TQ_KEY_KP_6,
    TQ_KEY_KP_7,
    TQ_KEY_KP_8,
    TQ_KEY_KP_9,
    TQ_KEY_KP_MUL,
    TQ_KEY_KP_ADD,
    TQ_KEY_KP_SUB,
    TQ_KEY_KP_POINT,
    TQ_KEY_KP_DIV,
    TQ_KEY_KP_ENTER,
    TQ_TOTAL_KEYS,
} tq_key_t;

/* Mouse buttons. */
typedef enum tq_mouse_button
{
    TQ_MOUSE_BUTTON_LEFT,
    TQ_MOUSE_BUTTON_RIGHT,
    TQ_MOUSE_BUTTON_MIDDLE,
    TQ_TOTAL_MOUSE_BUTTONS,
} tq_mouse_button_t;

/* Joystick axes. */
typedef enum tq_joystick_axis
{
    TQ_TOTAL_JOYSTICK_AXES,
} tq_joystick_axis_t;

/* Joystick buttons. */
typedef enum tq_joystick_button
{
    TQ_TOTAL_JOYSTICK_BUTTONS,
} tq_joystick_button_t;

/* Returns display (window) width. */
TQ_EXPORT TQ_API uint32_t TQ_CALL tq_get_display_width(void);

/* Returns display (window) height. */
TQ_EXPORT TQ_API uint32_t TQ_CALL tq_get_display_height(void);

/* Returns display size. */
TQ_EXPORT TQ_API void TQ_CALL tq_get_display_size(uint32_t *width, uint32_t *height);

/* Changes or sets display (window) size. Can be called before initialization. */
TQ_EXPORT TQ_API void TQ_CALL tq_set_display_size(uint32_t width, uint32_t height);

/* Returns current application title. */
TQ_EXPORT TQ_API char const * TQ_CALL tq_get_title(void);

/* Changes or sets the application title. Can be called before initialization. */
TQ_EXPORT TQ_API void TQ_CALL tq_set_title(char const *title);

/* Checks if a key is pressed. */
TQ_EXPORT TQ_API bool TQ_CALL tq_is_key_pressed(tq_key_t key);

/* Checks if a mouse button is pressed. */
TQ_EXPORT TQ_API bool TQ_CALL tq_is_mouse_button_pressed(tq_mouse_button_t mouse_button);

/* Returns mouse cursor X position. */
TQ_EXPORT TQ_API int32_t TQ_CALL tq_get_mouse_cursor_x(void);

/* Returns mouse cursor Y position. */
TQ_EXPORT TQ_API int32_t TQ_CALL tq_get_mouse_cursor_y(void);

/* Returns mouse cursor position to the given arguments. Both should be valid pointers. */
TQ_EXPORT TQ_API void TQ_CALL tq_get_mouse_cursor_position(int32_t *x, int32_t *y);

/* Returns how many seconds passed since the library initialization (millisecond precision). */
TQ_EXPORT TQ_API float TQ_CALL tq_get_time_mediump(void);

/* Returns how many seconds passed since the library initialization (nanosecond precision). */
TQ_EXPORT TQ_API double TQ_CALL tq_get_time_highp(void);

/* Returns delta time between current and previous frames. */
TQ_EXPORT TQ_API double TQ_CALL tq_get_delta_time(void);

/* Get the current framerate in frames per second. */
TQ_EXPORT TQ_API unsigned int TQ_CALL tq_get_framerate(void);

//------------------------------------------------------------------------------
// Graphics

//----------------------------------------------------------
// Colors

/* 32-bit color value. */
typedef uint32_t tq_color_t;

/* Get single color value from RGB values. */
static inline tq_color_t tq_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 24) | (g << 16) | (b << 8) | (255);
}

/* Get single color value from RGBA values. */
static inline tq_color_t tq_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (r << 24) | (g << 16) | (b << 8) | (a << 0);
}

//----------------------------------------------------------
// Background

/* Clear the screen. */
TQ_EXPORT TQ_API void TQ_CALL tq_clear(void);

/* Get the current background color. */
TQ_EXPORT TQ_API tq_color_t TQ_CALL tq_get_clear_color(void);

/* Set the current background color. */
TQ_EXPORT TQ_API void TQ_CALL tq_set_clear_color(tq_color_t clear_color);

//----------------------------------------------------------
// Views

/* Set current view rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_set_view_f(float x, float y, float width, float height, float angle);

/* Set current view rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_set_view_v(tq_vec2_t position, tq_vec2_t size, float angle);

/* Reset view to its default state. */
TQ_EXPORT TQ_API void TQ_CALL tq_reset_view(void);

//----------------------------------------------------------
// Transformation matrix

/* Duplicate current transformation matrix and push it to the matrix stack. */
TQ_EXPORT TQ_API void TQ_CALL tq_push_matrix(void);

/* Replace current transformation matrix by the first matrix in the stack, popping it from there. */
TQ_EXPORT TQ_API void TQ_CALL tq_pop_matrix(void);

/* Translate current transformation matrix. */
TQ_EXPORT TQ_API void TQ_CALL tq_translate_matrix_f(float x, float y);

/* Translate current transformation matrix. */
TQ_EXPORT TQ_API void TQ_CALL tq_translate_matrix_v(tq_vec2_t v);

/* Scale current transformation matrix. */
TQ_EXPORT TQ_API void TQ_CALL tq_scale_matrix_f(float x, float y);

/* Scale current transformation matrix. */
TQ_EXPORT TQ_API void TQ_CALL tq_scale_matrix_v(tq_vec2_t v);

/* Rotate current transformation matrix. */
TQ_EXPORT TQ_API void TQ_CALL tq_rotate_matrix(float degrees);

//--------------------------------------
// Primitives

/* Draw a point. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_point_f(float x, float y);

/* Draw a point (vector arguments). */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_point_v(tq_vec2_t position);

/* Draws a line. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_line_f(float ax, float ay, float bx, float by);

/* Draws a line. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_line_v(tq_vec2_t a, tq_vec2_t b);

/* Fills and outlines a triangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_triangle_f(float ax, float ay, float bx, float by, float cx, float cy);

/* Fills and outlines a triangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_triangle_v(tq_vec2_t a, tq_vec2_t b, tq_vec2_t c);

/* Fills and outlines a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_rectangle_f(float x, float y, float w, float h);

/* Fills and outlines a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_rectangle_v(tq_vec2_t position, tq_vec2_t size);

/* Fills and outlines a circle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_circle_f(float x, float y, float radius);

/* Fills and outlines a circle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_circle_v(tq_vec2_t position, float radius);

/* Outlines a triangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_outline_triangle_f(float ax, float ay, float bx, float by, float cx, float cy);

/* Outlines a triangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_outline_triangle_v(tq_vec2_t a, tq_vec2_t b, tq_vec2_t c);

/* Outlines a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_outline_rectangle_f(float x, float y, float w, float h);

/* Outlines a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_outline_rectangle_v(tq_vec2_t position, tq_vec2_t size);

/* Outlines a circle. */
TQ_EXPORT TQ_API void TQ_CALL tq_outline_circle_f(float x, float y, float radius);

/* Outlines a circle. */
TQ_EXPORT TQ_API void TQ_CALL tq_outline_circle_v(tq_vec2_t position, float radius);

/* Fills a triangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_fill_triangle_f(float ax, float ay, float bx, float by, float cx, float cy);

/* Fills a triangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_fill_triangle_v(tq_vec2_t a, tq_vec2_t b, tq_vec2_t c);

/* Fills a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_fill_rectangle_f(float x, float y, float w, float h);

/* Fills a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_fill_rectangle_v(tq_vec2_t position, tq_vec2_t size);

/* Fills a circle. */
TQ_EXPORT TQ_API void TQ_CALL tq_fill_circle_f(float x, float y, float radius);

/* Fills a circle. */
TQ_EXPORT TQ_API void TQ_CALL tq_fill_circle_v(tq_vec2_t position, float radius);

/* Returns current point color (used by draw_point). */
TQ_EXPORT TQ_API tq_color_t TQ_CALL tq_get_point_color(void);

/* Replaces current point color (used by draw_point). */
TQ_EXPORT TQ_API void TQ_CALL tq_set_point_color(tq_color_t point_color);

/* Returns current line color (used by draw_line). */
TQ_EXPORT TQ_API tq_color_t TQ_CALL tq_get_line_color(void);

/* Replaces current line color (used by draw_line). */
TQ_EXPORT TQ_API void TQ_CALL tq_set_line_color(tq_color_t line_color);

/* Returns current outline color. */
TQ_EXPORT TQ_API tq_color_t TQ_CALL tq_get_outline_color(void);

/* Replaces current outline color. */
TQ_EXPORT TQ_API void TQ_CALL tq_set_outline_color(tq_color_t outline_color);

/* Returns current fill color. */
TQ_EXPORT TQ_API tq_color_t TQ_CALL tq_get_fill_color(void);

/* Replaces current fill color. */
TQ_EXPORT TQ_API void TQ_CALL tq_set_fill_color(tq_color_t fill_color);

//--------------------------------------
// Textures

#define TQ_TEXTURE_LIMIT        256

/* Load texture from a file. */
TQ_EXPORT TQ_API tq_handle_t TQ_CALL tq_load_texture_from_file(char const *path);

/* Load texture from a memory buffer. */
TQ_EXPORT TQ_API tq_handle_t TQ_CALL tq_load_texture_from_memory(uint8_t const *buffer, size_t length);

/* Delete a texture from video memory. */
TQ_EXPORT TQ_API void TQ_CALL tq_delete_texture(tq_handle_t texture_handle);

/* Get the width of a texture. */
TQ_EXPORT TQ_API uint32_t TQ_CALL tq_get_texture_width(tq_handle_t texture_handle);

/* Get the height of a texture. */
TQ_EXPORT TQ_API uint32_t TQ_CALL tq_get_texture_height(tq_handle_t texture_handle);

/* Get both width and height of a texture. Both pointers should be valid. */
TQ_EXPORT TQ_API void TQ_CALL tq_get_texture_size(tq_handle_t texture_handle, uint32_t *width, uint32_t *height);

/* Draw the texture inside a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_texture_f(
    tq_handle_t texture_handle,
    float x, float y,
    float w, float h
);

/* Draw the texture inside a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_texture_v(
    tq_handle_t texture_handle,
    tq_vec2_t position,
    tq_vec2_t size
);

/* Draw a part of the texture inside a rectangle. Texture coordinates should be in pixel space. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_texture_fragment_f(
    tq_handle_t texture_handle,
    float x, float y,
    float w, float h,
    float u0, float v0,
    float u1, float v1
);

/* Draw a part of the texture inside a rectangle. Texture coordinates should be in pixel space. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_texture_fragment_v(
    tq_handle_t texture_handle,
    tq_vec2_t position,
    tq_vec2_t size,
    tq_vec2_t uv0,
    tq_vec2_t uv1
);

//------------------------------------------------------------------------------
// Audio

#define TQ_MUSIC_LIMIT              32
#define TQ_SOUND_LIMIT              256
#define TQ_WAVE_LIMIT               16
#define TQ_SOUND_STREAM_LIMIT       16

typedef enum tq_wave_state
{
    TQ_WAVE_STATE_INACTIVE,
    TQ_WAVE_STATE_PAUSED,
    TQ_WAVE_STATE_PLAYING,
} tq_wave_state_t;

TQ_EXPORT TQ_API tq_handle_t TQ_CALL tq_load_sound_from_file(char const *path);
TQ_EXPORT TQ_API tq_handle_t TQ_CALL tq_load_sound_from_memory(uint8_t const *buffer, size_t length);
TQ_EXPORT TQ_API void TQ_CALL tq_delete_sound(tq_handle_t sound_handle);
TQ_EXPORT TQ_API tq_handle_t TQ_CALL tq_play_sound(tq_handle_t sound_handle, float left_volume, float right_volume, int loop);

TQ_EXPORT TQ_API tq_handle_t TQ_CALL tq_open_music_from_file(char const *path);
TQ_EXPORT TQ_API tq_handle_t TQ_CALL tq_open_music_from_memory(uint8_t const *buffer, size_t length);
TQ_EXPORT TQ_API void TQ_CALL tq_close_music(tq_handle_t music_handle);
TQ_EXPORT TQ_API tq_handle_t TQ_CALL tq_play_music(tq_handle_t music_handle, int loop);

TQ_EXPORT TQ_API tq_wave_state_t TQ_CALL tq_get_wave_state(tq_handle_t wave_handle);
TQ_EXPORT TQ_API void TQ_CALL tq_pause_wave(tq_handle_t wave_handle);
TQ_EXPORT TQ_API void TQ_CALL tq_unpause_wave(tq_handle_t wave_handle);
TQ_EXPORT TQ_API void TQ_CALL tq_stop_wave(tq_handle_t wave_handle);

//------------------------------------------------------------------------------

#endif // TQ_PUBLIC_H_INC

//------------------------------------------------------------------------------
