
//------------------------------------------------------------------------------

#ifndef TQ_GRAPHICS_H_INC
#define TQ_GRAPHICS_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"
#include "tq_display.h"
#include "tq_renderer.h"

//------------------------------------------------------------------------------

void graphics_initialize(struct renderer const *renderer);
void graphics_terminate(void);
void graphics_flush(void);

void graphics_clear(void);
tq_color_t graphics_get_clear_color(void);
void graphics_set_clear_color(tq_color_t clear_color);

void graphics_set_view(float x, float y, float width, float height, float angle);
void graphics_reset_view(void);

void graphics_push_matrix(void);
void graphics_pop_matrix(void);
void graphics_translate_matrix(float x, float y);
void graphics_scale_matrix(float x, float y);
void graphics_rotate_matrix(float a);

void graphics_draw_point(float x, float y);
void graphics_draw_line(float ax, float ay, float bx, float by);
void graphics_draw_triangle(float ax, float ay, float bx, float by, float cx, float cy);
void graphics_draw_rectangle(float x, float y, float w, float h);
void graphics_draw_circle(float x, float y, float radius);

void graphics_outline_triangle(float ax, float ay, float bx, float by, float cx, float cy);
void graphics_outline_rectangle(float x, float y, float w, float h);
void graphics_outline_circle(float x, float y, float radius);

void graphics_fill_triangle(float ax, float ay, float bx, float by, float cx, float cy);
void graphics_fill_rectangle(float x, float y, float w, float h);
void graphics_fill_circle(float x, float y, float radius);

tq_color_t graphics_get_point_color(void);
void graphics_set_point_color(tq_color_t point_color);

tq_color_t graphics_get_line_color(void);
void graphics_set_line_color(tq_color_t line_color);

tq_color_t graphics_get_outline_color(void);
void graphics_set_outline_color(tq_color_t outline_color);

tq_color_t graphics_get_fill_color(void);
void graphics_set_fill_color(tq_color_t fill_color);

tq_handle_t graphics_load_texture_from_file(char const *path);
tq_handle_t graphics_load_texture_from_memory(uint8_t const *buffer, size_t length);
void graphics_delete_texture(tq_handle_t texture_handle);
void graphics_get_texture_size(tq_handle_t texture_handle, uint32_t *width, uint32_t *height);
void graphics_draw_texture(tq_handle_t texture_handle, float x, float y, float w, float h);
void graphics_draw_texture_fragment(tq_handle_t texture_handle, float x, float y, float w, float h, float u0, float v0, float u1, float v1);

//------------------------------------------------------------------------------

#endif // TQ_GRAPHICS_H_INC
