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
// tq library
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
// Library version

#define TQ_VERSION          "0.3.2"

//------------------------------------------------------------------------------
// Compiler-dependent macros

#if defined(__cplusplus)
#   define TQ_LINKAGE       extern "C"
#else
#   define TQ_LINKAGE       extern
#endif

#if defined(__GNUC__)
#   define TQ_EXPORT        __attribute__((visibility("default")))
#   define TQ_CALL
#   define TQ_NO_RET        __attribute__((__noreturn__))
#   define TQ_DEPREC        __attribute__((deprecated))
#elif defined(_MSC_VER)
#   if defined(TQ_BUILD)
#       define TQ_EXPORT    __declspec(dllexport)
#   else
#       define TQ_EXPORT    __declspec(dllimport)
#   endif
#   define TQ_CALL          __cdecl
#   define TQ_NO_RET        __declspec(noreturn)
#   define TQ_DEPREC        __declspec(deprecated)
#else
#   define TQ_ENTRY
#   define TQ_CALL
#   define TQ_NO_RET
#   define TQ_DEPREC
#endif

#ifdef TQ_SHARED
    #define TQ_API TQ_LINKAGE TQ_EXPORT
#else
    #define TQ_API TQ_LINKAGE
#endif

/**
 * C++ lacks C99's compound literals. However, since C++11, plain structures
 * can be initialized with slightly different syntax:
 * `xyz_person { "John", "Doe" }`
 *
 * The same thing, but without parentheses. Sadly, this doesn't support
 * designated initializers.
 */

#ifdef __cplusplus
    #define TQ_COMPOUND(Type)       Type
#else
    #define TQ_COMPOUND(Type)       (Type)
#endif

//------------------------------------------------------------------------------
// Constants

/**
 * π number.
 */
#define TQ_PI                           (3.14159265358979323846)

/**
 * Maximum number of open music tracks.
 */
#define TQ_MUSIC_LIMIT                  (32)

/**
 * Maximum number of sounds.
 */
#define TQ_SOUND_LIMIT                  (256)

/**
 * Maximum number of playing audio channels.
 */
#define TQ_CHANNEL_LIMIT                (16)

/**
 * Regular font weight.
 */
#define TQ_FONT_NORMAL                  (400)

/**
 * Bold font weight.
 */
#define TQ_FONT_BOLD                    (700)

//------------------------------------------------------------------------------
// Enumerations

/**
 * Enumeration of keyboard keys.
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
} tq_key;

/**
 * Enumeration of mouse buttons.
 */
typedef enum tq_mouse_button
{
    TQ_MOUSE_BUTTON_LEFT,
    TQ_MOUSE_BUTTON_RIGHT,
    TQ_MOUSE_BUTTON_MIDDLE,
    TQ_MOUSE_BUTTON_X4,
    TQ_MOUSE_BUTTON_X5,
    TQ_TOTAL_MOUSE_BUTTONS,
} tq_mouse_button;

/**
 * Enumeration of joystick axes.
 */
typedef enum tq_joystick_axis
{
    TQ_TOTAL_JOYSTICK_AXES,
} tq_joystick_axis;

/**
 * Enumeration of joystick buttons.
 */
typedef enum tq_joystick_button
{
    TQ_TOTAL_JOYSTICK_BUTTONS,
} tq_joystick_button;

/**
 * Enumeration of blend mode factors.
 */
typedef enum tq_blend_factor
{
    TQ_BLEND_ZERO,
    TQ_BLEND_ONE,
    TQ_BLEND_SRC_COLOR,
    TQ_BLEND_ONE_MINUS_SRC_COLOR,
    TQ_BLEND_DST_COLOR,
    TQ_BLEND_ONE_MINUS_DST_COLOR,
    TQ_BLEND_SRC_ALPHA,
    TQ_BLEND_ONE_MINUS_SRC_ALPHA,
    TQ_BLEND_DST_ALPHA,
    TQ_BLEND_ONE_MINUS_DST_ALPHA,
} tq_blend_factor;

