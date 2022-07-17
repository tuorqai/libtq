
//------------------------------------------------------------------------------

#include <math.h>
#include <string.h>

#include <hb-ft.h>

#include "tq_core.h"
#include "tq_error.h"
#include "tq_graphics.h"
#include "tq_math.h"
#include "tq_mem.h"
#include "tq_log.h"
#include "tq_stream.h"
#include "tq_text.h"

//------------------------------------------------------------------------------
// Declarations

#define MAX_MATRICES 32

typedef struct tq_graphics
{
    tq_renderer_t   renderer;

    float           default_projection[16];
    float           model_view[MAX_MATRICES][9];
    uint32_t        model_view_index;

    tq_color_t      clear_color;
    tq_color_t      point_color;
    tq_color_t      line_color;
    tq_color_t      outline_color;
    tq_color_t      fill_color;
} tq_graphics_t;

//------------------------------------------------------------------------------
// Definitions

static tq_graphics_t graphics;

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

void tq_graphics_initialize(void)
{
#if defined(TQ_USE_OPENGL)
    tq_construct_gl_renderer(&graphics.renderer);
#else
    #error Invalid configuration. Check your build settings.
#endif

    uint32_t width = tq_core_get_display_width();
    uint32_t height = tq_core_get_display_height();

    make_default_projection(graphics.default_projection, width, height);

    for (int index = 0; index < MAX_MATRICES; index++) {
        mat3_identity(graphics.model_view[index]);
    }
    
    graphics.model_view_index = 0;

    graphics.clear_color = TQ_COLOR24(29, 43, 83);
    graphics.point_color = TQ_COLOR24(255, 241, 232);
    graphics.line_color = TQ_COLOR24(255, 236, 39);
    graphics.outline_color = TQ_COLOR24(255, 204, 170);
    graphics.fill_color = TQ_COLOR24(126, 37, 83);

    graphics.renderer.initialize();

    graphics.renderer.update_viewport(0, 0, width, height);
    graphics.renderer.update_projection(graphics.default_projection);
    graphics.renderer.update_model_view(graphics.model_view[0]);

    graphics.renderer.set_clear_color(graphics.clear_color);
    graphics.renderer.set_point_color(graphics.point_color);
    graphics.renderer.set_line_color(graphics.line_color);
    graphics.renderer.set_outline_color(graphics.outline_color);
    graphics.renderer.set_fill_color(graphics.fill_color);

    text_initialize(&graphics.renderer);
}

void tq_graphics_terminate(void)
{
    text_terminate();
    graphics.renderer.terminate();
}

void tq_graphics_process(void)
{
    graphics.renderer.process();

    graphics.model_view_index = 0;
    mat3_identity(graphics.model_view[0]);

    graphics.renderer.update_projection(graphics.default_projection);
    graphics.renderer.update_model_view(graphics.model_view[0]);
}

void graphics_clear(void)
{
    graphics.renderer.clear();
}

tq_color_t graphics_get_clear_color(void)
{
    return graphics.clear_color;
}

void graphics_set_clear_color(tq_color_t clear_color)
{
    graphics.clear_color = clear_color;
    graphics.renderer.set_clear_color(graphics.clear_color);
}

void tq_graphics_view(float x, float y, float w, float h, float rotation)
{
    float projection[16];

    make_projection(projection, x, y, w, h, rotation);
    graphics.renderer.update_projection(projection);
}

void graphics_push_matrix(void)
{
    unsigned int index = graphics.model_view_index;

    if (index == (MAX_MATRICES - 1)) {
        return;
    }

    mat3_copy(graphics.model_view[index + 1], graphics.model_view[index]);

    graphics.model_view_index++;
    graphics.renderer.update_model_view(graphics.model_view[graphics.model_view_index]);
}

void graphics_pop_matrix(void)
{
    if (graphics.model_view_index == 0) {
        return;
    }

    graphics.model_view_index--;
    graphics.renderer.update_model_view(graphics.model_view[graphics.model_view_index]);
}

void graphics_translate_matrix(float x, float y)
{
    unsigned int index = graphics.model_view_index;
    mat3_translate(graphics.model_view[index], x, y);
    graphics.renderer.update_model_view(graphics.model_view[index]);
}

void graphics_scale_matrix(float x, float y)
{
    unsigned int index = graphics.model_view_index;
    mat3_scale(graphics.model_view[index], x, y);
    graphics.renderer.update_model_view(graphics.model_view[index]);
}

void graphics_rotate_matrix(float a)
{
    unsigned int index = graphics.model_view_index;
    mat3_rotate(graphics.model_view[index], RADIANS(a));
    graphics.renderer.update_model_view(graphics.model_view[index]);
}

void graphics_draw_point(float x, float y)
{
    float data[] = { x, y };
    graphics.renderer.draw_points(data, 1);
}

void graphics_draw_line(float ax, float ay, float bx, float by)
{
    float data[] = { ax, ay, bx, by };
    graphics.renderer.draw_lines(data, 2);
}

