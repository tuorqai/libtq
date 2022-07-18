
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

// [tq::graphics::clear]
void graphics_clear(void);

// [tq::graphics::get_clear_color]
tq_color_t graphics_get_clear_color(void);

// [tq::graphics::set_clear_color]
void graphics_set_clear_color(tq_color_t clear_color);

// [tq::graphics::view]
void tq_graphics_view(float x, float y, float w, float h, float rotation);

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

// [tq::graphics::get_point_color]
tq_color_t graphics_get_point_color(void);

// [tq::graphics::set_point_color]
void graphics_set_point_color(tq_color_t point_color);

// [tq::graphics::get_line_color]
tq_color_t graphics_get_line_color(void);

// [tq::graphics::set_line_color]
void graphics_set_line_color(tq_color_t line_color);

// [tq::graphics::get_outline_color]
tq_color_t graphics_get_outline_color(void);

// [tq::graphics::set_outline_color]
void graphics_set_outline_color(tq_color_t outline_color);

// [tq::graphics::get_fill_color]
tq_color_t graphics_get_fill_color(void);

// [tq::graphics::set_fill_color]
void graphics_set_fill_color(tq_color_t fill_color);

int graphics_load_texture(int stream_id);
int graphics_load_texture_from_file(char const *path);
int graphics_load_texture_from_memory(void const *buffer, size_t length);
void graphics_delete_texture(int texture_id);

void graphics_get_texture_size(int texture_id, int *width, int *height);

// [tq::graphics::draw_texture]
void graphics_draw_texture(int texture_id,
    float x, float y,
    float w, float h);

// [tq::graphics::draw_texture_fragment]
void graphics_draw_texture_fragment(int texture_id,
    float x, float y,
    float w, float h,
    float fx, float fy,
    float fw, float fh);

// [tq::graphics::on_display_resized]
void tq_graphics_on_display_resized(uint32_t width, uint32_t height);

//------------------------------------------------------------------------------

enum
{
    RENDERER_MODE_POINTS,
    RENDERER_MODE_LINE_STRIP,
    RENDERER_MODE_TRIANGLES,
    RENDERER_MODE_TRIANGLE_FAN,
};

typedef struct renderer_impl
{
    void        (*initialize)(void);
    void        (*terminate)(void);
    void        (*process)(void);

    void        (*update_viewport)(int x, int y, int w, int h);
    void        (*update_projection)(float const *mat4);
    void        (*update_model_view)(float const *mat3);

    int32_t     (*create_texture)(int width, int height, int channels);
    void        (*delete_texture)(int32_t texture_id);
    void        (*get_texture_size)(int texture_id, int *width, int *height);
    void        (*update_texture)(int texture_id, int x_offset, int y_offset, int width, int height, unsigned char *pixels);
    void        (*resize_texture)(int texture_id, int new_width, int new_height);
    void        (*bind_texture)(int texture_id);

    void (*clear)(float r, float g, float b);
    void (*draw_solid)(int mode, float const *data, int num_vertices);
    void (*draw_textured)(int mode, float const *data, int num_vertices);
    void (*draw_font)(float const *data, unsigned int const *indices, int num_indices);
} tq_renderer_t;

//------------------------------------------------------------------------------

#if defined(TQ_USE_OPENGL)
    void tq_construct_gl_renderer(tq_renderer_t *renderer);
#endif

//------------------------------------------------------------------------------

#endif // TQ_GRAPHICS_H_INC
