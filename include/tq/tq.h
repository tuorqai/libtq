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
// tq library                                                     version 0.2.0
//------------------------------------------------------------------------------

#ifndef TQ_PUBLIC_H_INC
#define TQ_PUBLIC_H_INC

//------------------------------------------------------------------------------
// Standard headers

#include <math.h>
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
#   define TQ_NO_RET    __attribute__((__noreturn__))
#elif defined(_MSC_VER)
#   if defined(TQ_BUILD)
#       define TQ_API   __declspec(dllexport)
#   else
#       define TQ_API   __declspec(dllimport)
#   endif
#   define TQ_CALL      __cdecl
#   define TQ_NO_RET    __declspec(noreturn)
#else
#   define TQ_API
#   define TQ_CALL
#   define TQ_NO_RET
#endif

//------------------------------------------------------------------------------
// Constants

/**
 * @brief π number.
 */
#define TQ_PI                           (3.14159265358979323846)

/**
 * @brief Maximum number of textures.
 */
#define TQ_TEXTURE_LIMIT                (256)

/**
 * Maximum number of fonts.
 */
#define TQ_MAX_FONTS                    (16)

/**
 * @brief Maximum number of open music tracks.
 */
#define TQ_MUSIC_LIMIT                  (32)

/**
 * @brief Maximum number of sounds.
 */
#define TQ_SOUND_LIMIT                  (256)

/**
 * @brief Maximum number of playing audio channels.
 */
#define TQ_CHANNEL_LIMIT                (16)

/**
 * Regular font weight.
 */
#define TQ_FONT_NORMAL                  (400)

/**
 * Cx.
 */
#define TQ_FONT_BOLD                    (700)

//------------------------------------------------------------------------------
// Enumerations

/**
 * @brief Enumeration of keyboard keys.
 */
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

/**
 * @brief Enumeration of mouse buttons.
 */
typedef enum tq_mouse_button
{
    TQ_MOUSE_BUTTON_LEFT,
    TQ_MOUSE_BUTTON_RIGHT,
    TQ_MOUSE_BUTTON_MIDDLE,
    TQ_TOTAL_MOUSE_BUTTONS,
} tq_mouse_button_t;

/**
 * @brief Enumeration of joystick axes.
 */
typedef enum tq_joystick_axis
{
    TQ_TOTAL_JOYSTICK_AXES,
} tq_joystick_axis_t;

/**
 * @brief Enumeration of joystick buttons.
 */
typedef enum tq_joystick_button
{
    TQ_TOTAL_JOYSTICK_BUTTONS,
} tq_joystick_button_t;

/**
 * @brief Enumeration of audio channel states.
 */
typedef enum tq_channel_state
{
    TQ_CHANNEL_STATE_INACTIVE,
    TQ_CHANNEL_STATE_PAUSED,
    TQ_CHANNEL_STATE_PLAYING,
} tq_channel_state_t;

//------------------------------------------------------------------------------
// Typedefs and structs

/**
 * @brief 32-bit color value.
 */
typedef uint32_t tq_color_t;

/**
 * Texture identifier.
 */
typedef struct { int32_t id; } tq_texture;

/**
 * Font identifier.
 */
typedef struct { int32_t id; } tq_font;

/**
 * Sound identifier.
 */
typedef struct { int32_t id; } tq_sound;

/**
 * Music identifier.
 */
typedef struct { int32_t id; } tq_music;

/**
 * Channel identifier.
 */
typedef struct { int32_t id; } tq_channel;

/**
 * @brief Keyboard event callback.
 */
typedef void (*tq_key_callback_t)(tq_key_t key);

/**
 * @brief Mouse button event callback.
 */
typedef void (*tq_mouse_button_callback_t)(tq_mouse_button_t mouse_button, int32_t x, int32_t y);

/**
 * @brief Mouse cursor event callback.
 */
typedef void (*tq_mouse_cursor_callback_t)(int32_t x, int32_t y);