/**
 * Enumeration of blend mode equations.
 */
typedef enum tq_blend_equation
{
    TQ_BLEND_ADD,
    TQ_BLEND_SUB,
    TQ_BLEND_REV_SUB,
} tq_blend_equation;

/**
 * Enumeration of audio channel states.
 */
typedef enum tq_channel_state
{
    TQ_CHANNEL_INACTIVE,
    TQ_CHANNEL_PAUSED,
    TQ_CHANNEL_PLAYING,
} tq_channel_state;

//------------------------------------------------------------------------------
// Typedefs and structs

/**
 * Texture identifier.
 */
typedef struct { int id; } tq_texture;

/**
 * Surface identifier.
 */
typedef struct { int id; } tq_surface;

/**
 * Font identifier.
 */
typedef struct { int id; } tq_font;

/**
 * Sound identifier.
 */
typedef struct { int id; } tq_sound;

/**
 * Music identifier.
 */
typedef struct { int id; } tq_music;

/**
 * Channel identifier.
 */
typedef struct { int id; } tq_channel;

/**
 * Two-dimensional vector of integer numbers.
 */
typedef struct tq_vec2i
{
    int x;
    int y;
} tq_vec2i;

/**
 * Two-dimensional vector of floating-point numbers.
 */
typedef struct tq_vec2f
{
    float x;
    float y;
} tq_vec2f;

/**
 * Rectangle (integer).
 */
typedef struct tq_recti
{
    int x;
    int y;
    int w;
    int h;
} tq_recti;

/**
 * Rectangle (floating point).
 */
typedef struct tq_rectf
{
    float x;
    float y;
    float w;
    float h;
} tq_rectf;

/**
 * 32 bit color value.
 */
typedef struct tq_color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} tq_color;

/**
 * Blend mode structure.
 */
typedef struct tq_blend_mode
{
    tq_blend_factor color_src_factor;
    tq_blend_factor color_dst_factor;
    tq_blend_equation color_equation;

    tq_blend_factor alpha_src_factor;
    tq_blend_factor alpha_dst_factor;
    tq_blend_equation alpha_equation;
} tq_blend_mode;

/**
 * Keyboard event callback.
 */
typedef void (*tq_key_callback)(tq_key key);

/**
 * Mouse button event callback.
 */
typedef void (*tq_mouse_button_callback)(tq_vec2i cursor, tq_mouse_button mouse_button);

/**
 * Mouse cursor event callback.
 */
typedef void (*tq_mouse_cursor_callback)(tq_vec2i cursor);

/**
 * Mouse wheel event callback.
 */
typedef void (*tq_mouse_wheel_callback)(tq_vec2i cursor, tq_vec2f wheel);

//------------------------------------------------------------------------------

/**
 * Initialize the library.
 */
TQ_API void TQ_CALL tq_initialize(void);

/**
 * Terminate the library and release all resources.
 */
TQ_API void TQ_CALL tq_terminate(void);

/**
 * Process user input and do other important things.
 */
TQ_API bool TQ_CALL tq_process(void);

//------------------------------------------------------------------------------
// Core

//----------------------------------------------------------
// Display

/**
 * Get display (or window) size.
 */
TQ_API tq_vec2i TQ_CALL tq_get_display_size(void);

/**
 * Changes or sets display (window) size. Can be called before initialization.
 */
TQ_API void TQ_CALL tq_set_display_size(tq_vec2i size);

/**
 * Get current application title.
 */
TQ_API char const * TQ_CALL tq_get_title(void);

/**
 * Changes or sets the application title. Can be called before initialization.
 */
TQ_API void TQ_CALL tq_set_title(char const *title);

//----------------------------------------------------------
// Keyboard

/**
 * Check if keyboard auto-repeat feature is enabled.
 */
