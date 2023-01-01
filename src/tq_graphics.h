//------------------------------------------------------------------------------
// Copyright (c) 2021-2023 tuorqai
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//------------------------------------------------------------------------------

#ifndef TQ_GRAPHICS_H_INC
#define TQ_GRAPHICS_H_INC

//------------------------------------------------------------------------------

#include "tq_image_loader.h"
#include "tq_stream.h"

//------------------------------------------------------------------------------

typedef enum tq_primitive
{
    TQ_PRIMITIVE_POINTS,
    TQ_PRIMITIVE_LINE_STRIP,
    TQ_PRIMITIVE_LINE_LOOP,
    TQ_PRIMITIVE_TRIANGLES,
    TQ_PRIMITIVE_TRIANGLE_FAN,
} tq_primitive;

typedef struct tq_renderer_impl
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
} tq_renderer_impl;

#if defined(TQ_WIN32) || defined(TQ_LINUX)
    void tq_construct_gl_renderer(tq_renderer_impl *impl);
#endif

#if defined(TQ_ANDROID) || defined(TQ_USE_GLES2)
    void tq_construct_gles2_renderer(tq_renderer_impl *impl);
#endif

void tq_construct_null_renderer(tq_renderer_impl *impl);

//------------------------------------------------------------------------------

void tq_initialize_graphics(void);
void tq_terminate_graphics(void);
void tq_process_graphics(void);

tq_vec2i tq_conv_display_coord(tq_vec2i coord);

void tq_on_rc_create(int rc);
void tq_on_rc_destroy(void);

//------------------------------------------------------------------------------

#endif // TQ_GRAPHICS_H_INC
