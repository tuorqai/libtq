
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

//------------------------------------------------------------------------------
// Declarations

#define MAX_MATRICES 32

struct glyph
{
    float           w, h;
    float           s, t;
    float           u, v; // left-bearing and top-bearing
};

struct fonts
{
    hb_font_t       *handle[TQ_MAX_FONTS];
    FT_Face         face[TQ_MAX_FONTS];
    FT_StreamRec    stream[TQ_MAX_FONTS];
    int32_t         texture_id[TQ_MAX_FONTS];
    uint32_t        num_glyphs[TQ_MAX_FONTS];
    struct glyph    *metrics[TQ_MAX_FONTS];
};

typedef struct tq_graphics
{
    tq_renderer_t   renderer;
    FT_Library      ft;

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
static struct fonts fonts;

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

    if (FT_Init_FreeType(&graphics.ft) != 0) {
        tq_error("Failed to initialize FreeType library.");
    }
}

void tq_graphics_terminate(void)
{
    for (int32_t font_id = 0; font_id < TQ_MAX_FONTS; font_id++) {
        if (fonts.face[font_id] != NULL) {
            graphics_delete_font(font_id);
        }
    }

    FT_Done_FreeType(graphics.ft);
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

static int32_t get_font_id(void)
{
    for (int32_t font_id = 0; font_id < TQ_MAX_FONTS; font_id++) {
        if (fonts.face[font_id] == NULL) {
            return font_id;
        }
    }

    return -1;
}

/**
 * FT_Stream_IoFunc()
 */
static unsigned long ft_stream_io(FT_Stream stream,
    unsigned long offset,
    unsigned char *buffer,
    unsigned long count)
{
    if (input_stream_seek(stream->descriptor.value, offset) == -1) {
        if (count == 0) {
            return 1;
        }

        return 0;
    }

    return input_stream_read(stream->descriptor.value, buffer, count);
}

/**
 * FT_Stream_CloseFunc()
 */
static void ft_stream_close(FT_Stream stream)
{
}

static int32_t load_font(int32_t stream_id, float pt, int weight)
{
    int32_t font_id = get_font_id();

    if (font_id == -1) {
        return -1;
    }

    fonts.stream[font_id] = (FT_StreamRec) {
        .base = NULL,
        .size = input_stream_size(stream_id),
        .pos = input_stream_tell(stream_id),
        .descriptor = { .value = stream_id },
        .pathname = { .pointer = (void *) input_stream_repr(stream_id) },
        .read = ft_stream_io,
        .close = ft_stream_close,
    };

    FT_Error ft_status = FT_Open_Face(graphics.ft,
        &(FT_Open_Args) {
            .flags = FT_OPEN_STREAM,
            .stream = &fonts.stream[font_id],
        },
        0, &fonts.face[font_id]
    );

    if (ft_status != 0) {
        log_error("Failed to open font %s. Error: 0x%02x\n",
            input_stream_repr(stream_id), ft_status);

        fonts.face[font_id] = NULL;
        return -1;
    }

    FT_Set_Char_Size(fonts.face[font_id], 0, (FT_F26Dot6) (pt * 64.0f), 0, 0);

    fonts.handle[font_id] = hb_ft_font_create_referenced(fonts.face[font_id]);

#if 0

    fonts.num_glyphs[font_id] = 224;
    fonts.metrics[font_id] = mem_malloc(sizeof(struct glyph) * 224);

    int atlas_width = 0;
    int atlas_height = 0;

    for (unsigned long c = 0; c < 224; c++) {
        if (FT_Load_Char(fonts.face[font_id], c, FT_LOAD_RENDER) != 0) {
            continue;
        }

        atlas_width += fonts.face[font_id]->glyph->bitmap.width;
        atlas_height = TQ_MAX(atlas_height, fonts.face[font_id]->glyph->bitmap.rows);
    }

    log_debug("atlas: %dx%d\n", atlas_width, atlas_height);

    struct image atlas = image_create(atlas_width, atlas_height, PIXEL_FORMAT_GRAYSCALE);
    int atlas_column = 0;

    for (unsigned long c = 0; c < 224; c++) {
        if (FT_Load_Char(fonts.face[font_id], c, FT_LOAD_RENDER) != 0) {
            continue;
        }

        FT_GlyphSlot glyph = fonts.face[font_id]->glyph;

        for (unsigned int row = 0; row < glyph->bitmap.rows; row++) {
            void *dst = &atlas.pixels[(row * atlas.width) + atlas_column];
            void *src = &glyph->bitmap.buffer[glyph->bitmap.width * row];

            memcpy(dst, src, glyph->bitmap.width);
        }

        fonts.metrics[font_id][c].w = glyph->bitmap.width;
        fonts.metrics[font_id][c].h = glyph->bitmap.rows;

        fonts.metrics[font_id][c].s = atlas_column;
        fonts.metrics[font_id][c].t = 0;

        fonts.metrics[font_id][c].u = glyph->bitmap_left;
        fonts.metrics[font_id][c].v = glyph->bitmap_top - pt;

        atlas_column += glyph->bitmap.width;
    }

    fonts.texture_id[font_id] = graphics.renderer.create_texture(&(tq_image_t) {
        .pixels = atlas.pixels,
        .width = atlas.width,
        .height = atlas.height,
        .pixel_format = atlas.channels,
        .stream_id = -1,
    });

    mem_free(atlas.pixels);

#endif

    return font_id;
}

int32_t graphics_load_font_from_file(char const *path, float pt, int weight)
{
    int32_t stream_id = open_file_input_stream(path);

    if (stream_id == -1) {
        return -1;
    }

    int32_t font_id = load_font(stream_id, pt, weight);

    if (font_id == -1) {
        input_stream_close(stream_id);
        return -1;
    }

    return font_id;
}

int32_t graphics_load_font_from_memory(uint8_t const *buffer, size_t size, float pt, int weight)
{
    int32_t stream_id = open_memory_input_stream(buffer, size);

    if (stream_id == -1) {
        return -1;
    }

    int32_t font_id = load_font(stream_id, pt, weight);

    if (font_id == -1) {
        input_stream_close(stream_id);
        return -1;
    }

    return font_id;
}

void graphics_delete_font(int32_t font_id)
{
    mem_free(fonts.metrics[font_id]);

    int32_t stream_id = fonts.stream[font_id].descriptor.value;

    hb_font_destroy(fonts.handle[font_id]);
    input_stream_close(stream_id);

    fonts.face[font_id] = NULL;
}

static int get_glyph_height(FT_GlyphSlot glyph)
{
    return (glyph->bitmap.rows);
}

static int get_glyph_baseline(FT_GlyphSlot glyph)
{
    return (glyph->bitmap.rows) - (glyph->bitmap.rows - glyph->bitmap_top);
}

void graphics_draw_text(int32_t font_id, float x, float y, char const *fmt, va_list ap)
{
    char str[1024];
    int len = vsnprintf(str, sizeof(str) - 1, fmt, ap);

    hb_font_t *font = fonts.handle[font_id];
    FT_Face face = hb_ft_font_get_face(font);

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_add_utf8(buffer, str, len, 0, -1);
    hb_buffer_guess_segment_properties(buffer);
    hb_shape(fonts.handle[font_id], buffer, NULL, 0);

    unsigned int glyph_count = hb_buffer_get_length(buffer);
    hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buffer, NULL);

    int min_texture_width = 0;
    int min_texture_height = 0;
    int baseline = 0;

    for (unsigned int i = 0; i < glyph_count; i++) {
        min_texture_width += glyph_pos[i].x_advance / 64.0f;

        if (FT_Load_Glyph(face, glyph_info[i].codepoint, FT_LOAD_RENDER) == 0) {
            int glyph_height = get_glyph_height(face->glyph);
            int glyph_baseline = get_glyph_baseline(face->glyph);
    
            if (glyph_height > min_texture_height) {
                min_texture_height = glyph_height;
            }

            if (glyph_baseline > baseline) {
                baseline = glyph_baseline;
            }
        }
    }

    int texture_width = 16;
    int texture_height = 16;

    while (texture_width < min_texture_width) {
        texture_width *= 2;
    }

    while (texture_height < min_texture_height) {
        texture_height *= 2;
    }

    struct image text = image_create(texture_width, texture_height, PIXEL_FORMAT_GRAYSCALE);

    int x_current = 0;
    int y_current = 0;

    for (unsigned int i = 0; i < glyph_count; i++) {
        if (FT_Load_Glyph(face, glyph_info[i].codepoint, FT_LOAD_RENDER) != 0) {
            continue;
        }

        int glyph_baseline = get_glyph_baseline(face->glyph);

        int x_offset = glyph_pos[i].x_offset >> 6;
        int y_offset = (glyph_pos[i].y_offset >> 6) + (baseline - glyph_baseline);

        for (unsigned int row = 0; row < face->glyph->bitmap.rows; row++) {
            int glyph_row = row;
            int text_row = y_current + y_offset + row;

            for (unsigned int col = 0; col < face->glyph->bitmap.width; col++) {
                int glyph_col = col;
                int text_col = x_current + x_offset + col;

                int glyph_idx = glyph_row * face->glyph->bitmap.width + glyph_col;
                int text_idx = text_row * text.width + text_col;

                int sum = text.pixels[text_idx] + face->glyph->bitmap.buffer[glyph_idx];
                text.pixels[text_idx] = TQ_MIN(255, sum);
            }
        }

        x_current += glyph_pos[i].x_advance >> 6;
        y_current += glyph_pos[i].y_advance >> 6;
    }

    graphics.renderer.draw_text2(x, y, text);

    mem_free(text.pixels);

    hb_buffer_destroy(buffer);
}

//------------------------------------------------------------------------------