TQ_API bool TQ_CALL tq_is_key_autorepeat_enabled(void);

/**
 * Set keyboard auto-repeat feature. If enabled, your key-press callback
 * will be called repeatedly when a key is hold. This feature is enabled by
 * default.
 */
TQ_API void TQ_CALL tq_set_key_autorepeat_enabled(bool enabled);

/**
 * Check if a key is pressed.
 */
TQ_API bool TQ_CALL tq_is_key_pressed(tq_key key);

/**
 * Set callback function that will be called when a key is pressed.
 */
TQ_API void TQ_CALL tq_on_key_pressed(tq_key_callback callback);

/**
 * Set callback function that will be called when a key is released.
 */
TQ_API void TQ_CALL tq_on_key_released(tq_key_callback callback);

//----------------------------------------------------------
// Mouse

/**
 * Check if mouse cursor is hidden.
 */
TQ_API bool TQ_CALL tq_is_mouse_cursor_hidden(void);

/**
 * Use this option to hide mouse cursor.
 * It will be invisible only if it's inside the window boundaries,
 * otherwise it can be visible.
 */
TQ_API void TQ_CALL tq_set_mouse_cursor_hidden(bool hidden);

/**
 * Check if a mouse button is pressed.
 */
TQ_API bool TQ_CALL tq_is_mouse_button_pressed(tq_mouse_button mouse_button);

/**
 * Get mouse cursor position.
 */
TQ_API tq_vec2i TQ_CALL tq_get_mouse_cursor_position(void);

/**
 * Set callback function that will be called when a mouse button is pressed.
 */
TQ_API void TQ_CALL tq_on_mouse_button_pressed(tq_mouse_button_callback callback);

/**
 * Set callback function that will be called when a mouse button is released.
 */
TQ_API void TQ_CALL tq_on_mouse_button_released(tq_mouse_button_callback callback);

/**
 * Set callback function that will be called when a mouse cursor is moved.
 */
TQ_API void TQ_CALL tq_on_mouse_cursor_moved(tq_mouse_cursor_callback callback);

/**
 * Register a function that will be called when a mouse wheel is scrolled.
 */
TQ_API void TQ_CALL tq_on_mouse_wheel_scrolled(tq_mouse_wheel_callback callback);

//----------------------------------------------------------
// Time and other

/**
 * Get amount of seconds passed since the library initialization (millisecond precision).
 */
TQ_API float TQ_CALL tq_get_time_mediump(void);

/**
 * Get amount of seconds passed since the library initialization (nanosecond precision).
 */
TQ_API double TQ_CALL tq_get_time_highp(void);

/**
 * Get delta time between current and previous frames.
 */
TQ_API double TQ_CALL tq_get_delta_time(void);

/**
 * Get the current framerate in frames per second.
 */
TQ_API int TQ_CALL tq_get_framerate(void);

//------------------------------------------------------------------------------
// Graphics

//----------------------------------------------------------
// Colors

/**
 * Construct color struct from RGB values.
 */
static inline tq_color tq_c24(unsigned char r, unsigned char g, unsigned char b)
{
    return (tq_color) {
        .r = r,
        .g = g,
        .b = b,
        .a = 255,
    };
}

/**
 * Construct color struct from RGBA values.
 */
static inline tq_color tq_c32(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    return (tq_color) {
        .r = r,
        .g = g,
        .b = b,
        .a = a,
    };
}

//----------------------------------------------------------
// Options

/**
 * Set color key value. Pass {0, 0, 0, 0} to disable this.
 * If color keying is enabled, all image loading operations
 * will convert that color (alpha ignored) to fully transparent value.
 * For example, if you set this value to {255, 0, 0, 255}, then all
 * red pixels will be converted to transparent pixels.
 * Color keying is disabled by default.
 * It's advised not to use this function and use proper textures instead.
 */
TQ_API void TQ_CALL tq_set_color_key(tq_color color);

