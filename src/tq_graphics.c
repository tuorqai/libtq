
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

typedef struct tq_graphics
{
    tq_renderer_t   renderer;

    float           default_projection[16];
    float           model_view[MAX_MATRICES][9];
    uint32_t        model_view_index;
} tq_graphics_t;

struct color
{
    tq_color_t direct;
    float normalized[4];
};

static tq_graphics_t graphics;
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

    int count = (int) (ceilf(2.0f * M_PI / angle)) + 1;
    float *data = mem_malloc(6 * sizeof(float) * count);

    for (int v = 0; v < count - 1; v++) {
        data[6 * v + 0] = x + (radius * cos(v * angle));
        data[6 * v + 1] = y + (radius * sin(v * angle));
        data[6 * v + 2] = colors[color_id].normalized[0];
        data[6 * v + 3] = colors[color_id].normalized[1];
        data[6 * v + 4] = colors[color_id].normalized[2];
        data[6 * v + 5] = colors[color_id].normalized[3];
    }

    data[6 * (count - 1) + 0] = data[0];
    data[6 * (count - 1) + 1] = data[1];
    data[6 * (count - 1) + 2] = data[2];
    data[6 * (count - 1) + 3] = data[3];
    data[6 * (count - 1) + 4] = data[4];
    data[6 * (count - 1) + 5] = data[5];

    *length = count;
    return data;
}

static void decode_color24(float *dst, tq_color_t color)
{
    dst[0] = ((color >> 24) & 255) / 255.0f;
    dst[1] = ((color >> 16) & 255) / 255.0f;
    dst[2] = ((color >>  8) & 255) / 255.0f;
    dst[3] = 1.0f;
}

static void decode_color32(float *dst, tq_color_t color)
{
    dst[0] = ((color >> 24) & 255) / 255.0f;
    dst[1] = ((color >> 16) & 255) / 255.0f;
    dst[2] = ((color >>  8) & 255) / 255.0f;
    dst[3] = ((color <<  0) & 255) / 255.0f;
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

    graphics_set_clear_color(TQ_COLOR24(0, 0, 0));
    graphics_set_point_color(TQ_COLOR24(255, 255, 255));
    graphics_set_line_color(TQ_COLOR24(255, 255, 255));
    graphics_set_outline_color(TQ_COLOR24(255, 255, 255));
    graphics_set_fill_color(TQ_COLOR24(0, 0, 0));

    graphics.renderer.initialize();

    graphics.renderer.update_viewport(0, 0, width, height);
    graphics.renderer.update_projection(graphics.default_projection);
    graphics.renderer.update_model_view(graphics.model_view[0]);

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
    graphics.renderer.clear(
        colors[COLOR_CLEAR].normalized[0],
        colors[COLOR_CLEAR].normalized[1],
        colors[COLOR_CLEAR].normalized[2]
    );
}

tq_color_t graphics_get_clear_color(void)
{
    return colors[COLOR_CLEAR].direct;
}

void graphics_set_clear_color(tq_color_t clear_color)
{
    decode_color24(colors[COLOR_CLEAR].normalized, clear_color);
    colors[COLOR_CLEAR].direct = clear_color;
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
    float r = colors[COLOR_POINT].normalized[0];
    float g = colors[COLOR_POINT].normalized[1];
    float b = colors[COLOR_POINT].normalized[2];
    float a = colors[COLOR_POINT].normalized[3];

    float data[] = {
        x, y, r, g, b, a,
    };

    graphics.renderer.draw_solid(RENDERER_MODE_POINTS, data, 1);
}

void graphics_draw_line(float ax, float ay, float bx, float by)
{
    float r = colors[COLOR_LINE].normalized[0];
    float g = colors[COLOR_LINE].normalized[1];
    float b = colors[COLOR_LINE].normalized[2];
    float a = colors[COLOR_LINE].normalized[3];

    float data[] = {
        ax, ay, r, g, b, a,
        bx, by, r, g, b, a,
    };

    graphics.renderer.draw_solid(RENDERER_MODE_LINE_STRIP, data, 2);
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
    float r0 = colors[COLOR_OUTLINE].normalized[0];
    float g0 = colors[COLOR_OUTLINE].normalized[1];
    float b0 = colors[COLOR_OUTLINE].normalized[2];
    float a0 = colors[COLOR_OUTLINE].normalized[3];

    float data[] = {
        ax, ay, r0, g0, b0, a0,
        bx, by, r0, g0, b0, a0,
        cx, cy, r0, g0, b0, a0,
        ax, ay, r0, g0, b0, a0,
    };

    graphics.renderer.draw_solid(RENDERER_MODE_LINE_STRIP, data, 4);
}

