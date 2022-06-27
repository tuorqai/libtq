
//------------------------------------------------------------------------------

#include "tq_audio.h"
#include "tq_core.h"
#include "tq_graphics.h"
#include "tq_meta.h"

//------------------------------------------------------------------------------
// Meta

void tq_initialize(void)
{
    meta_initialize();
}

void tq_terminate(void)
{
    meta_terminate();
}

bool tq_process(void)
{
    return meta_process();
}

//------------------------------------------------------------------------------
// Core

uint32_t tq_get_display_width(void)
{
    return core_get_display_width();
}

uint32_t tq_get_display_height(void)
{
    return core_get_display_height();
}

void tq_get_display_size(uint32_t *width, uint32_t *height)
{
    core_get_display_size(width, height);
}

void tq_set_display_size(uint32_t width, uint32_t height)
{
    core_set_display_size(width, height);
}

char const *tq_get_title(void)
{
    return core_get_title();
}

void tq_set_title(char const *title)
{
    core_set_title(title);
}

bool tq_is_key_pressed(tq_key_t key)
{
    return core_is_key_pressed(key);
}

bool tq_is_mouse_button_pressed(tq_mouse_button_t mouse_button)
{
    return core_is_mouse_button_pressed(mouse_button);
}

int32_t tq_get_mouse_cursor_x(void)
{
    return core_get_mouse_cursor_x();
}

int32_t tq_get_mouse_cursor_y(void)
{
    return core_get_mouse_cursor_y();
}

void tq_get_mouse_cursor_position(int32_t *x, int32_t *y)
{
    *x = core_get_mouse_cursor_x();
    *y = core_get_mouse_cursor_y();
}

float tq_get_time_mediump(void)
{
    return core_get_time_mediump();
}

double tq_get_time_highp(void)
{
    return core_get_time_highp();
}

double tq_get_delta_time(void)
{
    return core_get_delta_time();
}