/**
 * Set anti-aliasing level. It's applied to both canvas and surfaces.
 * If the level isn't supported, no changes are made.
 * Possible values are 0, 1, 2, 4, 8 and 16. Level 0 is equal to 1.
 * Default level is 0.
 * It's safe to call this function before tq_initialize().
 */
TQ_API void TQ_CALL tq_set_antialiasing_level(int level);

//----------------------------------------------------------
// Canvas

/**
 * Clear the canvas with background color.
 */
TQ_API void TQ_CALL tq_clear(void);

/**
 * Get background color.
 */
TQ_API tq_color TQ_CALL tq_get_clear_color(void);

/**
 * Set background color.
 */
TQ_API void TQ_CALL tq_set_clear_color(tq_color clear_color);

/**
 * Get canvas size.
 */
TQ_API tq_vec2i TQ_CALL tq_get_canvas_size(void);

/**
 * Resize the canvas. By default, canvas is initialized with
 * display size.
 */
TQ_API void TQ_CALL tq_set_canvas_size(tq_vec2i size);

/**
 * Check if canvas is antialiased.
 */
TQ_API bool TQ_CALL tq_is_canvas_smooth(void);

/**
 * This option sets if canvas is antialiased when resized.
 * Default value: enabled.
 */
TQ_API void TQ_CALL tq_set_canvas_smooth(bool smooth);

//----------------------------------------------------------
// Views

/**
 * Convert a point in display coordinates to current view coordinates.
 * For example, this can be used to convert coordinates
 * from `tq_get_mouse_cursor_position()`.
 */
TQ_API tq_vec2f TQ_CALL tq_get_relative_position(tq_vec2f absolute);

/**
 * Set the view rectangle and rotation.
 */
TQ_API void TQ_CALL tq_set_view(tq_rectf rect, float rotation);

/**
 * Reset the view to its default state.
 * Default view is an area from top-left corner of the screen to
 * its bottom-right corner.
 */
TQ_API void TQ_CALL tq_reset_view(void);

//----------------------------------------------------------
// Transformation matrix

/**
 * Duplicate current transformation matrix and push it to the matrix stack.
 */
TQ_API void TQ_CALL tq_push_matrix(void);

/**
 * Replace current transformation matrix by the first matrix in the stack, popping it from there.
 */
TQ_API void TQ_CALL tq_pop_matrix(void);

/**
 * Translate current transformation matrix.
 */
TQ_API void TQ_CALL tq_translate_matrix(tq_vec2f translate);

/**
 * Scale current transformation matrix.
 */
TQ_API void TQ_CALL tq_scale_matrix(tq_vec2f scale);

/**
 * Rotate current transformation matrix.
 */
TQ_API void TQ_CALL tq_rotate_matrix(float degrees);

//----------------------------------------------------------
// Primitives

/**
 * Draw a point.
 */
TQ_API void TQ_CALL tq_draw_point(tq_vec2f position);

/**
 * Draw a line.
 */
TQ_API void TQ_CALL tq_draw_line(tq_vec2f a, tq_vec2f b);

/**
 * Fill and outline a triangle.
 */
TQ_API void TQ_CALL tq_draw_triangle(tq_vec2f a, tq_vec2f b, tq_vec2f c);

/**
 * Fill and outline a rectangle.
 */
TQ_API void TQ_CALL tq_draw_rectangle(tq_rectf rect);

/**
 * Fill and outline a circle.
 */
TQ_API void TQ_CALL tq_draw_circle(tq_vec2f position, float radius);

/**
 * Outline a triangle.
 */
TQ_API void TQ_CALL tq_outline_triangle(tq_vec2f a, tq_vec2f b, tq_vec2f c);

/**
 * Outline a rectangle.
 */
TQ_API void TQ_CALL tq_outline_rectangle(tq_rectf rect);

/**
 * Outline a circle.
 */
TQ_API void TQ_CALL tq_outline_circle(tq_vec2f position, float radius);

