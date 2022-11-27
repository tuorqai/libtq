
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
    libtq_initialize_core();
    libtq_initialize_graphics();
    libtq_initialize_audio();
}

void tq_terminate(void)
{
    libtq_terminate_audio();
    libtq_terminate_graphics();
    libtq_terminate_core();
}

bool tq_process(void)
{
    libtq_process_graphics();
    libtq_process_audio();

    return libtq_process_core();
}

//------------------------------------------------------------------------------
// Core

//----------------------------------------------------------
// Display

tq_vec2i tq_get_display_size(void)
{
    tq_vec2i size;
    libtq_get_display_size(&size.x, &size.y);

    return size;
}

void tq_set_display_size(tq_vec2i size)
{
    libtq_set_display_size(size.x, size.y);
}

char const *tq_get_title(void)
{
    return libtq_get_title();
}

void tq_set_title(char const *title)
{
    libtq_set_title(title);
}

//----------------------------------------------------------
// Keyboard

bool tq_is_key_autorepeat_enabled(void)
{
    return libtq_is_key_autorepeat_enabled();
}

void tq_set_key_autorepeat_enabled(bool enabled)
{
    libtq_set_key_autorepeat_enabled(enabled);
}

bool tq_is_key_pressed(tq_key key)
{
    return libtq_is_key_pressed(key);
}

bool tq_is_mouse_button_pressed(tq_mouse_button mouse_button)
{
    return libtq_is_mouse_button_pressed(mouse_button);
}

void tq_on_key_pressed(tq_key_callback callback)
{
    libtq_set_key_press_callback(callback);
}

void tq_on_key_released(tq_key_callback callback)
{
    libtq_set_key_release_callback(callback);
}

//----------------------------------------------------------
// Mouse

bool tq_is_mouse_cursor_hidden(void)
{
    return libtq_is_mouse_cursor_hidden();
}

void tq_set_mouse_cursor_hidden(bool hidden)
{
    libtq_set_mouse_cursor_hidden(hidden);
}

tq_vec2i tq_get_mouse_cursor_position(void)
{
    tq_vec2i position;
    libtq_get_mouse_cursor_position(&position.x, &position.y);

    return position;
}

void tq_on_mouse_button_pressed(tq_mouse_button_callback callback)
{
    libtq_set_mousebutton_press_callback(callback);
}

void tq_on_mouse_button_released(tq_mouse_button_callback callback)
{
    libtq_set_mousebutton_release_callback(callback);
}

void tq_on_mouse_cursor_moved(tq_mouse_cursor_callback callback)
{
    libtq_set_mousecursor_move_callback(callback);
}

void tq_on_mouse_wheel_scrolled(tq_mouse_wheel_callback callback)
{
    libtq_set_mousewheel_scroll_callback(callback);
}

//----------------------------------------------------------
// Time

float tq_get_time_mediump(void)
{
    return libtq_get_time_mediump();
}

double tq_get_time_highp(void)
{
    return libtq_get_time_highp();
}

double tq_get_delta_time(void)
{
    return libtq_get_delta_time();
}

//----------------------------------------------------------
// Stats

int tq_get_framerate(void)
{
    return libtq_get_framerate();
}

//------------------------------------------------------------------------------
// Graphics

//----------------------------------------------------------
// Options

void tq_set_color_key(tq_color color)
{
    libtq_set_color_key(color);
}

void tq_set_antialiasing_level(int level)
{
    libtq_set_antialiasing_level(level);
}

//----------------------------------------------------------
// Canvas

void tq_clear(void)
{
    libtq_clear();
}

tq_color tq_get_clear_color(void)
{
    return libtq_get_clear_color();
}

void tq_set_clear_color(tq_color clear_color)
{
    libtq_set_clear_color(clear_color);
}

tq_vec2i tq_get_canvas_size(void)
{
    tq_vec2i size;
    libtq_get_canvas_size(&size.x, &size.y);

    return size;
}