/**
 * @brief Mouse wheel event callback.
 */
typedef void (*tq_mouse_wheel_callback_t)(float delta, int32_t x, int32_t y);

/**
 * Two-dimensional vector of integer numbers.
 */
typedef struct tq_vec2i
{
    int32_t x;
    int32_t y;
} tq_vec2i;

/**
 * Two-dimensional vector of unsigned integer numbers.
 */
typedef struct tq_vec2u
{
    uint32_t x;
    uint32_t y;
} tq_vec2u;

/**
 * Two-dimensional vector of floating-point numbers.
 */
typedef struct tq_vec2f
{
    float x;
    float y;
} tq_vec2f;

//------------------------------------------------------------------------------

typedef struct tq_vec2i tq_vec2i_t;
typedef struct tq_vec2u tq_vec2u_t;
typedef struct tq_vec2f tq_vec2f_t;

//------------------------------------------------------------------------------

/**
 * @brief Initialize the library.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_initialize(void);

/**
 * @brief Terminate the library and release all resources.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_terminate(void);

/**
 * @brief Process user input and do other important things.
 */
TQ_EXPORT TQ_API bool TQ_CALL tq_process(void);

//------------------------------------------------------------------------------
// Core

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

/**
 * Check if keyboard auto-repeat feature is enabled.
 */
TQ_EXPORT TQ_API bool TQ_CALL tq_is_key_autorepeat_enabled(void);

/**
 * Set keyboard auto-repeat feature. If enabled, your key-press callback
 * will be called repeatedly when a key is hold. This feature is enabled by
 * default.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_set_key_autorepeat_enabled(bool enabled);

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

/**
 * @brief Set callback function that will be called when a key is pressed.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_on_key_pressed(tq_key_callback_t callback);

/**
 * @brief Set callback function that will be called when a key is released.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_on_key_released(tq_key_callback_t callback);

/**
 * @brief Set callback function that will be called when a mouse button is pressed.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_on_mouse_button_pressed(tq_mouse_button_callback_t callback);

/**
 * @brief Set callback function that will be called when a mouse button is released.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_on_mouse_button_released(tq_mouse_button_callback_t callback);

/**
 * @brief Set callback function that will be called when a mouse cursor is moved.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_on_mouse_cursor_moved(tq_mouse_cursor_callback_t callback);

/**
 * Register a function that will be called when a mouse wheel is scrolled.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_on_mouse_wheel_scrolled(tq_mouse_wheel_callback_t callback);

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

/**
 * Get single color value from red, green and blue values.
 */
#define TQ_COLOR24(r, g, b) \
    ((tq_color_t) ((r) << 24) | ((g) << 16) | ((b) << 8) | (255))

/**
 * Get single color value from red, green, blue and alpha values.
 */
#define TQ_COLOR32(r, g, b, a) \
    ((tq_color_t) ((r) << 24) | ((g) << 16) | ((b) << 8) | (a))

/**
 * Extract RED channel from a color value.
 */
#define TQ_EXTRACT_R(c) \
    (((c) >> 24) & 255)

/**
 * Extract GREEN channel from a color value.
 */
#define TQ_EXTRACT_G(c) \
    (((c) >> 16) & 255)

/**
 * Extract BLUE channel from a color value.
 */
#define TQ_EXTRACT_B(c) \
    (((c) >> 8) & 255)

/**
 * Extract ALPHA channel from a color value.
 */
#define TQ_EXTRACT_A(c) \
    (((c) >> 0) & 255)

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

/** @brief Set the view rectangle.
 *
 * @note The view is reset every frame.
 *
 * @param x,y      Center of the view.
 * @param w,h      Size of the view rectangle.
 * @param rotation Rotation of the view, in degrees.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_view(
    float x, float y,
    float w, float h,
    float rotation
);

/** @brief Set the view rectangle (vector arguments).
 *
 * @note The view is reset every frame.
 *
 * @param center    Center of the view.
 * @param size      Size of the view rectangle.
 * @param rotation  Rotation of the view, in degrees.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_view_v(
    tq_vec2f_t center,
    tq_vec2f_t size,
    float rotation
);

//----------------------------------------------------------
// Transformation matrix

/* Duplicate current transformation matrix and push it to the matrix stack. */
TQ_EXPORT TQ_API void TQ_CALL tq_push_matrix(void);