/**
 * Fill a triangle.
 */
TQ_API void TQ_CALL tq_fill_triangle(tq_vec2f a, tq_vec2f b, tq_vec2f c);

/**
 * Fill a rectangle.
 */
TQ_API void TQ_CALL tq_fill_rectangle(tq_rectf rect);

/**
 * Fill a circle.
 */
TQ_API void TQ_CALL tq_fill_circle(tq_vec2f position, float radius);

/**
 * Get draw color.
 */
TQ_API tq_color TQ_CALL tq_get_draw_color(void);

/**
 * Set draw color.
 * This color is used by primitive drawing functions.
 */
TQ_API void TQ_CALL tq_set_draw_color(tq_color draw_color);

/**
 * Get outline color.
 */
TQ_API tq_color TQ_CALL tq_get_outline_color(void);

/**
 * Set outline color.
 * This color is used for outlining by primitive drawing functions.
 */
TQ_API void TQ_CALL tq_set_outline_color(tq_color outline_color);

//----------------------------------------------------------
// Textures

/**
 * Load texture from a file.
 */
TQ_API tq_texture TQ_CALL tq_load_texture_from_file(char const *path);

/**
 * Load texture from a memory buffer.
 */
TQ_API tq_texture TQ_CALL tq_load_texture_from_memory(uint8_t const *buffer, size_t length);

/**
 * Delete a texture from video memory.
 */
TQ_API void TQ_CALL tq_delete_texture(tq_texture texture);

/**
 * Get size of a texture.
 */
TQ_API tq_vec2i TQ_CALL tq_get_texture_size(tq_texture texture);

/**
 * If enabled, this option will make texture look blurry when magnified.
 * Default value: disabled.
 */
TQ_API void TQ_CALL tq_set_texture_smooth(tq_texture texture, bool smooth);

/**
 * Draw a texture inside a rectangle.
 */
TQ_API void TQ_CALL tq_draw_texture(tq_texture texture, tq_rectf rect);

/**
 * Draw a part of a texture inside a rectangle.
 * Texture coordinates should be in pixel space.
 */
TQ_API void TQ_CALL tq_draw_subtexture(tq_texture texture, tq_rectf sub, tq_rectf rect);

//----------------------------------------------------------
// Surfaces

/**
 * Create a new surface with given size.
 */
TQ_API tq_surface TQ_CALL tq_create_surface(tq_vec2i size);

/**
 * Delete a surface.
 */
TQ_API void TQ_CALL tq_delete_surface(tq_surface surface);

/**
 * Switch to surface. All subsequent rendering commands will affect
 * that surface.
 * The current surface is reset every frame.
 */
TQ_API void TQ_CALL tq_set_surface(tq_surface surface);

/**
 * Switch back to the canvas.
 */
TQ_API void TQ_CALL tq_reset_surface(void);

/**
 * Get the underlying texture of a surface.
 */
TQ_API tq_texture TQ_CALL tq_get_surface_texture(tq_surface surface);

//----------------------------------------------------------
// Fonts and text

/**
 * Load font from file.
 */
TQ_API tq_font TQ_CALL tq_load_font_from_file(char const *path, float pt, int weight);

/**
 * Load font from memory buffer.
 */
TQ_API tq_font TQ_CALL tq_load_font_from_memory(uint8_t const *buffer, size_t size, float pt, int weight);

/**
 * Delete previously loaded font.
 */
TQ_API void TQ_CALL tq_delete_font(tq_font font);

/**
 * Access underlying texture atlas of a font.
 */
TQ_API tq_texture TQ_CALL tq_get_font_texture(tq_font font);

/**
 * Draw simple text using specified font.
 */
TQ_API void TQ_CALL tq_draw_text(tq_font font, tq_vec2f position, char const *text);

/**
 * Draw formatted text using specified font.
 * See also: tq_set_fill_color(), tq_set_outline_color().
 */
