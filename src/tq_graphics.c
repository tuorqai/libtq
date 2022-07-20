
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

#define MAX_MATRICES 32

enum
{
    COLOR_CLEAR,
    COLOR_POINT,
    COLOR_LINE,
    COLOR_OUTLINE,
    COLOR_FILL,
    COLOR_COUNT,
};

struct matrices
{
    float       default_projection[16];
    float       model_view[MAX_MATRICES][9];
    uint32_t    model_view_index;
};

struct color
{
    tq_color_t direct;
};

static struct renderer_impl renderer;
static struct matrices matrices;
static struct color colors[COLOR_COUNT];

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

static float *make_circle(float x, float y, float radius, int color_id, int *length)
{
    float e = 0.25f;
    float angle = acosf(2.0f * (1.0f - e / radius) * (1.0f - e / radius) - 1.0f);

    int count = (int) ceilf(2.0f * M_PI / angle);
    float *data = mem_malloc(2 * sizeof(float) * count);

    for (int v = 0; v < count; v++) {
        data[2 * v + 0] = x + (radius * cos(v * angle));
        data[2 * v + 1] = y + (radius * sin(v * angle));
    }

    *length = count;
    return data;
}

//------------------------------------------------------------------------------

void tq_graphics_initialize(void)
{
#if defined(TQ_USE_OPENGL)
    construct_gl_renderer(&renderer);
#else
    #error Invalid configuration. Check your build settings.
#endif

    uint32_t width = tq_core_get_display_width();
    uint32_t height = tq_core_get_display_height();

    make_default_projection(matrices.default_projection, width, height);

    for (int index = 0; index < MAX_MATRICES; index++) {
        mat3_identity(matrices.model_view[index]);
    }
    
    matrices.model_view_index = 0;

    graphics_set_clear_color(TQ_COLOR24(0, 0, 0));
    graphics_set_point_color(TQ_COLOR24(255, 255, 255));
    graphics_set_line_color(TQ_COLOR24(255, 255, 255));
    graphics_set_outline_color(TQ_COLOR24(255, 255, 255));
    graphics_set_fill_color(TQ_COLOR24(0, 0, 0));

    renderer.initialize();

    renderer.update_viewport(0, 0, width, height);
    renderer.update_projection(matrices.default_projection);
    renderer.update_model_view(matrices.model_view[0]);

    text_initialize(&renderer);
}

void tq_graphics_terminate(void)
{
    text_terminate();
    renderer.terminate();
}

void tq_graphics_process(void)
{
    renderer.process();

    matrices.model_view_index = 0;
    mat3_identity(matrices.model_view[0]);

    renderer.update_projection(matrices.default_projection);
    renderer.update_model_view(matrices.model_view[0]);
}

void graphics_clear(void)
{
    renderer.clear();
}

tq_color_t graphics_get_clear_color(void)
{
    return colors[COLOR_CLEAR].direct;
}

void graphics_set_clear_color(tq_color_t clear_color)
{
    colors[COLOR_CLEAR].direct = clear_color;
    renderer.set_clear_color(clear_color);
}

void tq_graphics_view(float x, float y, float w, float h, float rotation)
{
    float projection[16];

    make_projection(projection, x, y, w, h, rotation);
    renderer.update_projection(projection);
}

void graphics_push_matrix(void)
{
    unsigned int index = matrices.model_view_index;

    if (index == (MAX_MATRICES - 1)) {
        return;
    }

    mat3_copy(matrices.model_view[index + 1], matrices.model_view[index]);

    matrices.model_view_index++;
    renderer.update_model_view(matrices.model_view[matrices.model_view_index]);
}

void graphics_pop_matrix(void)
{
    if (matrices.model_view_index == 0) {
        return;
    }

    matrices.model_view_index--;
    renderer.update_model_view(matrices.model_view[matrices.model_view_index]);
}

void graphics_translate_matrix(float x, float y)
{
    unsigned int index = matrices.model_view_index;
    mat3_translate(matrices.model_view[index], x, y);
    renderer.update_model_view(matrices.model_view[index]);
}

void graphics_scale_matrix(float x, float y)
{
    unsigned int index = matrices.model_view_index;
    mat3_scale(matrices.model_view[index], x, y);
    renderer.update_model_view(matrices.model_view[index]);
}

void graphics_rotate_matrix(float a)
{
    unsigned int index = matrices.model_view_index;
    mat3_rotate(matrices.model_view[index], RADIANS(a));
    renderer.update_model_view(matrices.model_view[index]);
}

void graphics_draw_point(float x, float y)
{
    float data[] = {
        x, y,
    };

    renderer.set_draw_color(colors[COLOR_POINT].direct);
    renderer.draw_solid(PRIMITIVE_POINTS, data, 1);
}

void graphics_draw_line(float ax, float ay, float bx, float by)
{
    float data[] = {
        ax, ay,
        bx, by,
    };

    renderer.set_draw_color(colors[COLOR_LINE].direct);
    renderer.draw_solid(PRIMITIVE_LINE_STRIP, data, 2);
}

void graphics_draw_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    graphics_fill_triangle(ax, ay, bx, by, cx, cy);
    graphics_outline_triangle(ax, ay, bx, by, cx, cy);
}

void graphics_draw_rectangle(float x, float y, float w, float h)
{
    graphics_fill_rectangle(x, y, w, h);
    graphics_outline_rectangle(x, y, w, h);
}

