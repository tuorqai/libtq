
//------------------------------------------------------------------------------

#include <math.h>
#include <string.h>

#include "tq_core.h"
#include "tq_graphics.h"
#include "tq_file.h"
#include "tq_math.h"
#include "tq_stream.h"

//------------------------------------------------------------------------------
// Declarations

#define MAX_MATRICES 32

typedef struct tq_graphics_priv
{
    tq_renderer_t const *renderer;

    float           default_projection[16];
    float           model_view[MAX_MATRICES][9];
    uint32_t        model_view_index;

    tq_color_t      clear_color;
    tq_color_t      point_color;
    tq_color_t      line_color;
    tq_color_t      outline_color;
    tq_color_t      fill_color;
} tq_graphics_priv_t;

//------------------------------------------------------------------------------
// Definitions

static tq_graphics_priv_t graphics;

//------------------------------------------------------------------------------
// Utility functions

static void make_projection(float *dst, float x, float y, float w, float h, float rotation)
{
    float left      = x - (w / 2.0f);
    float right     = x + (w / 2.0f);
    float bottom    = y + (h / 2.0f);
    float top       = y - (h / 2.0f);

    mat4_ortho(dst, left, right, bottom, top, -1.0f, +1.0f);

    if (rotation != 0.0f) {
        mat4_translate(dst, x, y, 0.0f);
        mat4_rotate(dst, RADIANS(rotation), 0.0f, 0.0f, 1.0f);
        mat4_translate(dst, -x, -y, 0.0f);
    }
}

static void make_default_projection(float *dst, uint32_t w, uint32_t h)
{
    mat4_ortho(dst, 0.0f, (float) w, (float) h, 0.0f, -1.0f, +1.0f);
}

static float *make_circle(float x, float y, float radius, unsigned int *length)
{
    float e = 0.25f;
    float angle = acosf(2.0f * (1.0f - e / radius) * (1.0f - e / radius) - 1.0f);

    *length = (unsigned int) (ceilf(2.0f * M_PI / angle));
    float *data = malloc(2 * sizeof(float) * (*length));

    for (unsigned int v = 0; v < (*length); v++) {
        data[2 * v + 0] = x + (radius * cos(v * angle));
        data[2 * v + 1] = y + (radius * sin(v * angle));
    }

    return data;
}

//------------------------------------------------------------------------------

void graphics_initialize(tq_renderer_t const *renderer)
{
    graphics.renderer = renderer;

    make_default_projection(graphics.default_projection,
        core_get_display_width(),
        core_get_display_height());

    for (int index = 0; index < MAX_MATRICES; index++) {
        mat3_identity(graphics.model_view[index]);
    }
    
    graphics.model_view_index = 0;

    graphics.clear_color = tq_rgb(128, 128, 128);
    graphics.point_color = tq_rgb(255, 0, 255);
    graphics.line_color = tq_rgb(255, 0, 255);
    graphics.outline_color = tq_rgb(255, 0, 255);
    graphics.fill_color = tq_rgb(0, 255, 255);

    graphics.renderer->initialize();

    graphics.renderer->update_viewport(0, 0, 200, 200);
    graphics.renderer->update_projection(graphics.default_projection);
    graphics.renderer->update_model_view(graphics.model_view[0]);

    graphics.renderer->set_clear_color(graphics.clear_color);
    graphics.renderer->set_point_color(graphics.point_color);
    graphics.renderer->set_line_color(graphics.line_color);
    graphics.renderer->set_outline_color(graphics.outline_color);
    graphics.renderer->set_fill_color(graphics.fill_color);
}

void graphics_terminate(void)
{
    graphics.renderer->terminate();
}

void tq_graphics_finish(void)
{
    graphics.renderer->flush();

    graphics.model_view_index = 0;
    mat3_identity(graphics.model_view[0]);

    graphics.renderer->update_projection(graphics.default_projection);
    graphics.renderer->update_model_view(graphics.model_view[0]);
}

void graphics_clear(void)
{
    graphics.renderer->clear();
}

tq_color_t graphics_get_clear_color(void)
{
    return graphics.clear_color;
}

void graphics_set_clear_color(tq_color_t clear_color)
{
    graphics.clear_color = clear_color;
    graphics.renderer->set_clear_color(graphics.clear_color);
}

void tq_graphics_view(float x, float y, float w, float h, float rotation)
{
    float projection[16];

    make_projection(projection, x, y, w, h, rotation);
    graphics.renderer->update_projection(projection);
}

void graphics_push_matrix(void)
{
    unsigned int index = graphics.model_view_index;

    if (index == (MAX_MATRICES - 1)) {
        return;
    }

    mat3_copy(graphics.model_view[index + 1], graphics.model_view[index]);

    graphics.model_view_index++;
    graphics.renderer->update_model_view(graphics.model_view[graphics.model_view_index]);
}

void graphics_pop_matrix(void)
{
    if (graphics.model_view_index == 0) {
        return;
    }

    graphics.model_view_index--;
    graphics.renderer->update_model_view(graphics.model_view[graphics.model_view_index]);
}

void graphics_translate_matrix(float x, float y)
{
    unsigned int index = graphics.model_view_index;
    mat3_translate(graphics.model_view[index], x, y);
    graphics.renderer->update_model_view(graphics.model_view[index]);
}

void graphics_scale_matrix(float x, float y)
{
    unsigned int index = graphics.model_view_index;
    mat3_scale(graphics.model_view[index], x, y);
    graphics.renderer->update_model_view(graphics.model_view[index]);
}

void graphics_rotate_matrix(float a)
{
    unsigned int index = graphics.model_view_index;
    mat3_rotate(graphics.model_view[index], RADIANS(a));
    graphics.renderer->update_model_view(graphics.model_view[index]);
}