TQ_API void TQ_CALL tq_print_text(tq_font font, tq_vec2f position, char const *fmt, ...);

//----------------------------------------------------------
// Blending

/**
 * Shortcut macro to quickly define custom blend mode.
 */
#define TQ_DEFINE_BLEND_MODE(SrcFactor, DstFactor) \
    TQ_COMPOUND(tq_blend_mode) { \
        (SrcFactor), (DstFactor), TQ_BLEND_ADD, \
        (SrcFactor), (DstFactor), TQ_BLEND_ADD }

/**
 * Shortcut macro to quickly define custom blend mode with custom equation.
 */
#define TQ_DEFINE_BLEND_MODE_EX(SrcFactor, DstFactor, Equation) \
    TQ_COMPOUND(tq_blend_mode) { \
        (SrcFactor), (DstFactor), (Equation), \
        (SrcFactor), (DstFactor), (Equation) }

/**
 * Predefined blending mode: none.
 * No blending is done, dst is overwritten by src.
 */
#define TQ_BLEND_MODE_NONE \
    TQ_COMPOUND(tq_blend_mode) { \
        TQ_BLEND_ONE, TQ_BLEND_ZERO, TQ_BLEND_ADD, \
        TQ_BLEND_ONE, TQ_BLEND_ZERO, TQ_BLEND_ADD }

/**
 * Predefined blending mode: alpha.
 */
#define TQ_BLEND_MODE_ALPHA \
    TQ_COMPOUND(tq_blend_mode) { \
        TQ_BLEND_SRC_ALPHA, TQ_BLEND_ONE_MINUS_SRC_ALPHA, TQ_BLEND_ADD, \
        TQ_BLEND_SRC_ALPHA, TQ_BLEND_ONE_MINUS_SRC_ALPHA, TQ_BLEND_ADD }

/**
 * Predefined blending mode: add.
 */
#define TQ_BLEND_MODE_ADD \
    TQ_COMPOUND(tq_blend_mode) { \
        TQ_BLEND_SRC_ALPHA, TQ_BLEND_ONE, TQ_BLEND_ADD, \
        TQ_BLEND_SRC_ALPHA, TQ_BLEND_ONE, TQ_BLEND_ADD }

/**
 * Predefined blending mode: multiply.
 */
#define TQ_BLEND_MODE_MUL \
    TQ_COMPOUND(tq_blend_mode) { \
        TQ_BLEND_ZERO, TQ_BLEND_SRC_COLOR, TQ_BLEND_ADD, \
        TQ_BLEND_ZERO, TQ_BLEND_SRC_ALPHA, TQ_BLEND_ADD }

/**
 * Set current blend mode. The default one is alpha blending.
 */
TQ_API void TQ_CALL tq_set_blend_mode(tq_blend_mode mode);

//------------------------------------------------------------------------------
// Audio

/**
 * Set master volume.
 */
TQ_API void TQ_CALL tq_set_master_volume(float volume);

/**
 * Decode and load sound to memory from a file.
 */
TQ_API tq_sound TQ_CALL tq_load_sound_from_file(char const *path);

/**
 * Decode sound from a memory buffer.
 */
TQ_API tq_sound TQ_CALL tq_load_sound_from_memory(void const *buffer, size_t size);

/**
 * Delete sound from memory.
 */
TQ_API void TQ_CALL tq_delete_sound(tq_sound sound);

/**
 * Play sound.
 */
TQ_API tq_channel TQ_CALL tq_play_sound(tq_sound sound, int loop);

/**
 * Open music stream from a file.
 */
TQ_API tq_music TQ_CALL tq_open_music_from_file(char const *path);

/**
 * Open music stream from memory buffer.
 */
TQ_API tq_music TQ_CALL tq_open_music_from_memory(void const *buffer, size_t size);

/**
 * Close music stream.
 */
TQ_API void TQ_CALL tq_close_music(tq_music music);

/**
 * Choose any unused channel and play music on it.
 */
