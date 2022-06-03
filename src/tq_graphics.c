
//------------------------------------------------------------------------------

#include <math.h>
#include <string.h>

#include "tq_graphics.h"
#include "tq_file.h"

//------------------------------------------------------------------------------

#define MAX_MATRICES 32

//------------------------------------------------------------------------------

static struct renderer const *renderer;

static float        matrices[MAX_MATRICES][9];
static unsigned int current_matrix;

static tq_color_t   clear_color;
static tq_color_t   point_color;
static tq_color_t   line_color;
static tq_color_t   outline_color;
static tq_color_t   fill_color;

//------------------------------------------------------------------------------

static float *build_circle(float x, float y, float radius, unsigned int *length)
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

#define RADIANS(deg) ((deg) * (M_PI / 180.0))
#define DEGREES(rad) ((rad) * (180.0 / M_PI))

void matrix3_identity(float *matrix)
{
    static float const identity[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };

    memcpy(matrix, identity, 9 * sizeof(float));
}

void matrix3_copy(float *dst, float const *src)
{
    memcpy(dst, src, 9 * sizeof(float));
}

void matrix3_multiply(float *m, float const *n)
{
    float const result[] = {
        m[0] * n[0] + m[1] * n[3] + m[2] * n[6],
        m[0] * n[1] + m[1] * n[4] + m[2] * n[7],
        m[0] * n[2] + m[1] * n[5] + m[2] * n[8],
        m[3] * n[0] + m[4] * n[3] + m[5] * n[6],
        m[3] * n[1] + m[4] * n[4] + m[5] * n[7],
        m[3] * n[2] + m[4] * n[5] + m[5] * n[8],
        m[6] * n[0] + m[7] * n[6] + m[8] * n[6],
        m[6] * n[1] + m[7] * n[7] + m[8] * n[7],
        m[6] * n[2] + m[7] * n[8] + m[8] * n[8],
    };

    memcpy(m, result, 9 * sizeof(float));
}

void matrix3_translate(float *matrix, float x, float y)
{
    float const multiplier[] = {
        1.0f, 0.0f, x,
        0.0f, 1.0f, y,
        0.0f, 0.0f, 1.0f,
    };

    matrix3_multiply(matrix, multiplier);
}

void matrix3_scale(float *matrix, float x, float y)
{
    float const multiplier[] = {
        x,      0.0f,   0.0f,
        0.0f,   y,      0.0f,
        0.0f,   0.0f,   1.0f,
    };

    matrix3_multiply(matrix, multiplier);
}

void matrix3_rotate(float *matrix, float a)
{
    float c = cosf(a);
    float s = sinf(a);

    float const multiplier[] = {
        c,      -s,     0.0f,
        s,      c,      0.0f,
        0.0f,   0.0f,   1.0f,
    };

    matrix3_multiply(matrix, multiplier);
}

//------------------------------------------------------------------------------

void graphics_initialize(struct renderer const *renderer_)
{
    renderer = renderer_;
    renderer->initialize();

    clear_color = tq_rgb(128, 128, 128);
    point_color = tq_rgb(255, 0, 255);
    line_color = tq_rgb(255, 0, 255);
    outline_color = tq_rgb(255, 0, 255);
    fill_color = tq_rgb(0, 255, 255);

    matrix3_identity(matrices[0]);
    renderer->transform(matrices[0]);

    current_matrix = 0;

    renderer->set_clear_color(clear_color);
    renderer->set_point_color(point_color);
    renderer->set_line_color(point_color);
    renderer->set_outline_color(outline_color);
    renderer->set_fill_color(fill_color);
}

void graphics_terminate(void)
{
    renderer->terminate();
}

void graphics_flush(void)
{
    renderer->flush();

    current_matrix = 0;
    matrix3_identity(matrices[0]);

    renderer->transform(matrices[0]);
}

void graphics_clear(void)
{
    renderer->clear();
}

tq_color_t graphics_get_clear_color(void)
{
    return clear_color;
}

void graphics_set_clear_color(tq_color_t clear_color_)
{
    clear_color = clear_color_;
    renderer->set_clear_color(clear_color);
}

void graphics_set_view(float x, float y, float width, float height, float angle)
{
    renderer->set_view(x, y, width, height, angle);
}

void graphics_reset_view(void)
{
    renderer->reset_view();
}

void graphics_push_matrix(void)
{
    unsigned int index = current_matrix;

    if (index == (MAX_MATRICES - 1)) {
        return;
    }

    matrix3_copy(matrices[index + 1], matrices[index]);

    current_matrix++;
    renderer->transform(matrices[current_matrix]);
}

void graphics_pop_matrix(void)
{
    if (current_matrix == 0) {
        return;
    }

    current_matrix--;
    renderer->transform(matrices[current_matrix]);
}

