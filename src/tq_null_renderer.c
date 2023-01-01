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

#include "tq_graphics.h"

//------------------------------------------------------------------------------

static void     initialize(void);
static void     terminate(void);
static void     process(void);
static void     post_process(void);

static int      request_antialiasing_level(int level);

static void     update_projection(float const *mat4);
static void     update_model_view(float const *mat3);

static int      create_texture(int width, int height, int channels);
static void     delete_texture(int texture_id);
static bool     is_texture_smooth(int texture_id);
static void     set_texture_smooth(int texture_id, bool smooth);
static void     get_texture_size(int texture_id, int *width, int *height);
static void     update_texture(int texture_id, int x_offset, int y_offset,
                               int width, int height, unsigned char *pixels);
static void     bind_texture(int texture_id);

static int      create_surface(int width, int height);
static void     delete_surface(int surface_id);
static int      get_surface_texture_id(int surface_id);
static void     bind_surface(int surface_id);

static void     set_clear_color(tq_color color);
static void     set_draw_color(tq_color color);
static void     set_blend_mode(tq_blend_mode mode);

static void     clear(void);
static void     draw_solid(int mode, float const *data, int num_vertices);
static void     draw_colored(int mode, float const *data, int num_vertices);
static void     draw_textured(int mode, float const *data, int num_vertices);
static void     draw_font(float const *data, int num_vertices);
static void     draw_canvas(float x0, float y0, float x1, float y1);

//------------------------------------------------------------------------------

void initialize(void)
{
}

void terminate(void)
{
}

void process(void)
{
}

void post_process(void)
{
}

int request_antialiasing_level(int level)
{
    return 0;
}

void update_projection(float const *mat4)
{
}

void update_model_view(float const *mat3)
{
}

int create_texture(int width, int height, int channels)
{
    return 0;
}

void delete_texture(int texture_id)
{
}

bool is_texture_smooth(int texture_id)
{
    return false;
}

void set_texture_smooth(int texture_id, bool smooth)
{
}

void get_texture_size(int texture_id, int *width, int *height)
{
}

void update_texture(int texture_id, int x_offset, int y_offset,
                    int width, int height, unsigned char *pixels)
{
}

void bind_texture(int texture_id)
{
}

int create_surface(int width, int height)
{
    return 0;
}

void delete_surface(int surface_id)
{
}

int get_surface_texture_id(int surface_id)
{
    return 0;
}

void bind_surface(int surface_id)
{
}

void set_clear_color(tq_color color)
{
}

void set_draw_color(tq_color color)
{
}

void set_blend_mode(tq_blend_mode mode)
{
}

void clear(void)
{
}

void draw_solid(int mode, float const *data, int num_vertices)
{
}

void draw_colored(int mode, float const *data, int num_vertices)
{
}

void draw_textured(int mode, float const *data, int num_vertices)
{
}

void draw_font(float const *data, int num_vertices)
{
}

void draw_canvas(float x0, float y0, float x1, float y1)
{
}

//------------------------------------------------------------------------------

void tq_construct_null_renderer(tq_renderer_impl *impl)
{
    *impl = (tq_renderer_impl) {
        .initialize             = initialize,
        .terminate              = terminate,
        .process                = process,
        .post_process           = post_process,
        .request_antialiasing_level
                                = request_antialiasing_level,
        .update_projection      = update_projection,
        .update_model_view      = update_model_view,
        .create_texture         = create_texture,
        .delete_texture         = delete_texture,
        .is_texture_smooth      = is_texture_smooth,
        .set_texture_smooth     = set_texture_smooth,
        .get_texture_size       = get_texture_size,
        .update_texture         = update_texture,
        .bind_texture           = bind_texture,
        .create_surface         = create_surface,
        .delete_surface         = delete_surface,
        .get_surface_texture_id = get_surface_texture_id,
        .bind_surface           = bind_surface,
        .set_clear_color        = set_clear_color,
        .set_draw_color         = set_draw_color,
        .set_blend_mode         = set_blend_mode,
        .clear                  = clear,
        .draw_solid             = draw_solid,
        .draw_colored           = draw_colored,
        .draw_textured          = draw_textured,
        .draw_font              = draw_font,
        .draw_canvas            = draw_canvas,
    };
}

//------------------------------------------------------------------------------
