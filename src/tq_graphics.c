
//------------------------------------------------------------------------------

#include <math.h>
#include <string.h>

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
    COLOR_DRAW,
    COLOR_OUTLINE,
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
    int canvas_surface_id;
    int canvas_width;
    int canvas_height;
    float canvas_aspect_ratio;
};

struct tq_graphics_priv
{
    bool ready;
    bool active_rc;
    bool color_key_enabled;
    tq_color color_key;
    int antialiasing_level;
};

static struct graphics graphics;
static struct tq_renderer_impl renderer;
static struct matrices matrices;
static struct color colors[COLOR_COUNT];
static struct tq_graphics_priv priv;

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

static float *make_circle(float x, float y, float radius, int count)
{
    float angle = TQ_DEG2RAD(360.0f / count);
    float *data = libtq_malloc(2 * sizeof(float) * count);

    for (int v = 0; v < count; v++) {
        data[2 * v + 0] = x + (radius * cos(v * angle));
        data[2 * v + 1] = y + (radius * sin(v * angle));
    }

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

static int load_texture(libtq_stream *stream)
{
    libtq_image *image;

    if (priv.color_key_enabled) {
        image = libtq_load_image_with_key(stream, priv.color_key);
    } else {
        image = libtq_load_image(stream);
    }

    libtq_stream_close(stream);

    if (!image) {
        return -1;
    }

    int texture_id = renderer.create_texture(image->width, image->height, image->channels);

    if (texture_id == -1) {
        libtq_free(image);
        return -1;
    }

    renderer.update_texture(texture_id, 0, 0, -1, -1, image->pixels);
    libtq_free(image);

    return texture_id;
}

//------------------------------------------------------------------------------

void tq_initialize_graphics(void)
{
#if defined(TQ_ANDROID) || defined(TQ_USE_GLES2)
    tq_construct_gles2_renderer(&renderer);
#elif defined(TQ_WIN32) || defined(TQ_LINUX)
    tq_construct_gl_renderer(&renderer);
#else
    tq_construct_null_renderer(&renderer);
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

    tq_set_clear_color(tq_c24(0, 0, 255));
    tq_set_draw_color(tq_c24(0, 255, 255));
    tq_set_outline_color(tq_c24(255, 0, 255));

    priv.ready = true;

    if (priv.active_rc > 0) {
        tq_on_rc_create(priv.active_rc);
    }
}

void tq_terminate_graphics(void)
{
    if (priv.active_rc > 0) {
        tq_on_rc_destroy();
    }

    priv.ready = false;
}

void tq_process_graphics(void)
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

    renderer.draw_canvas(x0, y0, x1, y1);
    renderer.bind_surface(graphics.canvas_surface_id);

    mat3_identity(matrices.model_view[0]);
    renderer.update_model_view(matrices.model_view[0]);

    matrices.current_model_view = 0;

    renderer.post_process();
}

tq_vec2i tq_conv_display_coord(tq_vec2i coord)
{
    int display_width, display_height;
    libtq_get_display_size(&display_width, &display_height);

    float canvas_aspect = graphics.canvas_aspect_ratio;
    float display_aspect = libtq_get_display_aspect_ratio();

    if (display_aspect > canvas_aspect) {
        float x_scale = display_height / (float) graphics.canvas_height;
        float x_offset = (display_width - (graphics.canvas_width * x_scale)) / (x_scale * 2.0f);

        return (tq_vec2i) {
            .x = (coord.x * (float) graphics.canvas_height) / display_height - x_offset,
            .y = (coord.y / (float) display_height) * graphics.canvas_height,
        };
    } else {
        float y_scale = display_width / (float) graphics.canvas_width;
        float y_offset = (display_height - (graphics.canvas_height * y_scale)) / (y_scale * 2.0f);

        return (tq_vec2i) {
            .x = (coord.x / (float) display_width) * graphics.canvas_width,
            .y = (coord.y * (float) graphics.canvas_width) / display_width - y_offset,
        };
    }
}

//------------------------------------------------------------------------------
// API entries: state

