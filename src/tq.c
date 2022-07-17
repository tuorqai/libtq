
//------------------------------------------------------------------------------
// tq library :3
//------------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "tq_audio.h"
#include "tq_core.h"
#include "tq_graphics.h"
#include "tq_mem.h"
#include "tq_text.h"

//------------------------------------------------------------------------------

void tq_initialize(void)
{
    tq_core_initialize();
    tq_graphics_initialize();
    tq_audio_initialize();
}

void tq_terminate(void)
{
    tq_audio_terminate();
    tq_graphics_terminate();
    tq_core_terminate();
}

bool tq_process(void)
{
    tq_graphics_process();
    tq_audio_process();

    return tq_core_process();
}

//------------------------------------------------------------------------------
// Core

//----------------------------------------------------------
// Display

uint32_t tq_get_display_width(void)
{
    return tq_core_get_display_width();
}

uint32_t tq_get_display_height(void)
{
    return tq_core_get_display_height();
}

void tq_get_display_size(uint32_t *width, uint32_t *height)
{
    tq_core_get_display_size(width, height);
}

void tq_set_display_size(uint32_t width, uint32_t height)
{
    tq_core_set_display_size(width, height);
}

char const *tq_get_title(void)
{
    return tq_core_get_title();
}

void tq_set_title(char const *title)
{
    tq_core_set_title(title);
}

//----------------------------------------------------------
// Keyboard

bool tq_is_key_autorepeat_enabled(void)
{
    return tq_core_is_key_autorepeat_enabled();
}

void tq_set_key_autorepeat_enabled(bool enabled)
{
    tq_core_set_key_autorepeat_enabled(enabled);
}

bool tq_is_key_pressed(tq_key_t key)
{
    return tq_core_is_key_pressed(key);
}

bool tq_is_mouse_button_pressed(tq_mouse_button_t mouse_button)
{
    return tq_core_is_mouse_button_pressed(mouse_button);
}

void tq_on_key_pressed(tq_key_callback_t callback)
{
    tq_core_set_key_press_callback(callback);
}

void tq_on_key_released(tq_key_callback_t callback)
{
    tq_core_set_key_release_callback(callback);
}

//----------------------------------------------------------
// Mouse

int32_t tq_get_mouse_cursor_x(void)
{
    return tq_core_get_mouse_cursor_x();
}

int32_t tq_get_mouse_cursor_y(void)
{
    return tq_core_get_mouse_cursor_y();
}

void tq_get_mouse_cursor_position(int32_t *x, int32_t *y)
{
    *x = tq_core_get_mouse_cursor_x();
    *y = tq_core_get_mouse_cursor_y();
}

void tq_on_mouse_button_pressed(tq_mouse_button_callback_t callback)
{
    tq_core_set_mouse_button_press_callback(callback);
}

void tq_on_mouse_button_released(tq_mouse_button_callback_t callback)
{
    tq_core_set_mouse_button_release_callback(callback);
}

void tq_on_mouse_cursor_moved(tq_mouse_cursor_callback_t callback)
{
    tq_core_set_mouse_cursor_move_callback(callback);
}

void tq_on_mouse_wheel_scrolled(tq_mouse_wheel_callback_t callback)
{
    tq_core_set_mouse_wheel_scroll_callback(callback);
}

//----------------------------------------------------------
// Time

float tq_get_time_mediump(void)
{
    return tq_core_get_time_mediump();
}

double tq_get_time_highp(void)
{
    return tq_core_get_time_highp();
}

double tq_get_delta_time(void)
{
    return tq_core_get_delta_time();
}

//----------------------------------------------------------
// Stats

unsigned int tq_get_framerate(void)
{
    return tq_core_get_framerate();
}

//------------------------------------------------------------------------------
// Graphics

void tq_clear(void)
{
    graphics_clear();
}

tq_color_t tq_get_clear_color(void)
{
    return graphics_get_clear_color();
}

void tq_set_clear_color(tq_color_t clear_color)
{
    graphics_set_clear_color(clear_color);
}

void tq_view(float x, float y, float width, float height, float angle)
{
    tq_graphics_view(x, y, width, height, angle);
}

void tq_view_v(tq_vec2f_t position, tq_vec2f_t size, float angle)
{
    tq_graphics_view(position.x, position.y, size.x, size.y, angle);
}

//----------------------------------------------------------
// Matrices

void tq_push_matrix(void)
{
    graphics_push_matrix();
}

void tq_pop_matrix(void)
{
    graphics_pop_matrix();
}

void tq_translate_matrix_f(float x, float y)
{
    graphics_translate_matrix(x, y);
}

void tq_translate_matrix_v(tq_vec2f_t v)
{
    graphics_translate_matrix(v.x, v.y);
}

void tq_scale_matrix_f(float x, float y)
{
    graphics_scale_matrix(x, y);
}