void graphics_translate_matrix(float x, float y)
{
    unsigned int index = current_matrix;
    matrix3_translate(matrices[index], x, y);
    renderer->transform(matrices[index]);
}

void graphics_scale_matrix(float x, float y)
{
    unsigned int index = current_matrix;
    matrix3_scale(matrices[index], x, y);
    renderer->transform(matrices[index]);
}

void graphics_rotate_matrix(float a)
{
    unsigned int index = current_matrix;
    matrix3_rotate(matrices[index], RADIANS(a));
    renderer->transform(matrices[index]);
}

void graphics_draw_point(float x, float y)
{
    float data[] = { x, y };
    renderer->draw_points(data, 1);
}

void graphics_draw_line(float ax, float ay, float bx, float by)
{
    float data[] = { ax, ay, bx, by };
    renderer->draw_lines(data, 2);
}

void graphics_draw_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = { ax, ay, bx, by, cx, cy };
    renderer->draw_fill(data, 3);
    renderer->draw_outline(data, 3);
}

void graphics_draw_rectangle(float x, float y, float w, float h)
{
    float data[] = { x, y, x + w, y, x + w, y + h, x, y + h };
    renderer->draw_fill(data, 4);
    renderer->draw_outline(data, 4);
}

void graphics_draw_circle(float x, float y, float radius)
{
    unsigned int length;
    float *data = build_circle(x, y, radius, &length);
    renderer->draw_fill(data, length);
    renderer->draw_outline(data, length);
    free(data);
}

void graphics_outline_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = { ax, ay, bx, by, cx, cy };
    renderer->draw_outline(data, 3);
}

void graphics_outline_rectangle(float x, float y, float w, float h)
{
    float data[] = { x, y, x + w, y, x + w, y + h, x, y + h };
    renderer->draw_outline(data, 4);
}

void graphics_outline_circle(float x, float y, float radius)
{
    unsigned int length;
    float *data = build_circle(x, y, radius, &length);
    renderer->draw_outline(data, length);
    free(data);
}

void graphics_fill_triangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    float data[] = { ax, ay, bx, by, cx, cy };
    renderer->draw_fill(data, 3);
}

void graphics_fill_rectangle(float x, float y, float w, float h)
{
    float data[] = { x, y, x + w, y, x + w, y + h, x, y + h };
    renderer->draw_fill(data, 4);
}

void graphics_fill_circle(float x, float y, float radius)
{
    unsigned int length;
    float *data = build_circle(x, y, radius, &length);
    renderer->draw_fill(data, length);
    free(data);
}

tq_color_t graphics_get_point_color(void)
{
    return point_color;
}

void graphics_set_point_color(tq_color_t point_color_)
{
    point_color = point_color_;
    renderer->set_point_color(point_color);
}

tq_color_t graphics_get_line_color(void)
{
    return line_color;
}

void graphics_set_line_color(tq_color_t line_color_)
{
    line_color = line_color_;
    renderer->set_line_color(line_color);
}

tq_color_t graphics_get_outline_color(void)
{
    return outline_color;
}

void graphics_set_outline_color(tq_color_t outline_color_)
{
    outline_color = outline_color_;
    renderer->set_outline_color(outline_color);
}

tq_color_t graphics_get_fill_color(void)
{
    return fill_color;
}

void graphics_set_fill_color(tq_color_t fill_color_)
{
    fill_color = fill_color_;
    renderer->set_fill_color(fill_color);
}

tq_handle_t graphics_load_texture_from_file(char const *path)
{
    uint8_t *buffer;
    size_t length;

    if (file_load(path, &length, &buffer) >= 0) {
        tq_handle_t texture_handle = renderer->load_texture(buffer, length);
        free(buffer);

        return texture_handle;
    }

    return TQ_INVALID_HANDLE;
}

tq_handle_t graphics_load_texture_from_memory(uint8_t const *buffer, size_t length)
{
    return renderer->load_texture(buffer, length);
}

void graphics_delete_texture(tq_handle_t texture_handle)
{
    renderer->delete_texture(texture_handle);
}

void graphics_get_texture_size(tq_handle_t texture_handle, uint32_t *width, uint32_t *height)
{
    renderer->get_texture_size(texture_handle, width, height);
}

void graphics_draw_texture(tq_handle_t texture_handle,
    float x, float y,
    float w, float h)
{
    uint32_t tw, th;
    renderer->get_texture_size(texture_handle, &tw, &th);

    float data[] = {
        x,      y,      0.0f,   0.0f,
        x + w,  y,      tw,     0.0f,
        x + w,  y + h,  tw,     th,
        x,      y + h,  0.0f,   th,
    };

    renderer->draw_texture(data, 4, texture_handle);
}

void graphics_draw_texture_fragment(tq_handle_t texture_handle,
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

    renderer->draw_texture(data, 4, texture_handle);
}

//------------------------------------------------------------------------------