void tq_set_color_key(tq_color color)
{
    if (color.r == 0 && color.g == 0 && color.b == 0 && color.a == 0) {
        priv.color_key_enabled = false;
        return;
    }

    priv.color_key_enabled = true;
    priv.color_key = color;
}

void tq_set_antialiasing_level(int level)
{
    priv.antialiasing_level = level;

    if (renderer.request_antialiasing_level) {
        priv.antialiasing_level = renderer.request_antialiasing_level(level);
    }
}

void tq_set_blend_mode(tq_blend_mode mode)
{
    renderer.set_blend_mode(mode);
}

//------------------------------------------------------------------------------
// API entries: canvas

void tq_clear(void)
{
    renderer.clear();
}

tq_color tq_get_clear_color(void)
{
    return colors[COLOR_CLEAR].value;
}

void tq_set_clear_color(tq_color clear_color)
{
    colors[COLOR_CLEAR].value = clear_color;
    renderer.set_clear_color(clear_color);
}

tq_vec2i tq_get_canvas_size(void)
{
    return (tq_vec2i) {
        .x = graphics.canvas_width,
        .y = graphics.canvas_height,
    };
}

void tq_set_canvas_size(tq_vec2i size)
{
    graphics.canvas_width = size.x;
    graphics.canvas_height = size.y;
    graphics.canvas_aspect_ratio = (float) size.x / (float) size.y;

    if (priv.ready && priv.active_rc > 0) {
        renderer.delete_surface(graphics.canvas_surface_id);
        graphics.canvas_surface_id = renderer.create_surface(size.x, size.y);
    }

    make_default_projection(matrices.default_projection, size.x, size.y);
}

bool tq_is_canvas_smooth(void)
{
    int texture_id = renderer.get_surface_texture_id(graphics.canvas_surface_id);
    return renderer.is_texture_smooth(texture_id);
}

void tq_set_canvas_smooth(bool smooth)
{
    int texture_id = renderer.get_surface_texture_id(graphics.canvas_surface_id);
    renderer.set_texture_smooth(texture_id, smooth);
}

//------------------------------------------------------------------------------
// API entries: views

tq_vec2f tq_get_relative_position(tq_vec2f absolute)
{
    float u = -1.0f + 2.0f * (absolute.x / (float) graphics.canvas_width);
    float v = +1.0f - 2.0f * (absolute.y / (float) graphics.canvas_height);

    tq_vec2f relative;
    mat4_transform_point(get_inverse_projection(), u, v, &relative.x, &relative.y);

    return relative;
}

void tq_set_view(tq_rectf rect, float rotation)
{
    make_projection(matrices.projection, rect.x, rect.y, rect.w, rect.h, rotation);
    renderer.update_projection(matrices.projection);

    matrices.dirty_inverse_projection = true;
}

void tq_set_view_f(float x, float y, float w, float h, float rotation)
{
    tq_rectf rect = { x, y, w, h };

    tq_set_view(rect, rotation);
}

void tq_reset_view(void)
{
    mat4_copy(matrices.projection, matrices.default_projection);
    renderer.update_projection(matrices.projection);

    matrices.dirty_inverse_projection = true;
}

//------------------------------------------------------------------------------
// API entries: matrices

void tq_push_matrix(void)
{
    int index = matrices.current_model_view;

    if (index == (MAX_MATRICES - 1)) {
        return;
    }

    mat3_copy(matrices.model_view[index + 1], matrices.model_view[index]);

    matrices.current_model_view++;
    renderer.update_model_view(matrices.model_view[matrices.current_model_view]);
}

void tq_pop_matrix(void)
{
    if (matrices.current_model_view == 0) {
        return;
    }

    matrices.current_model_view--;
    renderer.update_model_view(matrices.model_view[matrices.current_model_view]);
}

void tq_translate_matrix(tq_vec2f v)
{
    int index = matrices.current_model_view;
    mat3_translate(matrices.model_view[index], v.x, v.y);
    renderer.update_model_view(matrices.model_view[index]);
}

void tq_scale_matrix(tq_vec2f v)
{
    int index = matrices.current_model_view;
    mat3_scale(matrices.model_view[index], v.x, v.y);
    renderer.update_model_view(matrices.model_view[index]);
}

