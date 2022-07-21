
//------------------------------------------------------------------------------

#ifndef TQ_GRAPHICS_H_INC
#define TQ_GRAPHICS_H_INC

//------------------------------------------------------------------------------

#include <stdarg.h>
#include "tq_image_loader.h"
#include "tq_stream.h"

//------------------------------------------------------------------------------

// [tq::graphics::initialize]
void tq_graphics_initialize(void);

// [tq::graphics::terminate]
void tq_graphics_terminate(void);

// [tq::graphics::process]
void tq_graphics_process(void);

void graphics_clear(void);
tq_color graphics_get_clear_color(void);
void graphics_set_clear_color(tq_color clear_color);

void graphics_get_relative_position(float ax, float ay, float *x, float *y);
void graphics_set_view(float x, float y, float w, float h, float rotation);
void graphics_reset_view(void);
void graphics_set_auto_view_reset_enabled(bool enabled);

// [tq::graphics::push_matrix]
void graphics_push_matrix(void);

// [tq::graphics::pop_matrix]
void graphics_pop_matrix(void);

// [tq::graphics::translate_matrix]
void graphics_translate_matrix(float x, float y);

// [tq::graphics::scale_matrix]
void graphics_scale_matrix(float x, float y);

// [tq::graphics::rotate_matrix]
void graphics_rotate_matrix(float a);

// [tq::graphics::draw_point]
void graphics_draw_point(float x, float y);

// [tq::graphics::draw_line]
void graphics_draw_line(float ax, float ay, float bx, float by);

// [tq::graphics::draw_triangle]
void graphics_draw_triangle(float ax, float ay, float bx, float by, float cx, float cy);

// [tq::graphics::draw_rectangle]
void graphics_draw_rectangle(float x, float y, float w, float h);

// [tq::graphics::draw_circle]
void graphics_draw_circle(float x, float y, float radius);

// [tq::graphics::outline_triangle]
void graphics_outline_triangle(float ax, float ay, float bx, float by, float cx, float cy);

// [tq::graphics::outline_rectangle]
void graphics_outline_rectangle(float x, float y, float w, float h);

// [tq::graphics::outline_circle]
void graphics_outline_circle(float x, float y, float radius);

// [tq::graphics::fill_triangle]
void graphics_fill_triangle(float ax, float ay, float bx, float by, float cx, float cy);

// [tq::graphics::fill_rectangle]
void graphics_fill_rectangle(float x, float y, float w, float h);

// [tq::graphics::fill_circle]
void graphics_fill_circle(float x, float y, float radius);

tq_color graphics_get_point_color(void);
void graphics_set_point_color(tq_color point_color);

tq_color graphics_get_line_color(void);
void graphics_set_line_color(tq_color line_color);

tq_color graphics_get_outline_color(void);
void graphics_set_outline_color(tq_color outline_color);

tq_color graphics_get_fill_color(void);
void graphics_set_fill_color(tq_color fill_color);

int graphics_load_texture(int stream_id);
int graphics_load_texture_from_file(char const *path);
int graphics_load_texture_from_memory(void const *buffer, size_t length);
void graphics_delete_texture(int texture_id);

void graphics_get_texture_size(int texture_id, int *width, int *height);
void graphics_set_texture_smooth(int texture_id, bool smooth);

void graphics_draw_texture(int texture_id,
    float x, float y,
    float w, float h);

void graphics_draw_subtexture(int texture_id,
    float x, float y,
    float w, float h,
    float fx, float fy,
    float fw, float fh);

void graphics_on_display_resized(int width, int height);

//------------------------------------------------------------------------------

enum
{
    PRIMITIVE_POINTS,
    PRIMITIVE_LINE_STRIP,
    PRIMITIVE_LINE_LOOP,
    PRIMITIVE_TRIANGLES,
    PRIMITIVE_TRIANGLE_FAN,
};

struct renderer_impl
{
    void (*initialize)(void);
    void (*terminate)(void);
    void (*process)(void);

    void (*update_viewport)(int x, int y, int w, int h);
    void (*update_projection)(float const *mat4);
    void (*update_model_view)(float const *mat3);

    int32_t (*create_texture)(int width, int height, int channels);
    void (*delete_texture)(int32_t texture_id);
    void (*set_texture_smooth)(int texture_id, bool smooth);
    void (*get_texture_size)(int texture_id, int *width, int *height);
    void (*update_texture)(int texture_id, int x_offset, int y_offset, int width, int height, unsigned char *pixels);
    void (*resize_texture)(int texture_id, int new_width, int new_height);
    void (*bind_texture)(int texture_id);

    void (*set_clear_color)(tq_color color);
    void (*set_draw_color)(tq_color color);

    void (*clear)(void);
    void (*draw_solid)(int mode, float const *data, int num_vertices);
    void (*draw_colored)(int mode, float const *data, int num_vertices);
    void (*draw_textured)(int mode, float const *data, int num_vertices);
    void (*draw_font)(float const *data, unsigned int const *indices, int num_indices);
};

//------------------------------------------------------------------------------

#if defined(TQ_USE_OPENGL)
    void construct_gl_renderer(struct renderer_impl *renderer);
#endif

//------------------------------------------------------------------------------

#endif // TQ_GRAPHICS_H_INC