void graphics_draw_point(float x, float y)
{
    float data[] = { x, y };
    graphics.renderer->draw_points(data, 1);
}

void graphics_draw_line(float ax, float ay, float bx, float by)
{
    float data[] = { ax, ay, bx, by };
    graphics.renderer->draw_lines(data, 2);
}

void graphics_draw_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = { ax, ay, bx, by, cx, cy };
    graphics.renderer->draw_fill(data, 3);
    graphics.renderer->draw_outline(data, 3);
}

void graphics_draw_rectangle(float x, float y, float w, float h)
{
    float data[] = { x, y, x + w, y, x + w, y + h, x, y + h };
    graphics.renderer->draw_fill(data, 4);
    graphics.renderer->draw_outline(data, 4);
}

void graphics_draw_circle(float x, float y, float radius)
{
    unsigned int length;
    float *data = make_circle(x, y, radius, &length);
    graphics.renderer->draw_fill(data, length);
    graphics.renderer->draw_outline(data, length);
    free(data);
}

void graphics_outline_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = { ax, ay, bx, by, cx, cy };
    graphics.renderer->draw_outline(data, 3);
}

void graphics_outline_rectangle(float x, float y, float w, float h)
{
    float data[] = { x, y, x + w, y, x + w, y + h, x, y + h };
    graphics.renderer->draw_outline(data, 4);
}

void graphics_outline_circle(float x, float y, float radius)
{
    unsigned int length;
    float *data = make_circle(x, y, radius, &length);
    graphics.renderer->draw_outline(data, length);
    free(data);
}

void graphics_fill_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = { ax, ay, bx, by, cx, cy };
    graphics.renderer->draw_fill(data, 3);
}

void graphics_fill_rectangle(float x, float y, float w, float h)
{
    float data[] = { x, y, x + w, y, x + w, y + h, x, y + h };
    graphics.renderer->draw_fill(data, 4);
}

void graphics_fill_circle(float x, float y, float radius)
{
    unsigned int length;
    float *data = make_circle(x, y, radius, &length);
    graphics.renderer->draw_fill(data, length);
    free(data);
}

tq_color_t graphics_get_point_color(void)
{
    return graphics.point_color;
}

void graphics_set_point_color(tq_color_t point_color)
{
    graphics.point_color = point_color;
    graphics.renderer->set_point_color(graphics.point_color);
}

tq_color_t graphics_get_line_color(void)
{
    return graphics.line_color;
}

void graphics_set_line_color(tq_color_t line_color)
{
    graphics.line_color = line_color;
    graphics.renderer->set_line_color(graphics.line_color);
}

tq_color_t graphics_get_outline_color(void)
{
    return graphics.outline_color;
}

void graphics_set_outline_color(tq_color_t outline_color)
{
    graphics.outline_color = outline_color;
    graphics.renderer->set_outline_color(graphics.outline_color);
}

tq_color_t graphics_get_fill_color(void)
{
    return graphics.fill_color;
}

void graphics_set_fill_color(tq_color_t fill_color)
{
    graphics.fill_color = fill_color;
    graphics.renderer->set_fill_color(graphics.fill_color);
}

tq_handle_t graphics_load_texture_from_file(char const *path)
{
    stream_t stream;

    if (file_stream_open(&stream, path) == -1) {
        return TQ_INVALID_HANDLE;
    }

    tq_handle_t texture_id = graphics.renderer->load_texture(&stream);

    stream.close(stream.data);
    return texture_id;
}

tq_handle_t graphics_load_texture_from_memory(uint8_t const *buffer, size_t length)
{
    stream_t stream;

    if (memory_stream_open(&stream, buffer, length) == -1) {
        return TQ_INVALID_HANDLE;
    }

    tq_handle_t texture_id = graphics.renderer->load_texture(&stream);

    stream.close(stream.data);
    return texture_id;
}

void graphics_delete_texture(tq_handle_t texture_handle)
{
    graphics.renderer->delete_texture(texture_handle);
}

void graphics_get_texture_size(tq_handle_t texture_handle, uint32_t *width, uint32_t *height)
{
    graphics.renderer->get_texture_size(texture_handle, width, height);
}

void graphics_draw_texture(tq_handle_t texture_id,
    float x, float y,
    float w, float h)
{
    uint32_t u, v;
    graphics.renderer->get_texture_size(texture_id, &u, &v);

    float data[] = {
        x,      y,      0.0f,   0.0f,
        x + w,  y,      u,      0.0f,
        x + w,  y + h,  u,      v,
        x,      y + h,  0.0f,   v,
    };

    graphics.renderer->draw_texture(texture_id, data, 4);
}

void graphics_draw_texture_fragment(tq_handle_t texture_id,
    float x, float y,
    float w, float h,
    float u0, float v0,
    float u1, float v1)
{
    float data[] = {
        x,      y,      u0,     v0,
        x + w,  y,      u1,     v0,
        x + w,  y + h,  u1,     v1,
        x,      y + h,  u0,     v1,
    };

    graphics.renderer->draw_texture(texture_id, data, 4);
}

//--------------------------------------
// tq::graphics::on_display_resized
//--------------------------------------
void tq_graphics_on_display_resized(uint32_t width, uint32_t height)
{
    // This function is called when the user resizes the window.
    // Here we just replace the default projection with the new one
    // and tell the renderer to update its viewport
    // (basically call glViewport).

    make_default_projection(graphics.default_projection, width, height);
    graphics.renderer->update_viewport(0, 0, (int) width, (int) height);
}

//------------------------------------------------------------------------------