void tq_rotate_matrix(float degrees)
{
    int index = matrices.current_model_view;
    mat3_rotate(matrices.model_view[index], RADIANS(degrees));
    renderer.update_model_view(matrices.model_view[index]);
}

void tq_translate_matrix_f(float x, float y)
{
    tq_vec2f v = { x, y };

    tq_translate_matrix(v);
}

void tq_scale_matrix_f(float x, float y)
{
    tq_vec2f v = { x, y };

    tq_scale_matrix(v);
}

//------------------------------------------------------------------------------
// API entries: 2D primitives

void tq_draw_point(tq_vec2f position)
{
    float data[] = {
        position.x, position.y,
    };

    renderer.set_draw_color(colors[COLOR_DRAW].value);
    renderer.draw_solid(TQ_PRIMITIVE_POINTS, data, 1);
}

void tq_draw_line(tq_vec2f a, tq_vec2f b)
{
    float data[] = {
        a.x, a.y,
        b.x, b.y,
    };

    renderer.set_draw_color(colors[COLOR_DRAW].value);
    renderer.draw_solid(TQ_PRIMITIVE_LINE_STRIP, data, 2);
}

void tq_draw_triangle(tq_vec2f a, tq_vec2f b, tq_vec2f c)
{
    float data[] = {
        a.x, a.y,
        b.x, b.y,
        c.x, c.y,
    };

    renderer.set_draw_color(colors[COLOR_DRAW].value);
    renderer.draw_solid(TQ_PRIMITIVE_TRIANGLE_FAN, data, 3);

    renderer.set_draw_color(colors[COLOR_OUTLINE].value);
    renderer.draw_solid(TQ_PRIMITIVE_LINE_LOOP, data, 3);
}

void tq_draw_rectangle(tq_rectf rect)
{
    float data[] = {
        rect.x,             rect.y,
        rect.x + rect.w,    rect.y,
        rect.x + rect.w,    rect.y + rect.h,
        rect.x,             rect.y + rect.h,
    };

    renderer.set_draw_color(colors[COLOR_DRAW].value);
    renderer.draw_solid(TQ_PRIMITIVE_TRIANGLE_FAN, data, 4);

    renderer.set_draw_color(colors[COLOR_OUTLINE].value);
    renderer.draw_solid(TQ_PRIMITIVE_LINE_LOOP, data, 4);
}

void tq_draw_circle(tq_vec2f position, float radius)
{
    int precision = 32;
    float *data = make_circle(position.x, position.y, radius, precision);

    if (!data) {
        return;
    }

    renderer.set_draw_color(colors[COLOR_DRAW].value);
    renderer.draw_solid(TQ_PRIMITIVE_TRIANGLE_FAN, data, precision - 1);

    renderer.set_draw_color(colors[COLOR_OUTLINE].value);
    renderer.draw_solid(TQ_PRIMITIVE_LINE_LOOP, data, precision);

    libtq_free(data);
}

void tq_outline_triangle(tq_vec2f a, tq_vec2f b, tq_vec2f c)
{
    float data[] = {
        a.x, a.y,
        b.x, b.y,
        c.x, c.y,
    };

    renderer.set_draw_color(colors[COLOR_OUTLINE].value);
    renderer.draw_solid(TQ_PRIMITIVE_LINE_LOOP, data, 3);
}

void tq_outline_rectangle(tq_rectf rect)
{
    float data[] = {
        rect.x,             rect.y,
        rect.x + rect.w,    rect.y,
        rect.x + rect.w,    rect.y + rect.h,
        rect.x,             rect.y + rect.h,
    };

    renderer.set_draw_color(colors[COLOR_OUTLINE].value);
    renderer.draw_solid(TQ_PRIMITIVE_LINE_LOOP, data, 4);
}

void tq_outline_circle(tq_vec2f position, float radius)
{
    int precision = 32;
    float *data = make_circle(position.x, position.y, radius, precision);

    if (!data) {
        return;
    }

    renderer.set_draw_color(colors[COLOR_OUTLINE].value);
    renderer.draw_solid(TQ_PRIMITIVE_LINE_LOOP, data, precision);

    libtq_free(data);
}

