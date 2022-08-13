
//------------------------------------------------------------------------------

#ifndef TQ_GRAPHICS_H_INC
#define TQ_GRAPHICS_H_INC

//------------------------------------------------------------------------------

#include "tq_image_loader.h"
#include "tq_stream.h"

//------------------------------------------------------------------------------

enum
{
    LIBTQ_POINTS,
    LIBTQ_LINE_STRIP,
    LIBTQ_LINE_LOOP,
    LIBTQ_TRIANGLES,
    LIBTQ_TRIANGLE_FAN,
};

struct libtq_renderer_impl
{
    void    (*initialize)(void);
    void    (*terminate)(void);
    void    (*process)(void);
    void    (*post_process)(void);

    int     (*request_antialiasing_level)(int level);

    void    (*update_projection)(float const *mat4);
    void    (*update_model_view)(float const *mat3);

    int     (*create_texture)(int width, int height, int channels);
    void    (*delete_texture)(int32_t texture_id);
    bool    (*is_texture_smooth)(int texture_id);
    void    (*set_texture_smooth)(int texture_id, bool smooth);
    void    (*get_texture_size)(int texture_id, int *width, int *height);
    void    (*update_texture)(int texture_id, int x_offset, int y_offset, int width, int height, unsigned char *pixels);
    void    (*bind_texture)(int texture_id);

    int     (*create_surface)(int width, int height);
    void    (*delete_surface)(int surface_id);
    int     (*get_surface_texture_id)(int surface_id);
    void    (*bind_surface)(int surface_id);

    void    (*set_clear_color)(tq_color color);
    void    (*set_draw_color)(tq_color color);
    void    (*set_blend_mode)(tq_blend_mode mode);

    void    (*clear)(void);
    void    (*draw_solid)(int mode, float const *data, int num_vertices);
    void    (*draw_colored)(int mode, float const *data, int num_vertices);
    void    (*draw_textured)(int mode, float const *data, int num_vertices);
    void    (*draw_font)(float const *data, int num_vertices);
    void    (*draw_canvas)(float x0, float y0, float x1, float y1);
};

#if defined(TQ_USE_OPENGL)
    void libtq_construct_gl_renderer(struct libtq_renderer_impl *renderer);
#endif

//------------------------------------------------------------------------------

void        libtq_initialize_graphics(void);
void        libtq_terminate_graphics(void);
void        libtq_process_graphics(void);

int         libtq_get_antialiasing_level(void);
void        libtq_set_antialiasing_level(int level);

void        libtq_clear(void);
tq_color    libtq_get_clear_color(void);
void        libtq_set_clear_color(tq_color clear_color);

void        libtq_get_canvas_size(int *width, int *height);
void        libtq_set_canvas_size(int width, int height);
float       libtq_get_canvas_aspect_ratio(void);

bool        libtq_is_canvas_smooth(void);
void        libtq_set_canvas_smooth(bool smooth);

void        libtq_conv_display_coord_to_canvas_coord(int x, int y, int *u, int *v);

void        libtq_get_relative_position(float ax, float ay, float *x, float *y);
void        libtq_set_view(float x, float y, float w, float h, float rotation);
void        libtq_reset_view(void);

void        libtq_push_matrix(void);
void        libtq_pop_matrix(void);
void        libtq_translate_matrix(float x, float y);
void        libtq_scale_matrix(float x, float y);
void        libtq_rotate_matrix(float a);

void        libtq_draw_point(float x, float y);
void        libtq_draw_line(float ax, float ay, float bx, float by);
void        libtq_draw_triangle(float ax, float ay, float bx, float by, float cx, float cy);
void        libtq_draw_rectangle(float x, float y, float w, float h);
void        libtq_draw_circle(float x, float y, float radius);
void        libtq_outline_triangle(float ax, float ay, float bx, float by, float cx, float cy);
void        libtq_outline_rectangle(float x, float y, float w, float h);
void        libtq_outline_circle(float x, float y, float radius);
void        libtq_fill_triangle(float ax, float ay, float bx, float by, float cx, float cy);
void        libtq_fill_rectangle(float x, float y, float w, float h);
void        libtq_fill_circle(float x, float y, float radius);

tq_color    libtq_get_point_color(void);
void        libtq_set_point_color(tq_color point_color);
tq_color    libtq_get_line_color(void);
void        libtq_set_line_color(tq_color line_color);
tq_color    libtq_get_outline_color(void);
void        libtq_set_outline_color(tq_color outline_color);
tq_color    libtq_get_fill_color(void);
void        libtq_set_fill_color(tq_color fill_color);

int         libtq_load_texture(libtq_stream *stream);
int         libtq_load_texture_from_file(char const *path);
int         libtq_load_texture_from_memory(void const *buffer, size_t length);
void        libtq_delete_texture(int texture_id);

void        libtq_get_texture_size(int texture_id, int *width, int *height);
void        libtq_set_texture_smooth(int texture_id, bool smooth);

void        libtq_draw_texture(int texture_id, float x, float y, float w, float h);
void        libtq_draw_subtexture(int texture_id, float x, float y, float w, float h, float fx, float fy, float fw, float fh);

int         libtq_create_surface(int width, int height);
void        libtq_delete_surface(int surface_id);
void        libtq_set_surface(int surface_id);
void        libtq_reset_surface(void);
int         libtq_get_surface_texture_id(int surface_id);

void        libtq_set_blend_mode(tq_blend_mode mode);

//------------------------------------------------------------------------------

#endif // TQ_GRAPHICS_H_INC