void tq_set_canvas_size(tq_vec2i size)
{
    libtq_set_canvas_size(size.x, size.y);
}

bool tq_is_canvas_smooth(void)
{
    return libtq_is_canvas_smooth();
}

void tq_set_canvas_smooth(bool smooth)
{
    libtq_set_canvas_smooth(smooth);
}

//----------------------------------------------------------
// View

tq_vec2f tq_get_relative_position(tq_vec2f absolute)
{
    tq_vec2f relative;
    libtq_get_relative_position(absolute.x, absolute.y, &relative.x, &relative.y);

    return relative;
}

void tq_set_view(tq_rectf rect, float rotation)
{
    libtq_set_view(rect.x, rect.y, rect.w, rect.h, rotation);
}

void tq_reset_view(void)
{
    libtq_reset_view();
}

//----------------------------------------------------------
// Transformation matrix stack

void tq_push_matrix(void)
{
    libtq_push_matrix();
}

void tq_pop_matrix(void)
{
    libtq_pop_matrix();
}

void tq_translate_matrix(tq_vec2f v)
{
    libtq_translate_matrix(v.x, v.y);
}

void tq_scale_matrix(tq_vec2f v)
{
    libtq_scale_matrix(v.x, v.y);
}

void tq_rotate_matrix(float degrees)
{
    libtq_rotate_matrix(degrees);
}

//----------------------------------------------------------
// Primitives

void tq_draw_point(tq_vec2f position)
{
    libtq_draw_point(position.x, position.y);
}

void tq_draw_line(tq_vec2f a, tq_vec2f b)
{
    libtq_draw_line(a.x, a.y, b.x, b.y);
}

void tq_draw_triangle(tq_vec2f a, tq_vec2f b, tq_vec2f c)
{
    libtq_draw_triangle(a.x, a.y, b.x, b.y, c.x, c.y);
}

void tq_draw_rectangle(tq_rectf rect)
{
    libtq_draw_rectangle(rect.x, rect.y, rect.w, rect.h);
}

void tq_draw_circle(tq_vec2f position, float radius)
{
    libtq_draw_circle(position.x, position.y, radius);
}

void tq_outline_triangle(tq_vec2f a, tq_vec2f b, tq_vec2f c)
{
    libtq_outline_triangle(a.x, a.y, b.x, b.y, c.x, c.y);
}

void tq_outline_rectangle(tq_rectf rect)
{
    libtq_outline_rectangle(rect.x, rect.y, rect.w, rect.h);
}

void tq_outline_circle(tq_vec2f position, float radius)
{
    libtq_outline_circle(position.x, position.y, radius);
}

void tq_fill_triangle(tq_vec2f a, tq_vec2f b, tq_vec2f c)
{
    libtq_fill_triangle(a.x, a.y, b.x, b.y, c.x, c.y);
}

void tq_fill_rectangle(tq_rectf rect)
{
    libtq_fill_rectangle(rect.x, rect.y, rect.w, rect.h);
}

void tq_fill_circle(tq_vec2f position, float radius)
{
    libtq_fill_circle(position.x, position.y, radius);
}

tq_color tq_get_point_color(void)
{
    return libtq_get_point_color();
}

void tq_set_point_color(tq_color point_color)
{
    libtq_set_point_color(point_color);
}

tq_color tq_get_line_color(void)
{
    return libtq_get_line_color();
}

void tq_set_line_color(tq_color line_color)
{
    libtq_set_line_color(line_color);
}

tq_color tq_get_outline_color(void)
{
    return libtq_get_outline_color();
}

void tq_set_outline_color(tq_color outline_color)
{
    libtq_set_outline_color(outline_color);
}

tq_color tq_get_fill_color(void)
{
    return libtq_get_fill_color();
}

void tq_set_fill_color(tq_color fill_color)
{
    libtq_set_fill_color(fill_color);
}