void tq_fill_triangle(tq_vec2f a, tq_vec2f b, tq_vec2f c)
{
    float data[] = {
        a.x, a.y,
        b.x, b.y,
        c.x, c.y,
    };

    renderer.set_draw_color(colors[COLOR_DRAW].value);
    renderer.draw_solid(TQ_PRIMITIVE_TRIANGLE_FAN, data, 3);
}

void tq_fill_rectangle(tq_rectf rect)
{
    float data[] = {
        rect.x,             rect.y,
        rect.x + rect.w,    rect.y,
        rect.x + rect.w,    rect.y + rect.h,
        rect.x,             rect.y + rect.h,
    };

    renderer.set_draw_color(colors[COLOR_DRAW].value);
    renderer.draw_solid(TQ_PRIMITIVE_TRIANGLE_FAN, data, 4);
}

void tq_fill_circle(tq_vec2f position, float radius)
{
    int precision = 32;
    float *data = make_circle(position.x, position.y, radius, precision);

    if (!data) {
        return;
    }

    renderer.set_draw_color(colors[COLOR_DRAW].value);
    renderer.draw_solid(TQ_PRIMITIVE_TRIANGLE_FAN, data, precision - 1);

    libtq_free(data);
}

void tq_draw_point_f(float x, float y)
{
    tq_vec2f v = { x, y };

    tq_draw_point(v);
}

void tq_draw_line_f(float ax, float ay, float bx, float by)
{
    tq_vec2f a = { ax, ay };
    tq_vec2f b = { bx, by };

    tq_draw_line(a, b);
}

void tq_draw_triangle_f(float ax, float ay, float bx, float by, float cx, float cy)
{
    tq_vec2f a = { ax, ay };
    tq_vec2f b = { bx, by };
    tq_vec2f c = { cx, cy };

    tq_draw_triangle(a, b, c);
}

void tq_draw_rectangle_f(float x, float y, float w, float h)
{
    tq_rectf r = { x, y, w, h };

    tq_draw_rectangle(r);
}

void tq_draw_circle_f(float x, float y, float radius)
{
    tq_vec2f v = { x, y };

    tq_draw_circle(v, radius);
}

void tq_outline_triangle_f(float ax, float ay, float bx, float by, float cx, float cy)
{
    tq_vec2f a = { ax, ay };
    tq_vec2f b = { bx, by };
    tq_vec2f c = { cx, cy };

    tq_outline_triangle(a, b, c);
}

void tq_outline_rectangle_f(float x, float y, float w, float h)
{
    tq_rectf r = { x, y, w, h };

    tq_outline_rectangle(r);
}

void tq_outline_circle_f(float x, float y, float radius)
{
    tq_vec2f v = { x, y };

    tq_outline_circle(v, radius);
}

void tq_fill_triangle_f(float ax, float ay, float bx, float by, float cx, float cy)
{
    tq_vec2f a = { ax, ay };
    tq_vec2f b = { bx, by };
    tq_vec2f c = { cx, cy };

    tq_fill_triangle(a, b, c);
}

void tq_fill_rectangle_f(float x, float y, float w, float h)
{
    tq_rectf r = { x, y, w, h };

    tq_fill_rectangle(r);
}

void tq_fill_circle_f(float x, float y, float radius)
{
    tq_vec2f v = { x, y };

    tq_fill_circle(v, radius);
}

tq_color tq_get_draw_color(void)
{
    return colors[COLOR_DRAW].value;
}

void tq_set_draw_color(tq_color draw_color)
{
    colors[COLOR_DRAW].value = draw_color;
    tq_set_text_color(draw_color);
}

tq_color tq_get_outline_color(void)
{
    return colors[COLOR_OUTLINE].value;
}

void tq_set_outline_color(tq_color outline_color)
{
    colors[COLOR_OUTLINE].value = outline_color;
    tq_set_text_outline_color(outline_color);
}

//------------------------------------------------------------------------------
// API entries: textures

tq_texture tq_load_texture_from_file(char const *path)
{
    return (tq_texture) { load_texture(libtq_open_file_stream(path)) };
}

