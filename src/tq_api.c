
//------------------------------------------------------------------------------

#include "tq_audio.h"
#include "tq_core.h"
#include "tq_graphics.h"

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

void tq_on_key_pressed(tq_key_callback_t callback)
{
    tq_core_set_key_press_callback(callback);
}

void tq_on_key_released(tq_key_callback_t callback)
{
    tq_core_set_key_release_callback(callback);
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

int32_t tq_load_sound_from_file(char const *path)
{
    tq_audio_load_sound_from_file(path);
}

int32_t tq_load_sound_from_memory(uint8_t const *buffer, size_t length)
{
    tq_audio_load_sound_from_memory(buffer, length);
}

void tq_delete_sound(int32_t sound_id)
{
    tq_audio_delete_sound(sound_id);
}

int32_t tq_play_sound(int32_t sound_id, int loop)
{
    return tq_audio_play_sound(sound_id, loop);
}

int32_t tq_open_music_from_file(char const *path)
{
    return tq_audio_open_music_from_file(path);
}

int32_t tq_open_music_from_memory(uint8_t const *buffer, size_t length)
{
    return tq_audio_open_music_from_memory(buffer, length);
}

void tq_close_music(int32_t music_id)
{
    tq_audio_close_music(music_id);
}

int32_t tq_play_music(int32_t music_id, int loop)
{
    return tq_audio_play_music(music_id, loop);
}

tq_channel_state_t tq_get_channel_state(int32_t channel_id)
{
    return tq_audio_get_channel_state(channel_id);
}

void tq_pause_channel(int32_t channel_id)
{
    tq_audio_pause_channel(channel_id);
}

void tq_unpause_channel(int32_t channel_id)
{
    tq_audio_unpause_channel(channel_id);
}

void tq_stop_channel(int32_t channel_id)
{
    tq_audio_stop_channel(channel_id);
}

//------------------------------------------------------------------------------
