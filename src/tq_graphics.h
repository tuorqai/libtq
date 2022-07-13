
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

// [tq::graphics::load_texture_from_file]
int32_t graphics_load_texture_from_file(char const *path);

// [tq::graphics::load_texture_from_memory]
int32_t graphics_load_texture_from_memory(uint8_t const *buffer, size_t length);

// [tq::graphics::delete_texture]
void graphics_delete_texture(int32_t texture_id);

// [tq::graphics::get_texture_size]
void graphics_get_texture_size(int32_t texture_id, uint32_t *width, uint32_t *height);

// [tq::graphics::draw_texture]
void graphics_draw_texture(int32_t texture_id,
    float x, float y,
    float w, float h);

// [tq::graphics::draw_texture_fragment]
void graphics_draw_texture_fragment(int32_t texture_id,
    float x, float y,
    float w, float h,
    float fx, float fy,
    float fw, float fh);

// [tq::graphics::on_display_resized]
void tq_graphics_on_display_resized(uint32_t width, uint32_t height);

int32_t     graphics_load_font_from_file(char const *path, float pt, int weight);
int32_t     graphics_load_font_from_memory(uint8_t const *buffer, size_t size, float pt, int weight);
void        graphics_delete_font(int32_t font_id);
void        graphics_draw_text(int32_t font_id, float x, float y, char const *fmt, va_list ap);

//------------------------------------------------------------------------------

typedef struct tq_renderer
{
    void        (*initialize)(void);
    void        (*terminate)(void);
    void        (*process)(void);

    void        (*clear)(void);
    void        (*set_clear_color)(tq_color_t);

    void        (*update_viewport)(int x, int y, int w, int h);
    void        (*update_projection)(float const *mat4);
    void        (*update_model_view)(float const *mat3);

    void        (*draw_points)(float const *, unsigned int);
    void        (*draw_lines)(float const *, unsigned int);
    void        (*draw_outline)(float const *, unsigned int);
    void        (*draw_fill)(float const *, unsigned int);

    void        (*set_point_color)(tq_color_t);
    void        (*set_line_color)(tq_color_t);
    void        (*set_outline_color)(tq_color_t);
    void        (*set_fill_color)(tq_color_t);

    int32_t     (*create_texture)(tq_image_t const *image);
    int32_t     (*load_texture)(int32_t stream_id);
    void        (*delete_texture)(int32_t texture_id);
    void        (*get_texture_size)(int32_t texture_id, uint32_t *width, uint32_t *height);
    void        (*draw_texture)(int32_t texture_id, float const *data, uint32_t num_vertices);

    void        (*draw_text)(int32_t texture_id, float const *data, uint32_t num_vertices);
    void        (*draw_text2)(float x, float y, struct image image);
} tq_renderer_t;

//------------------------------------------------------------------------------

#if defined(TQ_USE_OPENGL)
    void tq_construct_gl_renderer(tq_renderer_t *renderer);
#endif

//------------------------------------------------------------------------------

#endif // TQ_GRAPHICS_H_INC