TQ_API tq_channel TQ_CALL tq_play_music(tq_music music, int loop);

/**
 * Get current state of the audio channel.
 */
TQ_API tq_channel_state TQ_CALL tq_get_channel_state(tq_channel channel);

/**
 * Pause the audio channel.
 */
TQ_API void TQ_CALL tq_pause_channel(tq_channel channel);

/**
 * Continue playback of the audio channel.
 */
TQ_API void TQ_CALL tq_unpause_channel(tq_channel channel);

/**
 * Stop the audio channel and mark it inactive.
 */
TQ_API void TQ_CALL tq_stop_channel(tq_channel channel);

//------------------------------------------------------------------------------
// Math

/**
 * Get smallest value.
 */
#define TQ_MIN(a, b)                (((a) < (b)) ? (a) : (b))

/**
 * Get largest value.
 */
#define TQ_MAX(a, b)                (((a) > (b)) ? (a) : (b))

/**
 * Converts degrees to radians.
 */
#define TQ_DEG2RAD(deg)             ((deg) * (TQ_PI / 180.0))

/**
 * Converts radians to degrees.
 */
#define TQ_RAD2DEG(rad)             ((rad) * (180.0 / TQ_PI))

/**
 * Cast 2-component integer vector to floating-point vector.
 */
static inline tq_vec2f tq_vec2i_cast(tq_vec2i ivec)
{
    return (tq_vec2f) {
        .x = (float) ivec.x,
        .y = (float) ivec.y,
    };
}

/**
 * Add two floating-point 2D vectors.
 */
static inline tq_vec2f tq_vec2f_add(tq_vec2f v0, tq_vec2f v1)
{
    return (tq_vec2f) {
        .x = v0.x + v1.x,
        .y = v0.y + v1.y,
    };
}

/**
 * Subtract two floating-point 2D vectors.
 */
static inline tq_vec2f tq_vec2f_subtract(tq_vec2f v0, tq_vec2f v1)
{
    return (tq_vec2f) {
        .x = v0.x - v1.x,
        .y = v0.y - v1.y,
    };
}

/**
 * Multiply two floating-point 2D vectors.
 */
static inline tq_vec2f tq_vec2f_multiply(tq_vec2f v0, tq_vec2f v1)
{
    return (tq_vec2f) {
        .x = v0.x * v1.x,
        .y = v0.y * v1.y,
    };
}

/**
 * Scale a floating-point 2D vector.
 */
static inline tq_vec2f tq_vec2f_scale(tq_vec2f vec, float f)
{
    return (tq_vec2f) {
        .x = vec.x * f,
        .y = vec.y * f,
    };
}

/**
 * Get length of a floating-point 2D vector.
 */
static inline float tq_vec2f_length(tq_vec2f vec)
{
    return sqrtf((vec.x * vec.x) + (vec.y * vec.y));
}

/**
 * Normalize a floating-point 2D vector.
 */
static inline tq_vec2f tq_vec2f_normalize(tq_vec2f vec)
{
    float const length = tq_vec2f_length(vec);

    return (tq_vec2f) {
        .x = vec.x / length,
        .y = vec.y / length,
    };
}

/**
 * Calculate the distance between two vectors.
 */
static inline float tq_vec2f_distance(tq_vec2f v0, tq_vec2f v1)
{
    return sqrtf(((v1.y - v0.y) * (v1.y - v0.y)) + ((v1.x - v0.x) * (v1.x - v0.x)));
}

/**
 * Calculate an angle needed to aim to the target.
 */
static inline float tq_vec2f_look_at(tq_vec2f observer, tq_vec2f target)
{
    float const u = target.x - observer.x;
    float const v = target.y - observer.y;

    return TQ_RAD2DEG(atan2f(v, u));
}

//------------------------------------------------------------------------------

#endif // TQ_PUBLIC_H_INC

//------------------------------------------------------------------------------