tq_texture tq_load_texture_from_memory(uint8_t const *buffer, size_t size)
{
    return (tq_texture) { load_texture(libtq_open_memory_stream(buffer, size)) };
}

void tq_delete_texture(tq_texture texture)
{
    renderer.delete_texture(texture.id);
}

tq_vec2i tq_get_texture_size(tq_texture texture)
{
    tq_vec2i size;
    renderer.get_texture_size(texture.id, &size.x, &size.y);

    return size;
}

void tq_set_texture_smooth(tq_texture texture, bool smooth)
{
    renderer.set_texture_smooth(texture.id, smooth);
}

void tq_draw_texture(tq_texture texture, tq_rectf rect)
{
    float data[] = {
        rect.x,             rect.y,             0.0f,   0.0f,
        rect.x + rect.w,    rect.y,             1.0f,   0.0f,
        rect.x + rect.w,    rect.y + rect.h,    1.0f,   1.0f,
        rect.x,             rect.y + rect.h,    0.0f,   1.0f,
    };

    renderer.bind_texture(texture.id);
    renderer.draw_textured(TQ_PRIMITIVE_TRIANGLE_FAN, data, 4);
}

void tq_draw_subtexture(tq_texture texture, tq_rectf sub, tq_rectf rect)
{
    int u, v;
    renderer.get_texture_size(texture.id, &u, &v);

    float as = sub.x / u;
    float at = sub.y / v;
    float bs = (sub.x + sub.w) / u;
    float bt = (sub.y + sub.h) / v;

    float data[] = {
        rect.x,             rect.y,             as,     at,
        rect.x + rect.w,    rect.y,             bs,     at,
        rect.x + rect.w,    rect.y + rect.h,    bs,     bt,
        rect.x,             rect.y + rect.h,    as,     bt,
    };

    renderer.bind_texture(texture.id);
    renderer.draw_textured(TQ_PRIMITIVE_TRIANGLE_FAN, data, 4);
}

void tq_draw_texture_f(tq_texture texture, float x, float y, float w, float h)
{
    tq_rectf rect = { x, y, w, h };

    tq_draw_texture(texture, rect);
}

void tq_draw_subtexture_f(tq_texture texture,
    float fx, float fy, float fw, float fh,
    float x, float y, float w, float h)
{
    tq_rectf sub = { fx, fy, fx, fh };
    tq_rectf rect = { x, y, w, h };

    tq_draw_subtexture(texture, sub, rect);
}

//------------------------------------------------------------------------------
// API entries: surfaces

tq_surface tq_create_surface(tq_vec2i size)
{
    return (tq_surface) {
        renderer.create_surface(size.x, size.y),
    };
}

void tq_delete_surface(tq_surface surface)
{
    renderer.delete_surface(surface.id);
}

void tq_set_surface(tq_surface surface)
{
    renderer.bind_surface(surface.id);

    int texture_id = renderer.get_surface_texture_id(surface.id);

    int width, height;
    renderer.get_texture_size(texture_id, &width, &height);

    float projection[16];
    make_default_projection_for_surface(projection, width, height);

    renderer.update_projection(projection);
}

void tq_reset_surface(void)
{
    renderer.bind_surface(graphics.canvas_surface_id);
    renderer.update_projection(matrices.projection);
}

tq_texture tq_get_surface_texture(tq_surface surface)
{
    return (tq_texture) {
        .id = renderer.get_surface_texture_id(surface.id),
    };
}

//------------------------------------------------------------------------------

void tq_on_rc_create(int rc)
{
    priv.active_rc = rc;

    if (!priv.ready) {
        return;
    }

    renderer.initialize();

    priv.antialiasing_level = renderer.request_antialiasing_level(priv.antialiasing_level);

    graphics.canvas_surface_id = renderer.create_surface(
        graphics.canvas_width,
        graphics.canvas_height
    );

    renderer.update_projection(matrices.projection);
    renderer.update_model_view(matrices.model_view[0]);

    tq_initialize_text(&renderer);
}

void tq_on_rc_destroy(void)
{
    if (priv.active_rc == 0) {
        return;
    }

    priv.active_rc = 0;

    tq_terminate_text();
    renderer.terminate();
}

//------------------------------------------------------------------------------