//----------------------------------------------------------
// Textures

tq_texture tq_load_texture_from_file(char const *path)
{
    return (tq_texture) { .id = libtq_load_texture_from_file(path) };
}

tq_texture tq_load_texture_from_memory(uint8_t const *buffer, size_t length)
{
    return (tq_texture) { .id = libtq_load_texture_from_memory(buffer, length) };
}

void tq_delete_texture(tq_texture texture)
{
    libtq_delete_texture(texture.id);
}

tq_vec2i tq_get_texture_size(tq_texture texture)
{
    tq_vec2i size;
    libtq_get_texture_size(texture.id, &size.x, &size.y);

    return size;
}

void tq_set_texture_smooth(tq_texture texture, bool smooth)
{
    libtq_set_texture_smooth(texture.id, smooth);
}

void tq_draw_texture(tq_texture texture, tq_rectf rect)
{
    libtq_draw_texture(texture.id, rect.x, rect.y, rect.w, rect.h);
}

void tq_draw_subtexture(tq_texture texture, tq_rectf sub, tq_rectf rect)
{
    libtq_draw_subtexture(texture.id,
        rect.x, rect.y, rect.w, rect.h,
        sub.x, sub.y, sub.w, sub.h);
}

//----------------------------------------------------------
// Surfaces

tq_surface tq_create_surface(tq_vec2i size)
{
    return (tq_surface) { .id = libtq_create_surface(size.x, size.y) };
}

void tq_delete_surface(tq_surface surface)
{
    libtq_delete_texture(surface.id);
}

void tq_set_surface(tq_surface surface)
{
    libtq_set_surface(surface.id);
}

void tq_reset_surface(void)
{
    libtq_reset_surface();
}

tq_texture tq_get_surface_texture(tq_surface surface)
{
    return (tq_texture) { .id = libtq_get_surface_texture_id(surface.id) };
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

            buffer = libtq_realloc(buffer, buffer_size);
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
    libtq_set_blend_mode(mode);
}

//------------------------------------------------------------------------------
// Audio

//----------------------------------------------------------
// Sounds

void tq_set_master_volume(float volume)
{
    libtq_set_master_volume(volume);
}

tq_sound tq_load_sound_from_file(char const *path)
{
    return (tq_sound) {libtq_load_sound(libtq_open_file_stream(path))};
}

tq_sound tq_load_sound_from_memory(void const *buffer, size_t size)
{
    return (tq_sound) {libtq_load_sound(libtq_open_memory_stream(buffer, size))};
}

void tq_delete_sound(tq_sound sound)
{
    libtq_delete_sound(sound.id);
}

tq_channel tq_play_sound(tq_sound sound, int loop)
{
    return (tq_channel) {libtq_play_sound(sound.id, loop)};
}

//----------------------------------------------------------
// Music

tq_music tq_open_music_from_file(char const *path)
{
    return (tq_music) {libtq_open_music(libtq_open_file_stream(path))};
}

tq_music tq_open_music_from_memory(void const *buffer, size_t size)
{
    return (tq_music) {libtq_open_music(libtq_open_memory_stream(buffer, size))};
}

void tq_close_music(tq_music music)
{
    libtq_close_music(music.id);
}

tq_channel tq_play_music(tq_music music, int loop)
{
    return (tq_channel) {libtq_play_music(music.id, loop)};
}

//----------------------------------------------------------
// Channels

tq_channel_state tq_get_channel_state(tq_channel channel)
{
    return libtq_get_channel_state(channel.id);
}

void tq_pause_channel(tq_channel channel)
{
    libtq_pause_channel(channel.id);
}

void tq_unpause_channel(tq_channel channel)
{
    libtq_unpause_channel(channel.id);
}

void tq_stop_channel(tq_channel channel)
{
    libtq_stop_channel(channel.id);
}

//------------------------------------------------------------------------------