void graphics_draw_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = { ax, ay, bx, by, cx, cy };
    graphics.renderer.draw_fill(data, 3);
    graphics.renderer.draw_outline(data, 3);
}

void graphics_draw_rectangle(float x, float y, float w, float h)
{
    float data[] = { x, y, x + w, y, x + w, y + h, x, y + h };
    graphics.renderer.draw_fill(data, 4);
    graphics.renderer.draw_outline(data, 4);
}

void graphics_draw_circle(float x, float y, float radius)
{
    unsigned int length;
    float *data = make_circle(x, y, radius, &length);
    graphics.renderer.draw_fill(data, length);
    graphics.renderer.draw_outline(data, length);
    free(data);
}

void graphics_outline_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = { ax, ay, bx, by, cx, cy };
    graphics.renderer.draw_outline(data, 3);
}

void graphics_outline_rectangle(float x, float y, float w, float h)
{
    float data[] = { x, y, x + w, y, x + w, y + h, x, y + h };
    graphics.renderer.draw_outline(data, 4);
}

void graphics_outline_circle(float x, float y, float radius)
{
    unsigned int length;
    float *data = make_circle(x, y, radius, &length);
    graphics.renderer.draw_outline(data, length);
    free(data);
}

void graphics_fill_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = { ax, ay, bx, by, cx, cy };
    graphics.renderer.draw_fill(data, 3);
}

void graphics_fill_rectangle(float x, float y, float w, float h)
{
    float data[] = { x, y, x + w, y, x + w, y + h, x, y + h };
    graphics.renderer.draw_fill(data, 4);
}

void graphics_fill_circle(float x, float y, float radius)
{
    unsigned int length;
    float *data = make_circle(x, y, radius, &length);
    graphics.renderer.draw_fill(data, length);
    free(data);
}

tq_color_t graphics_get_point_color(void)
{
    return graphics.point_color;
}

void graphics_set_point_color(tq_color_t point_color)
{
    graphics.point_color = point_color;
    graphics.renderer.set_point_color(graphics.point_color);
}

tq_color_t graphics_get_line_color(void)
{
    return graphics.line_color;
}

void graphics_set_line_color(tq_color_t line_color)
{
    graphics.line_color = line_color;
    graphics.renderer.set_line_color(graphics.line_color);
}

tq_color_t graphics_get_outline_color(void)
{
    return graphics.outline_color;
}

void graphics_set_outline_color(tq_color_t outline_color)
{
    graphics.outline_color = outline_color;
    graphics.renderer.set_outline_color(graphics.outline_color);
}

tq_color_t graphics_get_fill_color(void)
{
    return graphics.fill_color;
}

void graphics_set_fill_color(tq_color_t fill_color)
{
    graphics.fill_color = fill_color;
    graphics.renderer.set_fill_color(graphics.fill_color);
}

int32_t graphics_load_texture_from_file(char const *path)
{
    int32_t stream_id = tq_open_file_istream(path);

    if (stream_id == -1) {
        return -1;
    }

    int32_t texture_id = graphics.renderer.load_texture(stream_id);

    tq_istream_close(stream_id);

    return texture_id;
}

int32_t graphics_load_texture_from_memory(uint8_t const *buffer, size_t size)
{
    int32_t stream_id = tq_open_memory_istream(buffer, size);

    if (stream_id == -1) {
        return -1;
    }

    int32_t texture_id = graphics.renderer.load_texture(stream_id);

    tq_istream_close(stream_id);
    return texture_id;
}

void graphics_delete_texture(int32_t texture_handle)
{
    graphics.renderer.delete_texture(texture_handle);
}

void graphics_get_texture_size(int32_t texture_handle, uint32_t *width, uint32_t *height)
{
    graphics.renderer.get_texture_size(texture_handle, width, height);
}

void graphics_draw_texture(int32_t texture_id,
    float x, float y,
    float w, float h)
{
    uint32_t u, v;
    graphics.renderer.get_texture_size(texture_id, &u, &v);

    float data[] = {
        x,      y,      0.0f,   0.0f,
        x + w,  y,      u,      0.0f,
        x + w,  y + h,  u,      v,
        x,      y + h,  0.0f,   v,
    };

    graphics.renderer.draw_texture(texture_id, data, 4);
}

void graphics_draw_texture_fragment(int32_t texture_id,
    float x, float y,
    float w, float h,
    float fx, float fy,
    float fw, float fh)
{
    float data[] = {
        x,      y,      fx,         fy,
        x + w,  y,      fx + fw,    fy,
        x + w,  y + h,  fx + fw,    fy + fh,
        x,      y + h,  fx,         fy + fh,
    };

    graphics.renderer.draw_texture(texture_id, data, 4);
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
    graphics.renderer.update_viewport(0, 0, (int) width, (int) height);
}

//------------------------------------------------------------------------------
