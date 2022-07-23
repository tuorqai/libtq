
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

tq_vec2i tq_get_display_size(void)
{
    tq_vec2i size;
    tq_core_get_display_size(&size.x, &size.y);

    return size;
}

void tq_set_display_size(tq_vec2i size)
{
    tq_core_set_display_size(size.x, size.y);
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

tq_vec2i tq_get_mouse_cursor_position(void)
{
    tq_vec2i position;
    tq_core_get_mouse_cursor_position(&position.x, &position.y);

    return position;
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

//----------------------------------------------------------
// Canvas

void tq_clear(void)
{
    graphics_clear();
}

tq_color tq_get_clear_color(void)
{
    return graphics_get_clear_color();
}

void tq_set_clear_color(tq_color clear_color)
{
    graphics_set_clear_color(clear_color);
}

tq_vec2i tq_get_canvas_size(void)
{
    tq_vec2i size;
    graphics_get_canvas_size(&size.x, &size.y);

    return size;
}

void tq_set_canvas_size(tq_vec2i size)
{
    graphics_set_canvas_size(size.x, size.y);
}

bool tq_is_canvas_smooth(void)
{
    return graphics_is_canvas_smooth();
}

void tq_set_canvas_smooth(bool smooth)
{
    graphics_set_canvas_smooth(smooth);
}

//----------------------------------------------------------
// View

tq_vec2f tq_get_relative_position(tq_vec2f absolute)
{
    tq_vec2f relative;
    graphics_get_relative_position(absolute.x, absolute.y, &relative.x, &relative.y);

    return relative;
}

void tq_set_view(tq_rectf rect, float rotation)
{
    graphics_set_view(rect.x, rect.y, rect.w, rect.h, rotation);
}

void tq_reset_view(void)
{
    graphics_reset_view();
}

//----------------------------------------------------------
// Transformation matrix stack

void tq_push_matrix(void)
{
    graphics_push_matrix();
}

void tq_pop_matrix(void)
{
    graphics_pop_matrix();
}

void tq_translate_matrix(tq_vec2f v)
{
    graphics_translate_matrix(v.x, v.y);
}

void tq_scale_matrix(tq_vec2f v)
{
    graphics_scale_matrix(v.x, v.y);
}

void tq_rotate_matrix(float degrees)
{
    graphics_rotate_matrix(degrees);
}

//----------------------------------------------------------
// Primitives

void tq_draw_point(tq_vec2f position)
{
    graphics_draw_point(position.x, position.y);
}

void tq_draw_line(tq_vec2f a, tq_vec2f b)
{
    graphics_draw_line(a.x, a.y, b.x, b.y);
}

void tq_draw_triangle(tq_vec2f a, tq_vec2f b, tq_vec2f c)
{
    graphics_draw_triangle(a.x, a.y, b.x, b.y, c.x, c.y);
}

void tq_draw_rectangle(tq_rectf rect)
{
    graphics_draw_rectangle(rect.x, rect.y, rect.w, rect.h);
}

void tq_draw_circle(tq_vec2f position, float radius)
{
    graphics_draw_circle(position.x, position.y, radius);
}

void tq_outline_triangle(tq_vec2f a, tq_vec2f b, tq_vec2f c)
{
    graphics_outline_triangle(a.x, a.y, b.x, b.y, c.x, c.y);
}

void tq_outline_rectangle(tq_rectf rect)
{
    graphics_outline_rectangle(rect.x, rect.y, rect.w, rect.h);
}

void tq_outline_circle(tq_vec2f position, float radius)
{
    graphics_outline_circle(position.x, position.y, radius);
}

void tq_fill_triangle(tq_vec2f a, tq_vec2f b, tq_vec2f c)
{
    graphics_fill_triangle(a.x, a.y, b.x, b.y, c.x, c.y);
}

void tq_fill_rectangle(tq_rectf rect)
{
    graphics_fill_rectangle(rect.x, rect.y, rect.w, rect.h);
}

void tq_fill_circle(tq_vec2f position, float radius)
{
    graphics_fill_circle(position.x, position.y, radius);
}

tq_color tq_get_point_color(void)
{
    return graphics_get_point_color();
}

void tq_set_point_color(tq_color point_color)
{
    graphics_set_point_color(point_color);
}

tq_color tq_get_line_color(void)
{
    return graphics_get_line_color();
}

void tq_set_line_color(tq_color line_color)
{
    graphics_set_line_color(line_color);
}

tq_color tq_get_outline_color(void)
{
    return graphics_get_outline_color();
}

void tq_set_outline_color(tq_color outline_color)
{
    graphics_set_outline_color(outline_color);
}

tq_color tq_get_fill_color(void)
{
    return graphics_get_fill_color();
}

void tq_set_fill_color(tq_color fill_color)
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

tq_vec2i tq_get_texture_size(tq_texture texture)
{
    tq_vec2i size;
    graphics_get_texture_size(texture.id, &size.x, &size.y);

    return size;
}

void tq_set_texture_smooth(tq_texture texture, bool smooth)
{
    graphics_set_texture_smooth(texture.id, smooth);
}

void tq_draw_texture(tq_texture texture, tq_rectf rect)
{
    graphics_draw_texture(texture.id, rect.x, rect.y, rect.w, rect.h);
}

void tq_draw_subtexture(tq_texture texture, tq_rectf sub, tq_rectf rect)
{
    graphics_draw_subtexture(texture.id,
        rect.x, rect.y, rect.w, rect.h,
        sub.x, sub.y, sub.w, sub.h);
}

//----------------------------------------------------------
// Surfaces

tq_surface tq_create_surface(tq_vec2i size)
{
    return (tq_surface) { .id = graphics_create_surface(size.x, size.y) };
}

void tq_delete_surface(tq_surface surface)
{
    graphics_delete_texture(surface.id);
}

void tq_set_surface(tq_surface surface)
{
    graphics_set_surface(surface.id);
}

void tq_reset_surface(void)
{
    graphics_reset_surface();
}

tq_texture tq_get_surface_texture(tq_surface surface)
{
    return (tq_texture) { .id = graphics_get_surface_texture_id(surface.id) };
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

tq_texture tq_get_font_texture(tq_font font)
{
    return (tq_texture) { .id = text_get_font_texture(font.id) };
}

void tq_draw_text(tq_font font, tq_vec2f position, char const *text)
{
    text_draw_text(font.id, position.x, position.y, text);
}

void tq_print_text(tq_font font, tq_vec2f position, char const *fmt, ...)
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

//----------------------------------------------------------
// Blending

void tq_set_blend_mode(tq_blend_mode mode)
{
    graphics_set_blend_mode(mode);
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