void tq_scale_matrix_v(tq_vec2f_t v)
{
    graphics_scale_matrix(v.x, v.y);
}

void tq_rotate_matrix(float degrees)
{
    graphics_rotate_matrix(degrees);
}

//----------------------------------------------------------
// Primitives

void tq_draw_point_f(float x, float y)
{
    graphics_draw_point(x, y);
}

void tq_draw_point_v(tq_vec2f_t position)
{
    graphics_draw_point(position.x, position.y);
}

void tq_draw_line_f(float ax, float ay, float bx, float by)
{
    graphics_draw_line(ax, ay, bx, by);
}

void tq_draw_line_v(tq_vec2f_t a, tq_vec2f_t b)
{
    graphics_draw_line(a.x, a.y, b.x, b.y);
}

void tq_draw_triangle_f(float ax, float ay, float bx, float by, float cx, float cy)
{
    graphics_draw_triangle(ax, ay, bx, by, cx, cy);
}

void tq_draw_triangle_v(tq_vec2f_t a, tq_vec2f_t b, tq_vec2f_t c)
{
    graphics_draw_triangle(a.x, a.y, b.x, b.y, c.x, c.y);
}

void tq_draw_rectangle_f(float x, float y, float w, float h)
{
    graphics_draw_rectangle(x, y, w, h);
}

void tq_draw_rectangle_v(tq_vec2f_t position, tq_vec2f_t size)
{
    graphics_draw_rectangle(position.x, position.y, size.x, size.y);
}

void tq_draw_circle_f(float x, float y, float radius)
{
    graphics_draw_circle(x, y, radius);
}

void tq_draw_circle_v(tq_vec2f_t position, float radius)
{
    graphics_draw_circle(position.x, position.y, radius);
}

void tq_outline_triangle_f(float ax, float ay, float bx, float by, float cx, float cy)
{
    graphics_outline_triangle(ax, ay, bx, by, cx, cy);
}

void tq_outline_triangle_v(tq_vec2f_t a, tq_vec2f_t b, tq_vec2f_t c)
{
    graphics_outline_triangle(a.x, a.y, b.x, b.y, c.x, c.y);
}

void tq_outline_rectangle_f(float x, float y, float w, float h)
{
    graphics_outline_rectangle(x, y, w, h);
}

void tq_outline_rectangle_v(tq_vec2f_t position, tq_vec2f_t size)
{
    graphics_outline_rectangle(position.x, position.y, size.x, size.y);
}

void tq_outline_circle_f(float x, float y, float radius)
{
    graphics_outline_circle(x, y, radius);
}

void tq_outline_circle_v(tq_vec2f_t position, float radius)
{
    graphics_outline_circle(position.x, position.y, radius);
}

void tq_fill_triangle_f(float ax, float ay, float bx, float by, float cx, float cy)
{
    graphics_fill_triangle(ax, ay, bx, by, cx, cy);
}

void tq_fill_triangle_v(tq_vec2f_t a, tq_vec2f_t b, tq_vec2f_t c)
{
    graphics_fill_triangle(a.x, a.y, b.x, b.y, c.x, c.y);
}

void tq_fill_rectangle_f(float x, float y, float w, float h)
{
    graphics_fill_rectangle(x, y, w, h);
}

void tq_fill_rectangle_v(tq_vec2f_t position, tq_vec2f_t size)
{
    graphics_fill_rectangle(position.x, position.y, size.x, size.y);
}

void tq_fill_circle_f(float x, float y, float radius)
{
    graphics_fill_circle(x, y, radius);
}

void tq_fill_circle_v(tq_vec2f_t position, float radius)
{
    graphics_fill_circle(position.x, position.y, radius);
}

tq_color_t tq_get_point_color(void)
{
    return graphics_get_point_color();
}

void tq_set_point_color(tq_color_t point_color)
{
    graphics_set_point_color(point_color);
}

tq_color_t tq_get_line_color(void)
{
    return graphics_get_line_color();
}

void tq_set_line_color(tq_color_t line_color)
{
    graphics_set_line_color(line_color);
}

tq_color_t tq_get_outline_color(void)
{
    return graphics_get_outline_color();
}

void tq_set_outline_color(tq_color_t outline_color)
{
    graphics_set_outline_color(outline_color);
}

tq_color_t tq_get_fill_color(void)
{
    return graphics_get_fill_color();
}

void tq_set_fill_color(tq_color_t fill_color)
{
    graphics_set_fill_color(fill_color);
}

//----------------------------------------------------------
// Textures

tq_texture tq_load_texture_from_file(char const *path)
{
    return (tq_texture) { .id = graphics_load_texture_from_file(path) };
}

tq_texture tq_load_texture_from_memory(uint8_t const *buffer, size_t length)
{
    return (tq_texture) { .id = graphics_load_texture_from_memory(buffer, length) };
}