unsigned int tq_get_framerate(void)
{
    return core_get_framerate();
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

void tq_view_v(tq_vec2_t position, tq_vec2_t size, float angle)
{
    tq_graphics_view(position.x, position.y, size.x, size.y, angle);
}

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

void tq_translate_matrix_v(tq_vec2_t v)
{
    graphics_translate_matrix(v.x, v.y);
}

void tq_scale_matrix_f(float x, float y)
{
    graphics_scale_matrix(x, y);
}

void tq_scale_matrix_v(tq_vec2_t v)
{
    graphics_scale_matrix(v.x, v.y);
}

void tq_rotate_matrix(float degrees)
{
    graphics_rotate_matrix(degrees);
}

void tq_draw_point_f(float x, float y)
{
    graphics_draw_point(x, y);
}

void tq_draw_point_v(tq_vec2_t position)
{
    graphics_draw_point(position.x, position.y);
}

void tq_draw_line_f(float ax, float ay, float bx, float by)
{
    graphics_draw_line(ax, ay, bx, by);
}

void tq_draw_line_v(tq_vec2_t a, tq_vec2_t b)
{
    graphics_draw_line(a.x, a.y, b.x, b.y);
}

void tq_draw_triangle_f(float ax, float ay, float bx, float by, float cx, float cy)
{
    graphics_draw_triangle(ax, ay, bx, by, cx, cy);
}

void tq_draw_triangle_v(tq_vec2_t a, tq_vec2_t b, tq_vec2_t c)
{
    graphics_draw_triangle(a.x, a.y, b.x, b.y, c.x, c.y);
}

void tq_draw_rectangle_f(float x, float y, float w, float h)
{
    graphics_draw_rectangle(x, y, w, h);
}

void tq_draw_rectangle_v(tq_vec2_t position, tq_vec2_t size)
{
    graphics_draw_rectangle(position.x, position.y, size.x, size.y);
}

void tq_draw_circle_f(float x, float y, float radius)
{
    graphics_draw_circle(x, y, radius);
}

void tq_draw_circle_v(tq_vec2_t position, float radius)
{
    graphics_draw_circle(position.x, position.y, radius);
}

void tq_outline_triangle_f(float ax, float ay, float bx, float by, float cx, float cy)
{
    graphics_outline_triangle(ax, ay, bx, by, cx, cy);
}

void tq_outline_triangle_v(tq_vec2_t a, tq_vec2_t b, tq_vec2_t c)
{
    graphics_outline_triangle(a.x, a.y, b.x, b.y, c.x, c.y);
}

void tq_outline_rectangle_f(float x, float y, float w, float h)
{
    graphics_outline_rectangle(x, y, w, h);
}

void tq_outline_rectangle_v(tq_vec2_t position, tq_vec2_t size)
{
    graphics_outline_rectangle(position.x, position.y, size.x, size.y);
}

void tq_outline_circle_f(float x, float y, float radius)
{
    graphics_outline_circle(x, y, radius);
}

void tq_outline_circle_v(tq_vec2_t position, float radius)
{
    graphics_outline_circle(position.x, position.y, radius);
}

void tq_fill_triangle_f(float ax, float ay, float bx, float by, float cx, float cy)
{
    graphics_fill_triangle(ax, ay, bx, by, cx, cy);
}

void tq_fill_triangle_v(tq_vec2_t a, tq_vec2_t b, tq_vec2_t c)
{
    graphics_fill_triangle(a.x, a.y, b.x, b.y, c.x, c.y);
}

void tq_fill_rectangle_f(float x, float y, float w, float h)
{
    graphics_fill_rectangle(x, y, w, h);
}

void tq_fill_rectangle_v(tq_vec2_t position, tq_vec2_t size)
{
    graphics_fill_rectangle(position.x, position.y, size.x, size.y);
}

void tq_fill_circle_f(float x, float y, float radius)
{
    graphics_fill_circle(x, y, radius);
}

void tq_fill_circle_v(tq_vec2_t position, float radius)
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

tq_handle_t tq_load_texture_from_file(char const *path)
{
    return graphics_load_texture_from_file(path);
}

tq_handle_t tq_load_texture_from_memory(uint8_t const *buffer, size_t length)
{
    return graphics_load_texture_from_memory(buffer, length);
}

void tq_delete_texture(tq_handle_t texture_handle)
{
    graphics_delete_texture(texture_handle);
}

uint32_t tq_get_texture_width(tq_handle_t texture_handle)
{
    uint32_t width, height;
    graphics_get_texture_size(texture_handle, &width, &height);

    return width;
}

uint32_t tq_get_texture_height(tq_handle_t texture_handle)
{
    uint32_t width, height;
    graphics_get_texture_size(texture_handle, &width, &height);
    
    return height;
}

void tq_get_texture_size(tq_handle_t texture_handle, uint32_t *width, uint32_t *height)
{
    graphics_get_texture_size(texture_handle, width, height);
}

void tq_draw_texture_f(tq_handle_t texture_handle, float x, float y, float w, float h)
{
    graphics_draw_texture(texture_handle, x, y, w, h);
}

void tq_draw_texture_v(tq_handle_t texture_handle, tq_vec2_t position, tq_vec2_t size)
{
    graphics_draw_texture(texture_handle, position.x, position.y, size.x, size.y);
}

void tq_draw_texture_fragment_f(tq_handle_t texture_handle,
    float x, float y,
    float w, float h,
    float fx, float fy,
    float fw, float fh)
{
    graphics_draw_texture_fragment(texture_handle, x, y, w, h, fx, fy, fw, fh);
}

void tq_draw_texture_fragment_v(tq_handle_t texture_handle,
    tq_vec2_t position,
    tq_vec2_t size,
    tq_vec2_t fragment_position,
    tq_vec2_t fragment_size)
{
    graphics_draw_texture_fragment(texture_handle,
        position.x, position.y,
        size.x, size.y,
        fragment_position.x, fragment_position.y,
        fragment_size.x, fragment_size.y);
}

//------------------------------------------------------------------------------
// Audio

tq_handle_t tq_load_sound_from_file(char const *path)
{
    audio_load_sound_from_file(path);
}

tq_handle_t tq_load_sound_from_memory(uint8_t const *buffer, size_t length)
{
    audio_load_sound_from_memory(buffer, length);
}

void tq_delete_sound(tq_handle_t sound_handle)
{
    audio_delete_sound(sound_handle);
}

tq_handle_t tq_play_sound(tq_handle_t sound_handle, float left_volume, float right_volume, int loop)
{
    return audio_play_sound(sound_handle, left_volume, right_volume, loop);
}

tq_handle_t tq_open_music_from_file(char const *path)
{
    return audio_open_music_from_file(path);
}

tq_handle_t tq_open_music_from_memory(uint8_t const *buffer, size_t length)
{
    return audio_open_music_from_memory(buffer, length);
}

void tq_close_music(tq_handle_t music_handle)
{
    audio_close_music(music_handle);
}

tq_handle_t tq_play_music(tq_handle_t music_handle, int loop)
{
    return audio_play_music(music_handle, loop);
}

tq_wave_state_t tq_get_wave_state(tq_handle_t wave_handle)
{
    return audio_get_wave_state(wave_handle);
}

void tq_pause_wave(tq_handle_t wave_handle)
{
    audio_pause_wave(wave_handle);
}

void tq_unpause_wave(tq_handle_t wave_handle)
{
    audio_unpause_wave(wave_handle);
}

void tq_stop_wave(tq_handle_t wave_handle)
{
    audio_stop_wave(wave_handle);
}

//------------------------------------------------------------------------------