/* Replace current transformation matrix by the first matrix in the stack, popping it from there. */
TQ_EXPORT TQ_API void TQ_CALL tq_pop_matrix(void);

/* Translate current transformation matrix. */
TQ_EXPORT TQ_API void TQ_CALL tq_translate_matrix(tq_vec2f translate);

/* Scale current transformation matrix. */
TQ_EXPORT TQ_API void TQ_CALL tq_scale_matrix(tq_vec2f scale);

/* Rotate current transformation matrix. */
TQ_EXPORT TQ_API void TQ_CALL tq_rotate_matrix(float degrees);

//--------------------------------------
// Primitives

/* Draw a point. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_point_f(float x, float y);

/* Draw a point (vector arguments). */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_point_v(tq_vec2f_t position);

/* Draws a line. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_line_f(float ax, float ay, float bx, float by);

/* Draws a line. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_line_v(tq_vec2f_t a, tq_vec2f_t b);

/* Fills and outlines a triangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_triangle_f(float ax, float ay, float bx, float by, float cx, float cy);

/* Fills and outlines a triangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_triangle_v(tq_vec2f_t a, tq_vec2f_t b, tq_vec2f_t c);

/* Fills and outlines a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_rectangle_f(float x, float y, float w, float h);

/* Fills and outlines a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_rectangle_v(tq_vec2f_t position, tq_vec2f_t size);

/* Fills and outlines a circle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_circle_f(float x, float y, float radius);

/* Fills and outlines a circle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_circle_v(tq_vec2f_t position, float radius);

/* Outlines a triangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_outline_triangle_f(float ax, float ay, float bx, float by, float cx, float cy);

/* Outlines a triangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_outline_triangle_v(tq_vec2f_t a, tq_vec2f_t b, tq_vec2f_t c);

/* Outlines a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_outline_rectangle_f(float x, float y, float w, float h);

/* Outlines a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_outline_rectangle_v(tq_vec2f_t position, tq_vec2f_t size);

/* Outlines a circle. */
TQ_EXPORT TQ_API void TQ_CALL tq_outline_circle_f(float x, float y, float radius);

/* Outlines a circle. */
TQ_EXPORT TQ_API void TQ_CALL tq_outline_circle_v(tq_vec2f_t position, float radius);

/* Fills a triangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_fill_triangle_f(float ax, float ay, float bx, float by, float cx, float cy);

/* Fills a triangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_fill_triangle_v(tq_vec2f_t a, tq_vec2f_t b, tq_vec2f_t c);

/* Fills a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_fill_rectangle_f(float x, float y, float w, float h);

/* Fills a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_fill_rectangle_v(tq_vec2f_t position, tq_vec2f_t size);

/* Fills a circle. */
TQ_EXPORT TQ_API void TQ_CALL tq_fill_circle_f(float x, float y, float radius);

/* Fills a circle. */
TQ_EXPORT TQ_API void TQ_CALL tq_fill_circle_v(tq_vec2f_t position, float radius);

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

/* Load texture from a file. */
TQ_EXPORT TQ_API tq_texture TQ_CALL tq_load_texture_from_file(char const *path);

/* Load texture from a memory buffer. */
TQ_EXPORT TQ_API tq_texture TQ_CALL tq_load_texture_from_memory(uint8_t const *buffer, size_t length);

/* Delete a texture from video memory. */
TQ_EXPORT TQ_API void TQ_CALL tq_delete_texture(tq_texture texture);

/* Get the width of a texture. */
TQ_EXPORT TQ_API int TQ_CALL tq_get_texture_width(tq_texture texture);