void tq_delete_texture(tq_texture texture)
{
    graphics_delete_texture(texture.id);
}

uint32_t tq_get_texture_width(tq_texture texture)
{
    uint32_t width, height;
    graphics_get_texture_size(texture.id, &width, &height);

    return width;
}

uint32_t tq_get_texture_height(tq_texture texture)
{
    uint32_t width, height;
    graphics_get_texture_size(texture.id, &width, &height);
    
    return height;
}

void tq_get_texture_size(tq_texture texture, uint32_t *width, uint32_t *height)
{
    graphics_get_texture_size(texture.id, width, height);
}

void tq_draw_texture_f(tq_texture texture, float x, float y, float w, float h)
{
    graphics_draw_texture(texture.id, x, y, w, h);
}

void tq_draw_texture_v(tq_texture texture, tq_vec2f_t position, tq_vec2f_t size)
{
    graphics_draw_texture(texture.id, position.x, position.y, size.x, size.y);
}

void tq_draw_texture_fragment_f(tq_texture texture,
    float x, float y,
    float w, float h,
    float fx, float fy,
    float fw, float fh)
{
    graphics_draw_texture_fragment(texture.id, x, y, w, h, fx, fy, fw, fh);
}

void tq_draw_texture_fragment_v(tq_texture texture,
    tq_vec2f_t position,
    tq_vec2f_t size,
    tq_vec2f_t fragment_position,
    tq_vec2f_t fragment_size)
{
    graphics_draw_texture_fragment(texture.id,
        position.x, position.y,
        size.x, size.y,
        fragment_position.x, fragment_position.y,
        fragment_size.x, fragment_size.y);
}

//----------------------------------------------------------
// Fonts & text

tq_font tq_load_font_from_file(char const *path, float pt, int weight)
{
    return (tq_font) { .id = text_load_font_from_file(path, pt, weight) };
}

tq_font tq_load_font_from_memory(uint8_t const *buffer, size_t size, float pt, int weight)
{
    return (tq_font) { .id = text_load_font_from_memory(buffer, size, pt, weight) };
}

void tq_delete_font(tq_font font)
{
    text_delete_font(font.id);
}

void tq_draw_text(tq_font font, tq_vec2f_t position, char const *text)
{
    text_draw_text(font.id, position.x, position.y, text);
}

void tq_print_text(tq_font font, tq_vec2f_t position, char const *fmt, ...)
{
    static int buffer_size = 0;
    static char *buffer = NULL;

    va_list ap;

    va_start(ap, fmt);
    {
        int bytes_required = vsnprintf(buffer, buffer_size, fmt, ap);

        if (bytes_required >= buffer_size) {
            if (buffer_size == 0) {
                buffer_size = 64;
            }

            while (buffer_size < (bytes_required + 1)) {
                buffer_size *= 2;
            }

            buffer = mem_realloc(buffer, buffer_size);
            vsnprintf(buffer, buffer_size, fmt, ap);
        }
    }
    va_end(ap);

    text_draw_text(font.id, position.x, position.y, buffer);
}

//------------------------------------------------------------------------------
// Audio

//----------------------------------------------------------
// Sounds

tq_sound tq_load_sound_from_file(char const *path)
{
    return (tq_sound) { .id = tq_audio_load_sound_from_file(path) };
}

tq_sound tq_load_sound_from_memory(uint8_t const *buffer, size_t length)
{
    return (tq_sound) { .id = tq_audio_load_sound_from_memory(buffer, length) };
}

void tq_delete_sound(tq_sound sound)
{
    tq_audio_delete_sound(sound.id);
}

tq_channel tq_play_sound(tq_sound sound, int loop)
{
    return (tq_channel) { .id = tq_audio_play_sound(sound.id, loop) };
}

//----------------------------------------------------------
// Music

tq_music tq_open_music_from_file(char const *path)
{
    return (tq_music) { .id = tq_audio_open_music_from_file(path) };
}

tq_music tq_open_music_from_memory(uint8_t const *buffer, size_t length)
{
    return (tq_music) { .id = tq_audio_open_music_from_memory(buffer, length) };
}

void tq_close_music(tq_music music)
{
    tq_audio_close_music(music.id);
}

tq_channel tq_play_music(tq_music music, int loop)
{
    return (tq_channel) { .id = tq_audio_play_music(music.id, loop) };
}

//----------------------------------------------------------
// Channels

tq_channel_state_t tq_get_channel_state(tq_channel channel)
{
    return tq_audio_get_channel_state(channel.id);
}

void tq_pause_channel(tq_channel channel)
{
    tq_audio_pause_channel(channel.id);
}

void tq_unpause_channel(tq_channel channel)
{
    tq_audio_unpause_channel(channel.id);
}

void tq_stop_channel(tq_channel channel)
{
    tq_audio_stop_channel(channel.id);
}

//------------------------------------------------------------------------------
