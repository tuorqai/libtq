
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
    float       projection[16];
    float       model_view[MAX_MATRICES][9];
    int         current_model_view;

    float       inverse_projection[16];
    bool        dirty_inverse_projection;
};

struct color
{
    tq_color value;
};

static struct renderer_impl renderer;
static int display_width;
static int display_height;
static struct matrices matrices;
static struct color colors[COLOR_COUNT];
static bool auto_view_reset;

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

static void make_default_projection(float *dst, int w, int h)
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

static float const *get_inverse_projection(void)
{
    if (matrices.dirty_inverse_projection) {
        mat4_inverse(matrices.projection, matrices.inverse_projection);
        matrices.dirty_inverse_projection = false;
    }

    return matrices.inverse_projection;
}

//------------------------------------------------------------------------------

void tq_graphics_initialize(void)
{
#if defined(TQ_USE_OPENGL)
    construct_gl_renderer(&renderer);
#else
    #error Invalid configuration. Check your build settings.
#endif

    tq_core_get_display_size(&display_width, &display_height);

    make_default_projection(matrices.default_projection, display_width, display_height);
    mat4_copy(matrices.projection, matrices.default_projection);

    for (int index = 0; index < MAX_MATRICES; index++) {
        mat3_identity(matrices.model_view[index]);
    }
    
    matrices.current_model_view = 0;
    matrices.dirty_inverse_projection = true;

    graphics_set_clear_color(tq_c24(0, 0, 0));
    graphics_set_point_color(tq_c24(255, 255, 255));
    graphics_set_line_color(tq_c24(255, 255, 255));
    graphics_set_outline_color(tq_c24(255, 255, 255));
    graphics_set_fill_color(tq_c24(0, 0, 0));

    auto_view_reset = true;

    renderer.initialize();

    renderer.update_viewport(0, 0, display_width, display_height);
    renderer.update_projection(matrices.projection);
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

    mat3_identity(matrices.model_view[0]);
    renderer.update_model_view(matrices.model_view[0]);

    matrices.current_model_view = 0;

    if (auto_view_reset) {
        mat4_copy(matrices.projection, matrices.default_projection);
        renderer.update_projection(matrices.projection);
    }
}

//------------------------------------------------------------------------------

void graphics_clear(void)
{
    renderer.clear();
}

tq_color graphics_get_clear_color(void)
{
    return colors[COLOR_CLEAR].value;
}

void graphics_set_clear_color(tq_color clear_color)
{
    colors[COLOR_CLEAR].value = clear_color;
    renderer.set_clear_color(clear_color);
}

//------------------------------------------------------------------------------

void graphics_get_relative_position(float ax, float ay, float *x, float *y)
{
    float u = -1.0f + 2.0f * (ax / (float) display_width);
    float v = +1.0f - 2.0f * (ay / (float) display_height);

    mat4_transform_point(get_inverse_projection(), u, v, x, y);
}

void graphics_set_view(float x, float y, float w, float h, float rotation)
{
    make_projection(matrices.projection, x, y, w, h, rotation);
    renderer.update_projection(matrices.projection);

    matrices.dirty_inverse_projection = true;
}

void graphics_reset_view(void)
{
    mat4_copy(matrices.projection, matrices.default_projection);
    renderer.update_projection(matrices.projection);

    matrices.dirty_inverse_projection = true;
}

void graphics_set_auto_view_reset_enabled(bool enabled)
{
    auto_view_reset = enabled;
}

//------------------------------------------------------------------------------

void graphics_push_matrix(void)
{
    int index = matrices.current_model_view;

    if (index == (MAX_MATRICES - 1)) {
        return;
    }

    mat3_copy(matrices.model_view[index + 1], matrices.model_view[index]);

    matrices.current_model_view++;
    renderer.update_model_view(matrices.model_view[matrices.current_model_view]);
}

void graphics_pop_matrix(void)
{
    if (matrices.current_model_view == 0) {
        return;
    }

    matrices.current_model_view--;
    renderer.update_model_view(matrices.model_view[matrices.current_model_view]);
}

void graphics_translate_matrix(float x, float y)
{
    int index = matrices.current_model_view;
    mat3_translate(matrices.model_view[index], x, y);
    renderer.update_model_view(matrices.model_view[index]);
}

void graphics_scale_matrix(float x, float y)
{
    int index = matrices.current_model_view;
    mat3_scale(matrices.model_view[index], x, y);
    renderer.update_model_view(matrices.model_view[index]);
}

void graphics_rotate_matrix(float a)
{
    int index = matrices.current_model_view;
    mat3_rotate(matrices.model_view[index], RADIANS(a));
    renderer.update_model_view(matrices.model_view[index]);
}

//------------------------------------------------------------------------------

void graphics_draw_point(float x, float y)
{
    float data[] = {
        x, y,
    };

    renderer.set_draw_color(colors[COLOR_POINT].value);
    renderer.draw_solid(PRIMITIVE_POINTS, data, 1);
}

void graphics_draw_line(float ax, float ay, float bx, float by)
{
    float data[] = {
        ax, ay,
        bx, by,
    };

    renderer.set_draw_color(colors[COLOR_LINE].value);
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

    renderer.set_draw_color(colors[COLOR_OUTLINE].value);
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

    renderer.set_draw_color(colors[COLOR_OUTLINE].value);
    renderer.draw_solid(PRIMITIVE_LINE_LOOP, data, 4);
}

void graphics_outline_circle(float x, float y, float radius)
{
    int length;

    float *data = make_circle(x, y, radius, COLOR_OUTLINE, &length);

    renderer.set_draw_color(colors[COLOR_OUTLINE].value);
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

    renderer.set_draw_color(colors[COLOR_FILL].value);
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

    renderer.set_draw_color(colors[COLOR_FILL].value);
    renderer.draw_solid(PRIMITIVE_TRIANGLE_FAN, data, 4);
}

void graphics_fill_circle(float x, float y, float radius)
{
    int length;

    float *data = make_circle(x, y, radius, COLOR_FILL, &length);

    renderer.set_draw_color(colors[COLOR_FILL].value);
    renderer.draw_solid(PRIMITIVE_TRIANGLE_FAN, data, length - 1);

    mem_free(data);
}

tq_color graphics_get_point_color(void)
{
    return colors[COLOR_POINT].value;
}

void graphics_set_point_color(tq_color point_color)
{
    colors[COLOR_POINT].value = point_color;
}

tq_color graphics_get_line_color(void)
{
    return colors[COLOR_LINE].value;
}

void graphics_set_line_color(tq_color line_color)
{
    colors[COLOR_LINE].value = line_color;
}

tq_color graphics_get_outline_color(void)
{
    return colors[COLOR_OUTLINE].value;
}

void graphics_set_outline_color(tq_color outline_color)
{
    colors[COLOR_OUTLINE].value = outline_color;
}

tq_color graphics_get_fill_color(void)
{
    return colors[COLOR_FILL].value;
}

void graphics_set_fill_color(tq_color fill_color)
{
    colors[COLOR_FILL].value = fill_color;
}

//------------------------------------------------------------------------------

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

void graphics_set_texture_smooth(int texture_id, bool smooth)
{
    renderer.set_texture_smooth(texture_id, smooth);
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

void graphics_draw_subtexture(int texture_id,
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

//------------------------------------------------------------------------------

void graphics_on_display_resized(int width, int height)
{
    display_width = width;
    display_height = height;

    make_default_projection(matrices.default_projection, width, height);
    renderer.update_viewport(0, 0, width, height);
}

//------------------------------------------------------------------------------