/* Get the height of a texture. */
TQ_EXPORT TQ_API int TQ_CALL tq_get_texture_height(tq_texture texture);

/* Get both width and height of a texture. Both pointers should be valid. */
TQ_EXPORT TQ_API void TQ_CALL tq_get_texture_size(tq_texture texture, int *width, int *height);

/* Draw the texture inside a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_texture_f(
    tq_texture texture,
    float x, float y,
    float w, float h
);

/* Draw the texture inside a rectangle. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_texture_v(
    tq_texture texture,
    tq_vec2f_t position,
    tq_vec2f_t size
);

/* Draw a part of the texture inside a rectangle. Texture coordinates should be in pixel space. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_texture_fragment_f(
    tq_texture texture,
    float x, float y,
    float w, float h,
    float fx, float fy,
    float fw, float fh
);

/* Draw a part of the texture inside a rectangle. Texture coordinates should be in pixel space. */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_texture_fragment_v(
    tq_texture texture,
    tq_vec2f_t position,
    tq_vec2f_t size,
    tq_vec2f_t fragment_position,
    tq_vec2f_t fragment_size
);

//--------------------------------------
// Fonts

/**
 * Load font from file.
 */
TQ_EXPORT TQ_API tq_font TQ_CALL tq_load_font_from_file(
    char const *path,
    float pt, int weight);

/**
 * Load font from memory buffer.
 */
TQ_EXPORT TQ_API tq_font TQ_CALL tq_load_font_from_memory(
    uint8_t const *buffer, size_t size,
    float pt, int weight);

/**
 * Delete previously loaded font.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_delete_font(tq_font font);

/**
 * Draw simple text using specified font.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_draw_text(tq_font font, tq_vec2f_t position, char const *text);

/**
 * Draw formatted text using specified font.
 * See also: tq_set_fill_color(), tq_set_outline_color().
 */
TQ_EXPORT TQ_API void TQ_CALL tq_print_text(tq_font font, tq_vec2f_t position, char const *fmt, ...);

//------------------------------------------------------------------------------
// Audio

/**
 * Decode and load sound to memory from a file.
 */
TQ_EXPORT TQ_API tq_sound TQ_CALL tq_load_sound_from_file(char const *path);

/**
 * Decode sound from a memory buffer.
 */
TQ_EXPORT TQ_API tq_sound TQ_CALL tq_load_sound_from_memory(uint8_t const *buffer, size_t length);

/**
 * Delete sound from memory.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_delete_sound(tq_sound sound);

/**
 * Play sound.
 */
TQ_EXPORT TQ_API tq_channel TQ_CALL tq_play_sound(tq_sound sound, int loop);

/**
 * Open music stream from a file.
 */
TQ_EXPORT TQ_API tq_music TQ_CALL tq_open_music_from_file(char const *path);

/**
 * Open music stream from memory buffer.
 */
TQ_EXPORT TQ_API tq_music TQ_CALL tq_open_music_from_memory(uint8_t const *buffer, size_t length);

/**
 * Close music stream.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_close_music(tq_music music);

/**
 * Choose any unused channel and play music on it.
 */
TQ_EXPORT TQ_API tq_channel TQ_CALL tq_play_music(tq_music music, int loop);

/**
 * Get current state of the audio channel.
 */
TQ_EXPORT TQ_API tq_channel_state_t TQ_CALL tq_get_channel_state(tq_channel channel);

/**
 * Pause the audio channel.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_pause_channel(tq_channel channel);

/**
 * Continue playback of the audio channel.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_unpause_channel(tq_channel channel);

/**
 * Stop the audio channel and mark it inactive.
 */
TQ_EXPORT TQ_API void TQ_CALL tq_stop_channel(tq_channel channel);

//------------------------------------------------------------------------------
// Math

/**
 * @brief Get smallest value.
 */
#define TQ_MIN(a, b)                (((a) < (b)) ? (a) : (b))

/**
 * @brief Get largest value.
 */
#define TQ_MAX(a, b)                (((a) > (b)) ? (a) : (b))