void graphics_draw_circle(float x, float y, float radius)
{
    graphics_fill_circle(x, y, radius);
    graphics_outline_circle(x, y, radius);
}

void graphics_outline_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = {
        ax, ay,
        bx, by,
        cx, cy,
    };

    renderer.set_draw_color(colors[COLOR_OUTLINE].direct);
    renderer.draw_solid(PRIMITIVE_LINE_LOOP, data, 3);
}

void graphics_outline_rectangle(float x, float y, float w, float h)
{
    float data[] = {
        x,      y,
        x + w,  y,
        x + w,  y + h,
        x,      y + h,
    };

    renderer.set_draw_color(colors[COLOR_OUTLINE].direct);
    renderer.draw_solid(PRIMITIVE_LINE_LOOP, data, 4);
}

void graphics_outline_circle(float x, float y, float radius)
{
    int length;

    float *data = make_circle(x, y, radius, COLOR_OUTLINE, &length);

    renderer.set_draw_color(colors[COLOR_OUTLINE].direct);
    renderer.draw_solid(PRIMITIVE_LINE_LOOP, data, length);

    mem_free(data);
}

void graphics_fill_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = {
        ax, ay,
        bx, by,
        cx, cy,
    };

    renderer.set_draw_color(colors[COLOR_FILL].direct);
    renderer.draw_solid(PRIMITIVE_TRIANGLE_FAN, data, 3);
}

void graphics_fill_rectangle(float x, float y, float w, float h)
{
    float data[] = {
        x,      y,
        x + w,  y,
        x + w,  y + h,
        x,      y + h,
    };

    renderer.set_draw_color(colors[COLOR_FILL].direct);
    renderer.draw_solid(PRIMITIVE_TRIANGLE_FAN, data, 4);
}

void graphics_fill_circle(float x, float y, float radius)
{
    int length;

    float *data = make_circle(x, y, radius, COLOR_FILL, &length);

    renderer.set_draw_color(colors[COLOR_FILL].direct);
    renderer.draw_solid(PRIMITIVE_TRIANGLE_FAN, data, length - 1);

    mem_free(data);
}

tq_color_t graphics_get_point_color(void)
{
    return colors[COLOR_POINT].direct;
}

void graphics_set_point_color(tq_color_t point_color)
{
    colors[COLOR_POINT].direct = point_color;
}

tq_color_t graphics_get_line_color(void)
{
    return colors[COLOR_LINE].direct;
}

void graphics_set_line_color(tq_color_t line_color)
{
    colors[COLOR_LINE].direct = line_color;
}

tq_color_t graphics_get_outline_color(void)
{
    return colors[COLOR_OUTLINE].direct;
}

void graphics_set_outline_color(tq_color_t outline_color)
{
    colors[COLOR_OUTLINE].direct = outline_color;
}

tq_color_t graphics_get_fill_color(void)
{
    return colors[COLOR_FILL].direct;
}

void graphics_set_fill_color(tq_color_t fill_color)
{
    colors[COLOR_FILL].direct = fill_color;
}

int graphics_load_texture(int stream_id)
{
    struct image image = image_load(stream_id);
    input_stream_close(stream_id);

    if (!image.pixels) {
        return -1;
    }

    int texture_id = renderer.create_texture(image.width, image.height, image.channels);

    if (texture_id == -1) {
        mem_free(image.pixels);
        return -1;
    }

    renderer.update_texture(texture_id, 0, 0, -1, -1, image.pixels);
    mem_free(image.pixels);

    return texture_id;
}

int graphics_load_texture_from_file(char const *path)
{
    return graphics_load_texture(open_file_input_stream(path));
}

int graphics_load_texture_from_memory(void const *buffer, size_t size)
{
    return graphics_load_texture(open_memory_input_stream(buffer, size));
}

void graphics_delete_texture(int texture_id)
{
    renderer.delete_texture(texture_id);
}

void graphics_get_texture_size(int texture_id, int *width, int *height)
{
    renderer.get_texture_size(texture_id, width, height);
}

void graphics_draw_texture(int texture_id,
    float x, float y,
    float w, float h)
{
    float data[] = {
        x,      y,      0.0f,   0.0f,
        x + w,  y,      1.0f,   0.0f,
        x + w,  y + h,  1.0f,   1.0f,
        x,      y + h,  0.0f,   1.0f,
    };

    renderer.bind_texture(texture_id);
    renderer.draw_textured(PRIMITIVE_TRIANGLE_FAN, data, 4);
}

void graphics_draw_texture_fragment(int texture_id,
    float x, float y,
    float w, float h,
    float fx, float fy,
    float fw, float fh)
{
    int u, v;
    renderer.get_texture_size(texture_id, &u, &v);

    float data[] = {
        x,      y,      fx / u,         fy / v,
        x + w,  y,      (fx + fw) / u,  fy / v,
        x + w,  y + h,  (fx + fw) / u,  (fy + fh) / v,
        x,      y + h,  fx / u,         (fy + fh) / v,
    };

    renderer.bind_texture(texture_id);
    renderer.draw_textured(PRIMITIVE_TRIANGLE_FAN, data, 4);
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

    make_default_projection(matrices.default_projection, width, height);
    renderer.update_viewport(0, 0, (int) width, (int) height);
}

//------------------------------------------------------------------------------
