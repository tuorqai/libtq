
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

struct graphics
{
    int ready;

    int canvas_surface_id;
    int canvas_width;
    int canvas_height;
    float canvas_aspect_ratio;
};

static struct graphics graphics;
static struct libtq_renderer_impl renderer;
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

static void make_default_projection(float *dst, int w, int h)
{
    mat4_ortho(dst, 0.0f, (float) w, (float) h, 0.0f, -1.0f, +1.0f);
}

static void make_default_projection_for_surface(float *dst, int w, int h)
{
    mat4_ortho(dst, 0.0f, (float) w, 0.0f, (float) h, -1.0f, +1.0f);
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

void libtq_initialize_graphics(void)
{
#if defined(TQ_USE_OPENGL)
    libtq_construct_gl_renderer(&renderer);
#else
    #error Invalid configuration. Check your build settings.
#endif

    int display_width, display_height;
    libtq_get_display_size(&display_width, &display_height);

    if (!graphics.canvas_width || !graphics.canvas_height) {
        graphics.canvas_width = display_width;
        graphics.canvas_height = display_height;
        graphics.canvas_aspect_ratio = (float) display_width / (float) display_height;
    }

    make_default_projection(matrices.default_projection,
        graphics.canvas_width, graphics.canvas_height);
    mat4_copy(matrices.projection, matrices.default_projection);

    for (int index = 0; index < MAX_MATRICES; index++) {
        mat3_identity(matrices.model_view[index]);
    }

    matrices.current_model_view = 0;
    matrices.dirty_inverse_projection = true;

    libtq_set_clear_color(tq_c24(0, 0, 0));
    libtq_set_point_color(tq_c24(255, 255, 255));
    libtq_set_line_color(tq_c24(255, 255, 255));
    libtq_set_outline_color(tq_c24(255, 255, 255));
    libtq_set_fill_color(tq_c24(0, 0, 0));

    renderer.initialize();

    graphics.canvas_surface_id = renderer.create_surface(
        graphics.canvas_width,
        graphics.canvas_height
    );

    renderer.update_projection(matrices.projection);
    renderer.update_model_view(matrices.model_view[0]);

    text_initialize(&renderer);

    graphics.ready = 1;
}

void libtq_terminate_graphics(void)
{
    text_terminate();
    renderer.terminate();

    graphics.ready = 0;
}

void libtq_process_graphics(void)
{
    renderer.process();

    int canvas_texture_id = renderer.get_surface_texture_id(graphics.canvas_surface_id);

    renderer.bind_surface(-1);
    renderer.bind_texture(canvas_texture_id);

    float canvas_aspect_ratio = graphics.canvas_aspect_ratio;
    float display_aspect_ratio = libtq_get_display_aspect_ratio();

    float x0, x1, y0, y1;

    if (display_aspect_ratio > graphics.canvas_aspect_ratio) {
        x0 = -(canvas_aspect_ratio / display_aspect_ratio);
        x1 = +(canvas_aspect_ratio / display_aspect_ratio);
        y0 = -1.0f;
        y1 = +1.0f;
    } else {
        x0 = -1.0f;
        x1 = +1.0f;
        y0 = -(display_aspect_ratio / canvas_aspect_ratio);
        y1 = +(display_aspect_ratio / canvas_aspect_ratio);
    }

    float data[] = {
        x0, y0, 0.0f, 0.0f,
        x1, y0, 1.0f, 0.0f,
        x1, y1, 1.0f, 1.0f,
        x0, y1, 0.0f, 1.0f,
    };

    renderer.draw_canvas(data);
    renderer.bind_surface(graphics.canvas_surface_id);

    mat3_identity(matrices.model_view[0]);
    renderer.update_model_view(matrices.model_view[0]);

    matrices.current_model_view = 0;
}

//------------------------------------------------------------------------------

void libtq_clear(void)
{
    renderer.clear();
}

tq_color libtq_get_clear_color(void)
{
    return colors[COLOR_CLEAR].value;
}

void libtq_set_clear_color(tq_color clear_color)
{
    colors[COLOR_CLEAR].value = clear_color;
    renderer.set_clear_color(clear_color);
}

void libtq_get_canvas_size(int *width, int *height)
{
    *width = graphics.canvas_width;
    *height = graphics.canvas_height;
}

void libtq_set_canvas_size(int width, int height)
{
    graphics.canvas_width = width;
    graphics.canvas_height = height;
    graphics.canvas_aspect_ratio = (float) width / (float) height;

    if (graphics.ready) {
        renderer.delete_surface(graphics.canvas_surface_id);
        graphics.canvas_surface_id = renderer.create_surface(width, height);
    }

    make_default_projection(matrices.default_projection, width, height);
}

float libtq_get_canvas_aspect_ratio(void)
{
    return graphics.canvas_aspect_ratio;
}

bool libtq_is_canvas_smooth(void)
{
    int texture_id = renderer.get_surface_texture_id(graphics.canvas_surface_id);
    return renderer.is_texture_smooth(texture_id);
}

void libtq_set_canvas_smooth(bool smooth)
{
    int texture_id = renderer.get_surface_texture_id(graphics.canvas_surface_id);
    renderer.set_texture_smooth(texture_id, smooth);
}

void libtq_conv_display_coord_to_canvas_coord(int x, int y, int *u, int *v)
{
    int display_width, display_height;
    libtq_get_display_size(&display_width, &display_height);

    float canvas_aspect = graphics.canvas_aspect_ratio;
    float display_aspect = libtq_get_display_aspect_ratio();

    if (display_aspect > canvas_aspect) {
        float x_scale = display_height / (float) graphics.canvas_height;
        float x_offset = (display_width - (graphics.canvas_width * x_scale)) / (x_scale * 2.0f);

        *u = (x * (float) graphics.canvas_height) / display_height - x_offset;
        *v = (y / (float) display_height) * graphics.canvas_height;
    } else {
        float y_scale = display_width / (float) graphics.canvas_width;
        float y_offset = (display_height - (graphics.canvas_height * y_scale)) / (y_scale * 2.0f);

        *u = (x / (float) display_width) * graphics.canvas_width;
        *v = (y * (float) graphics.canvas_width) / display_width - y_offset;
    }
}

//------------------------------------------------------------------------------

void libtq_get_relative_position(float ax, float ay, float *x, float *y)
{
    float u = -1.0f + 2.0f * (ax / (float) graphics.canvas_width);
    float v = +1.0f - 2.0f * (ay / (float) graphics.canvas_height);

    mat4_transform_point(get_inverse_projection(), u, v, x, y);
}

void libtq_set_view(float x, float y, float w, float h, float rotation)
{
    make_projection(matrices.projection, x, y, w, h, rotation);
    renderer.update_projection(matrices.projection);

    matrices.dirty_inverse_projection = true;
}

void libtq_reset_view(void)
{
    mat4_copy(matrices.projection, matrices.default_projection);
    renderer.update_projection(matrices.projection);

    matrices.dirty_inverse_projection = true;
}

//------------------------------------------------------------------------------

void libtq_push_matrix(void)
{
    int index = matrices.current_model_view;

    if (index == (MAX_MATRICES - 1)) {
        return;
    }

    mat3_copy(matrices.model_view[index + 1], matrices.model_view[index]);

    matrices.current_model_view++;
    renderer.update_model_view(matrices.model_view[matrices.current_model_view]);
}

void libtq_pop_matrix(void)
{
    if (matrices.current_model_view == 0) {
        return;
    }

    matrices.current_model_view--;
    renderer.update_model_view(matrices.model_view[matrices.current_model_view]);
}

void libtq_translate_matrix(float x, float y)
{
    int index = matrices.current_model_view;
    mat3_translate(matrices.model_view[index], x, y);
    renderer.update_model_view(matrices.model_view[index]);
}

void libtq_scale_matrix(float x, float y)
{
    int index = matrices.current_model_view;
    mat3_scale(matrices.model_view[index], x, y);
    renderer.update_model_view(matrices.model_view[index]);
}

void libtq_rotate_matrix(float a)
{
    int index = matrices.current_model_view;
    mat3_rotate(matrices.model_view[index], RADIANS(a));
    renderer.update_model_view(matrices.model_view[index]);
}

//------------------------------------------------------------------------------

void libtq_draw_point(float x, float y)
{
    float data[] = {
        x, y,
    };

    renderer.set_draw_color(colors[COLOR_POINT].value);
    renderer.draw_solid(LIBTQ_POINTS, data, 1);
}

void libtq_draw_line(float ax, float ay, float bx, float by)
{
    float data[] = {
        ax, ay,
        bx, by,
    };

    renderer.set_draw_color(colors[COLOR_LINE].value);
    renderer.draw_solid(LIBTQ_LINE_STRIP, data, 2);
}

void libtq_draw_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    libtq_fill_triangle(ax, ay, bx, by, cx, cy);
    libtq_outline_triangle(ax, ay, bx, by, cx, cy);
}