void graphics_outline_rectangle(float x, float y, float w, float h)
{
    float r0 = colors[COLOR_OUTLINE].normalized[0];
    float g0 = colors[COLOR_OUTLINE].normalized[1];
    float b0 = colors[COLOR_OUTLINE].normalized[2];
    float a0 = colors[COLOR_OUTLINE].normalized[3];

    float data[] = {
        x,      y,      r0, g0, b0, a0,
        x + w,  y,      r0, g0, b0, a0,
        x + w,  y + h,  r0, g0, b0, a0,
        x,      y + h,  r0, g0, b0, a0,
        x,      y,      r0, g0, b0, a0,
    };

    graphics.renderer.draw_solid(RENDERER_MODE_LINE_STRIP, data, 5);
}

void graphics_outline_circle(float x, float y, float radius)
{
    int length;

    float *data = make_circle(x, y, radius, COLOR_OUTLINE, &length);
    graphics.renderer.draw_solid(RENDERER_MODE_LINE_STRIP, data, length);

    mem_free(data);
}

void graphics_fill_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float r0 = colors[COLOR_FILL].normalized[0];
    float g0 = colors[COLOR_FILL].normalized[1];
    float b0 = colors[COLOR_FILL].normalized[2];
    float a0 = colors[COLOR_FILL].normalized[3];

    float data[] = {
        ax, ay, r0, g0, b0, a0, 
        bx, by, r0, g0, b0, a0,
        cx, cy, r0, g0, b0, a0,
    };

    graphics.renderer.draw_solid(RENDERER_MODE_TRIANGLE_FAN, data, 3);
}

void graphics_fill_rectangle(float x, float y, float w, float h)
{
    float r0 = colors[COLOR_FILL].normalized[0];
    float g0 = colors[COLOR_FILL].normalized[1];
    float b0 = colors[COLOR_FILL].normalized[2];
    float a0 = colors[COLOR_FILL].normalized[3];

    float data[] = {
        x,      y,      r0, g0, b0, a0, 
        x + w,  y,      r0, g0, b0, a0,
        x + w,  y + h,  r0, g0, b0, a0,
        x,      y + h,  r0, g0, b0, a0,
    };

    graphics.renderer.draw_solid(RENDERER_MODE_TRIANGLE_FAN, data, 4);
}

void graphics_fill_circle(float x, float y, float radius)
{
    int length;

    float *data = make_circle(x, y, radius, COLOR_FILL, &length);
    graphics.renderer.draw_solid(RENDERER_MODE_TRIANGLE_FAN, data, length - 1);

    mem_free(data);
}

tq_color_t graphics_get_point_color(void)
{
    return colors[COLOR_POINT].direct;
}

void graphics_set_point_color(tq_color_t point_color)
{
    decode_color32(colors[COLOR_POINT].normalized, point_color);
    colors[COLOR_POINT].direct = point_color;
}

tq_color_t graphics_get_line_color(void)
{
    return colors[COLOR_LINE].direct;
}

void graphics_set_line_color(tq_color_t line_color)
{
    decode_color32(colors[COLOR_LINE].normalized, line_color);
    colors[COLOR_LINE].direct = line_color;
}

tq_color_t graphics_get_outline_color(void)
{
    return colors[COLOR_OUTLINE].direct;
}

void graphics_set_outline_color(tq_color_t outline_color)
{
    decode_color32(colors[COLOR_OUTLINE].normalized, outline_color);
    colors[COLOR_OUTLINE].direct = outline_color;
}

tq_color_t graphics_get_fill_color(void)
{
    return colors[COLOR_FILL].direct;
}

void graphics_set_fill_color(tq_color_t fill_color)
{
    decode_color32(colors[COLOR_FILL].normalized, fill_color);
    colors[COLOR_FILL].direct = fill_color;
}

int graphics_load_texture(int stream_id)
{
    struct image image = image_load(stream_id);
    input_stream_close(stream_id);

    if (!image.pixels) {
        return -1;
    }

    int texture_id = graphics.renderer.create_texture(image.width, image.height, image.channels);

    if (texture_id == -1) {
        mem_free(image.pixels);
        return -1;
    }

    graphics.renderer.update_texture(texture_id, 0, 0, -1, -1, image.pixels);
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
    graphics.renderer.delete_texture(texture_id);
}

void graphics_get_texture_size(int texture_id, int *width, int *height)
{
    graphics.renderer.get_texture_size(texture_id, width, height);
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

    graphics.renderer.bind_texture(texture_id);
    graphics.renderer.draw_textured(RENDERER_MODE_TRIANGLE_FAN, data, 4);
}

void graphics_draw_texture_fragment(int texture_id,
    float x, float y,
    float w, float h,
    float fx, float fy,
    float fw, float fh)
{
    int u, v;
    graphics.renderer.get_texture_size(texture_id, &u, &v);

    float data[] = {
        x,      y,      fx / u,         fy / v,
        x + w,  y,      (fx + fw) / u,  fy / v,
        x + w,  y + h,  (fx + fw) / u,  (fy + fh) / v,
        x,      y + h,  fx / u,         (fy + fh) / v,
    };

    graphics.renderer.bind_texture(texture_id);
    graphics.renderer.draw_textured(RENDERER_MODE_TRIANGLE_FAN, data, 4);
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