/**
 * @brief Converts degrees to radians.
 */
#define TQ_DEG2RAD(deg)             ((deg) * (TQ_PI / 180.0))

/**
 * @brief Converts radians to degrees.
 */
#define TQ_RAD2DEG(rad)             ((rad) * (180.0 / TQ_PI))

/**
 * Construct 2-component integer vector.
 */
#define TQ_VEC2I(x, y)              ((tq_vec2i_t) { (x), (y) })

/**
 * Construct 2-component unsigned integer vector.
 */
#define TQ_VEC2U(x, y)              ((tq_vec2u_t) { (x), (y) })

/**
 * Construct 2-component floating-point vector.
 */
#define TQ_VEC2F(x, y)              ((tq_vec2f_t) { (x), (y) })

/**
 * @brief Cast 2-component integer vector to floating-point vector.
 */
static inline tq_vec2f_t tq_vec2i_cast(tq_vec2i_t ivec)
{
    return (tq_vec2f_t) {
        .x = (float) ivec.x,
        .y = (float) ivec.y,
    };
}

/**
 * @brief Cast 2-component unsigned integer vector to floating-point vector.
 */
static inline tq_vec2f_t tq_vec2u_cast(tq_vec2u_t uvec)
{
    return (tq_vec2f_t) {
        .x = (float) uvec.x,
        .y = (float) uvec.y,
    };
}

/**
 * @brief Add two floating-point 2D vectors.
 */
static inline tq_vec2f_t tq_vec2f_add(tq_vec2f_t v0, tq_vec2f_t v1)
{
    return (tq_vec2f_t) {
        .x = v0.x + v1.x,
        .y = v0.y + v1.y,
    };
}

/**
 * @brief Subtract two floating-point 2D vectors.
 */
static inline tq_vec2f_t tq_vec2f_subtract(tq_vec2f_t v0, tq_vec2f_t v1)
{
    return (tq_vec2f_t) {
        .x = v0.x - v1.x,
        .y = v0.y - v1.y,
    };
}

/**
 * @brief Multiply two floating-point 2D vectors.
 */
static inline tq_vec2f_t tq_vec2f_multiply(tq_vec2f_t v0, tq_vec2f_t v1)
{
    return (tq_vec2f_t) {
        .x = v0.x * v1.x,
        .y = v0.y * v1.y,
    };
}

/**
 * @brief Scale a floating-point 2D vector.
 */
static inline tq_vec2f_t tq_vec2f_scale(tq_vec2f_t vec, float f)
{
    return (tq_vec2f_t) {
        .x = vec.x * f,
        .y = vec.y * f,
    };
}

/**
 * @brief Get length of a floating-point 2D vector.
 */
static inline float tq_vec2f_length(tq_vec2f_t vec)
{
    return sqrtf((vec.x * vec.x) + (vec.y * vec.y));
}

/**
 * @brief Normalize a floating-point 2D vector.
 */
static inline tq_vec2f_t tq_vec2f_normalize(tq_vec2f_t vec)
{
    float const length = tq_vec2f_length(vec);

    return (tq_vec2f_t) {
        .x = vec.x / length,
        .y = vec.y / length,
    };
}

/**
 * @brief Calculate the distance between two vectors.
 */
static inline float tq_vec2f_distance(tq_vec2f_t v0, tq_vec2f_t v1)
{
    return sqrtf(((v1.y - v0.y) * (v1.y - v0.y)) + ((v1.x - v0.x) * (v1.x - v0.x)));
}

/**
 * @brief Calculate an angle needed to aim to the target.
 */
static inline float tq_vec2f_look_at(tq_vec2f_t observer, tq_vec2f_t target)
{
    float const u = target.x - observer.x;
    float const v = target.y - observer.y;

    return TQ_RAD2DEG(atan2f(v, u));
}

//------------------------------------------------------------------------------

#endif // TQ_PUBLIC_H_INC

//------------------------------------------------------------------------------