void libtq_draw_rectangle(float x, float y, float w, float h)
{
    libtq_fill_rectangle(x, y, w, h);
    libtq_outline_rectangle(x, y, w, h);
}

void libtq_draw_circle(float x, float y, float radius)
{
    libtq_fill_circle(x, y, radius);
    libtq_outline_circle(x, y, radius);
}

void libtq_outline_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = {
        ax, ay,
        bx, by,
        cx, cy,
    };

    renderer.set_draw_color(colors[COLOR_OUTLINE].value);
    renderer.draw_solid(LIBTQ_LINE_LOOP, data, 3);
}

void libtq_outline_rectangle(float x, float y, float w, float h)
{
    float data[] = {
        x,      y,
        x + w,  y,
        x + w,  y + h,
        x,      y + h,
    };

    renderer.set_draw_color(colors[COLOR_OUTLINE].value);
    renderer.draw_solid(LIBTQ_LINE_LOOP, data, 4);
}

void libtq_outline_circle(float x, float y, float radius)
{
    int length;

    float *data = make_circle(x, y, radius, COLOR_OUTLINE, &length);

    renderer.set_draw_color(colors[COLOR_OUTLINE].value);
    renderer.draw_solid(LIBTQ_LINE_LOOP, data, length);

    mem_free(data);
}

void libtq_fill_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = {
        ax, ay,
        bx, by,
        cx, cy,
    };

    renderer.set_draw_color(colors[COLOR_FILL].value);
    renderer.draw_solid(LIBTQ_TRIANGLE_FAN, data, 3);
}

void libtq_fill_rectangle(float x, float y, float w, float h)
{
    float data[] = {
        x,      y,
        x + w,  y,
        x + w,  y + h,
        x,      y + h,
    };

    renderer.set_draw_color(colors[COLOR_FILL].value);
    renderer.draw_solid(LIBTQ_TRIANGLE_FAN, data, 4);
}

void libtq_fill_circle(float x, float y, float radius)
{
    int length;

    float *data = make_circle(x, y, radius, COLOR_FILL, &length);

    renderer.set_draw_color(colors[COLOR_FILL].value);
    renderer.draw_solid(LIBTQ_TRIANGLE_FAN, data, length - 1);

    mem_free(data);
}

tq_color libtq_get_point_color(void)
{
    return colors[COLOR_POINT].value;
}

void libtq_set_point_color(tq_color point_color)
{
    colors[COLOR_POINT].value = point_color;
}

tq_color libtq_get_line_color(void)
{
    return colors[COLOR_LINE].value;
}

void libtq_set_line_color(tq_color line_color)
{
    colors[COLOR_LINE].value = line_color;
}

tq_color libtq_get_outline_color(void)
{
    return colors[COLOR_OUTLINE].value;
}

void libtq_set_outline_color(tq_color outline_color)
{
    colors[COLOR_OUTLINE].value = outline_color;
    text_set_outline_color(outline_color);
}

tq_color libtq_get_fill_color(void)
{
    return colors[COLOR_FILL].value;
}

void libtq_set_fill_color(tq_color fill_color)
{
    colors[COLOR_FILL].value = fill_color;
    text_set_fill_color(fill_color);
}

//------------------------------------------------------------------------------

int libtq_load_texture(libtq_stream *stream)
{
    struct image image = image_load(stream);
    libtq_stream_close(stream);

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

int libtq_load_texture_from_file(char const *path)
{
    return libtq_load_texture(libtq_open_file_stream(path));
}

int libtq_load_texture_from_memory(void const *buffer, size_t size)
{
    return libtq_load_texture(libtq_open_memory_stream(buffer, size));
}

void libtq_delete_texture(int texture_id)
{
    renderer.delete_texture(texture_id);
}

void libtq_get_texture_size(int texture_id, int *width, int *height)
{
    renderer.get_texture_size(texture_id, width, height);
}

void libtq_set_texture_smooth(int texture_id, bool smooth)
{
    renderer.set_texture_smooth(texture_id, smooth);
}

void libtq_draw_texture(int texture_id,
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
    renderer.draw_textured(LIBTQ_TRIANGLE_FAN, data, 4);
}

void libtq_draw_subtexture(int texture_id,
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
    renderer.draw_textured(LIBTQ_TRIANGLE_FAN, data, 4);
}

//------------------------------------------------------------------------------

int libtq_create_surface(int width, int height)
{
    return renderer.create_surface(width, height);
}

void libtq_delete_surface(int surface_id)
{
    renderer.delete_surface(surface_id);
}

void libtq_set_surface(int surface_id)
{
    if (surface_id < 0) {
        return;
    }

    renderer.bind_surface(surface_id);

    int texture_id = renderer.get_surface_texture_id(surface_id);

    int width, height;
    renderer.get_texture_size(texture_id, &width, &height);

    float projection[16];
    make_default_projection_for_surface(projection, width, height);

    renderer.update_projection(projection);
}

void libtq_reset_surface(void)
{
    renderer.bind_surface(graphics.canvas_surface_id);
    renderer.update_projection(matrices.projection);
}

int libtq_get_surface_texture_id(int surface_id)
{
    return renderer.get_surface_texture_id(surface_id);
}

//------------------------------------------------------------------------------

void libtq_set_blend_mode(tq_blend_mode mode)
{
    renderer.set_blend_mode(mode);
}

//------------------------------------------------------------------------------