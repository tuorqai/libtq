
//------------------------------------------------------------------------------

#include <hb-ft.h>

#include "tq_error.h"
#include "tq_log.h"
#include "tq_mem.h"
#include "tq_text.h"

//------------------------------------------------------------------------------

#define INITIAL_FONT_COUNT          16
#define INITIAL_GLYPH_COUNT         256
#define INITIAL_VERTEX_BUFFER_SIZE  256
#define INITIAL_INDEX_BUFFER_SIZE   256

//------------------------------------------------------------------------------

struct font_atlas
{
    int             texture_id;
    int             width;
    int             height;
    int             cursor_x;
    int             cursor_y;
    int             line_height;
    int             x_padding;
    int             y_padding;
    unsigned char   *bitmap;
};

struct font_glyph
{
    unsigned long   codepoint;
    int             s0, t0;
    int             s1, t1;
    float           x_advance;
    float           y_advance;
    int             x_bearing;
    int             y_bearing;
};

struct font
{
    hb_font_t       *font;

    FT_Face         face;
    FT_StreamRec    stream;

    struct font_atlas   atlas;
    struct font_glyph   *glyphs;
    int                 glyph_count;

    float           height;
};

//------------------------------------------------------------------------------

static struct libtq_renderer_impl const *renderer;

static FT_Library       freetype;
static struct font      *fonts;
static int              font_count;

static float            *vertex_buffer;
static int              vertex_buffer_size;

static tq_color         text_outline_color;
static tq_color         text_fill_color;

//------------------------------------------------------------------------------

static unsigned long ft_stream_io(FT_Stream stream,
    unsigned long offset,
    unsigned char *buffer,
    unsigned long count)
{
    if (libtq_stream_seek(stream->descriptor.pointer, offset) == -1) {
        if (count == 0) {
            return 1;
        }

        return 0;
    }

    return libtq_stream_read(stream->descriptor.pointer, buffer, count);
}

static void ft_stream_close(FT_Stream stream)
{
    libtq_stream_close(stream->descriptor.pointer);
}

static int get_font_id(void)
{
    for (int i = 0; i < font_count; i++) {
        if (fonts[i].face == NULL) {
            return i;
        }
    }

    int next_count = TQ_MAX(font_count * 2, INITIAL_FONT_COUNT);
    struct font *next_array = libtq_realloc(fonts,
        sizeof(struct font) * next_count);

    if (!next_array) {
        libtq_out_of_memory();
    }

    int font_id = font_count;

    for (int i = font_id; i < next_count; i++) {
        next_array[i].face = NULL;
        next_array[i].font = NULL;
    }

    font_count = next_count;
    fonts = next_array;

    return font_id;
}

static int get_glyph_id(int font_id)
{
    for (int i = 0; i < fonts[font_id].glyph_count; i++) {
        if (fonts[font_id].glyphs[i].codepoint == 0) {
            return i;
        }
    }

    int next_count = TQ_MAX(fonts[font_id].glyph_count * 2, INITIAL_GLYPH_COUNT);
    struct font_glyph *next_array = libtq_realloc(fonts[font_id].glyphs,
        sizeof(struct font_glyph) * next_count);
    
    if (!next_array) {
        libtq_out_of_memory();
    }

    int glyph_id = fonts[font_id].glyph_count;

    for (int i = glyph_id; i < next_count; i++) {
        next_array[i].codepoint = 0;
    }

    fonts[font_id].glyph_count = next_count;
    fonts[font_id].glyphs = next_array;

    return glyph_id;
}

static int cache_glyph(int font_id, unsigned long codepoint, float x_advance, float y_advance)
{
    for (int i = 0; i < fonts[font_id].glyph_count; i++) {
        if (fonts[font_id].glyphs[i].codepoint == codepoint) {
            return i;
        }
    }

    int glyph_id = get_glyph_id(font_id);

    if (glyph_id == -1) {
        return -1;
    }

    if (FT_Load_Glyph(fonts[font_id].face, codepoint, FT_LOAD_DEFAULT)) {
        return -1;
    }

    if (FT_Render_Glyph(fonts[font_id].face->glyph, FT_RENDER_MODE_NORMAL)) {
        return -1;
    }

    unsigned char *bitmap = fonts[font_id].face->glyph->bitmap.buffer;
    int bitmap_width = fonts[font_id].face->glyph->bitmap.width;
    int bitmap_height = fonts[font_id].face->glyph->bitmap.rows;

    struct font_atlas *atlas = &fonts[font_id].atlas;
    struct font_glyph *glyph = &fonts[font_id].glyphs[glyph_id];

    if (atlas->cursor_x >= (atlas->width - atlas->x_padding - bitmap_width)) {
        atlas->cursor_x = atlas->x_padding;
        atlas->cursor_y += atlas->line_height + atlas->y_padding;

        if (atlas->cursor_y >= (atlas->height - atlas->y_padding - bitmap_height)) {
            atlas->height *= 2;
            atlas->bitmap = libtq_realloc(atlas->bitmap, atlas->width * atlas->height);

            if (!atlas->bitmap) {
                libtq_out_of_memory();
            }

            for (int y = atlas->height / 2; y < atlas->height; y++) {
                for (int x = 0; x < atlas->width; x++) {
                    atlas->bitmap[y * atlas->width + x] = 0;
                }
            }

            renderer->delete_texture(atlas->texture_id);
            atlas->texture_id = renderer->create_texture(atlas->width, atlas->height,
                LIBTQ_GRAYSCALE);
            renderer->update_texture(atlas->texture_id, 0, 0,
                atlas->width, atlas->height, atlas->bitmap);
        }

        atlas->line_height = 0;
    }

    for (int y = 0; y < bitmap_height; y++) {
        int atlas_y = atlas->cursor_y + y;

        for (int x = 0; x < bitmap_width; x++) {
            int atlas_x = atlas->cursor_x + x;
    
            atlas->bitmap[atlas_y * atlas->width + atlas_x] = bitmap[y * bitmap_width + x];
        }
    }

    renderer->update_texture(atlas->texture_id,
        atlas->cursor_x, atlas->cursor_y,
        bitmap_width, bitmap_height, bitmap);

    glyph->codepoint = codepoint;
    glyph->s0 = atlas->cursor_x;
    glyph->t0 = atlas->cursor_y;
    glyph->s1 = atlas->cursor_x + bitmap_width;
    glyph->t1 = atlas->cursor_y + bitmap_height;
    glyph->x_advance = x_advance;
    glyph->y_advance = y_advance;
    glyph->x_bearing = fonts[font_id].face->glyph->bitmap_left;
    glyph->y_bearing = fonts[font_id].face->glyph->bitmap_top;

    atlas->cursor_x += bitmap_width + atlas->x_padding;

    if (atlas->line_height < bitmap_height) {
        atlas->line_height = bitmap_height;
    }

    return glyph_id;
}

static float *maintain_vertex_buffer(int required_size)
{
    if (vertex_buffer_size > required_size) {
        return vertex_buffer;
    }

    int next_size = TQ_MAX(vertex_buffer_size * 2, INITIAL_VERTEX_BUFFER_SIZE);

    while (next_size < required_size) {
        next_size *= 2;
    }

    float *next_buffer = libtq_realloc(vertex_buffer, sizeof(float) * next_size);
    
    if (!next_buffer) {
        libtq_out_of_memory();
    }

    vertex_buffer = next_buffer;
    vertex_buffer_size = next_size;

    return vertex_buffer;
}

//------------------------------------------------------------------------------

void text_initialize(struct libtq_renderer_impl const *_renderer)
{
    renderer = _renderer;

    fonts = NULL;
    font_count = 0;

    FT_Error error = FT_Init_FreeType(&freetype);

    if (error) {
        libtq_error("Failed to initialize Freetype library. Reason: %s\n", FT_Error_String(error));
    }

    vertex_buffer = NULL;
    vertex_buffer_size = 0;
}

void text_terminate(void)
{
    libtq_free(vertex_buffer);

    for (int i = 0; i < font_count; i++) {
        text_delete_font(i);
    }

    libtq_free(fonts);
}

int text_load_font(libtq_stream *stream, float pt, int weight)
{
    int font_id = get_font_id();

    if (font_id == -1) {
        libtq_stream_close(stream);
        return -1;
    }

    memset(&fonts[font_id], 0, sizeof(struct font));

    fonts[font_id].stream.base = NULL;
    fonts[font_id].stream.size = libtq_stream_size(stream);
    fonts[font_id].stream.pos = libtq_stream_tell(stream);
    fonts[font_id].stream.descriptor.pointer = stream;
    fonts[font_id].stream.pathname.pointer = (void *) libtq_stream_repr(stream);
    fonts[font_id].stream.read = ft_stream_io;
    fonts[font_id].stream.close = ft_stream_close;

    FT_Open_Args args = { 0 };

    args.flags = FT_OPEN_STREAM;
    args.stream = &fonts[font_id].stream;

    FT_Error error = FT_Open_Face(freetype, &args, 0, &fonts[font_id].face);

    if (error) {
        libtq_log(LIBTQ_LOG_WARNING, "Failed to open font %s. Reason: %s\n",
            libtq_stream_repr(stream), FT_Error_String(error));

        fonts[font_id].face = NULL;
        return -1;
    }

    FT_Set_Char_Size(fonts[font_id].face, 0, (int) (pt * 64.0f), 0, 0);

    fonts[font_id].font = hb_ft_font_create_referenced(fonts[font_id].face);

    if (!fonts[font_id].font) {
        FT_Done_Face(fonts[font_id].face);
        fonts[font_id].face = NULL;

        return -1;
    }

    int width = 4096;
    int height = 16;

    while (height < (pt * 4)) {
        height *= 2;
    }

    fonts[font_id].atlas.texture_id = renderer->create_texture(width, height,
        LIBTQ_GRAYSCALE);

    fonts[font_id].atlas.bitmap = libtq_malloc(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            fonts[font_id].atlas.bitmap[y * width + x] = 0;
        }
    }

    if (fonts[font_id].atlas.texture_id == -1 || !fonts[font_id].atlas.bitmap) {
        libtq_free(fonts[font_id].atlas.bitmap);
        hb_font_destroy(fonts[font_id].font);

        fonts[font_id].face = NULL;
        fonts[font_id].font = NULL;

        return -1;
    }

    fonts[font_id].atlas.width = width;
    fonts[font_id].atlas.height = height;

    fonts[font_id].atlas.x_padding = 4;
    fonts[font_id].atlas.y_padding = 4;

    fonts[font_id].atlas.cursor_x = fonts[font_id].atlas.x_padding;
    fonts[font_id].atlas.cursor_y = fonts[font_id].atlas.y_padding;

    fonts[font_id].atlas.line_height = 0;

    for (int i = 0x20; i <= 0xFF; i++) {
        hb_codepoint_t codepoint;

        if (!hb_font_get_glyph(fonts[font_id].font, i, 0, &codepoint)) {
            continue;
        }

        hb_position_t x_advance, y_advance;
        hb_font_get_glyph_advance_for_direction(fonts[font_id].font, codepoint,
            HB_DIRECTION_LTR, &x_advance, &y_advance);

        cache_glyph(font_id, codepoint, x_advance / 64.0f, y_advance / 64.0f);
    }

    FT_F26Dot6 ascender = fonts[font_id].face->size->metrics.ascender;
    FT_F26Dot6 descender = fonts[font_id].face->size->metrics.descender;

    fonts[font_id].height = (ascender - descender) / 64.0f;

    return font_id;
}

int text_load_font_from_file(char const *path, float pt, int weight)
{
    libtq_stream *stream = libtq_open_file_stream(path);

    if (!stream) {
        return -1;
    }

    return text_load_font(stream, pt, weight);
}

int text_load_font_from_memory(void const *buffer, size_t size, float pt, int weight)
{
    libtq_stream *stream = libtq_open_memory_stream(buffer, size);

    if (!stream) {
        return -1;
    }

    return text_load_font(stream, pt, weight);
}

void text_delete_font(int font_id)
{
    if (font_id < 0 || font_id >= font_count || !fonts[font_id].face) {
        return;
    }

    libtq_free(fonts[font_id].glyphs);
    libtq_free(fonts[font_id].atlas.bitmap);
    renderer->delete_texture(fonts[font_id].atlas.texture_id);
    hb_font_destroy(fonts[font_id].font);

    libtq_stream_close(fonts[font_id].stream.descriptor.pointer);

    fonts[font_id].face = NULL;
    fonts[font_id].font = NULL;
}

int text_get_font_texture(int font_id)
{
    if (font_id < 0 || font_id >= font_count || !fonts[font_id].face) {
        return -1;
    }

    return fonts[font_id].atlas.texture_id;
}

void text_draw_text(int font_id, float x, float y, char const *text)
{
    if (font_id < 0 || font_id >= font_count || !fonts[font_id].face) {
        return;
    }

    hb_buffer_t *buffer = hb_buffer_create();

    hb_buffer_add_utf8(buffer, text, -1, 0, -1);
    hb_buffer_guess_segment_properties(buffer);

    hb_shape(fonts[font_id].font, buffer, NULL, 0);

    unsigned int length = hb_buffer_get_length(buffer);
    hb_glyph_info_t *info = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *pos = hb_buffer_get_glyph_positions(buffer, NULL);

    float x_offset = 0.0f;

    if (hb_buffer_get_direction(buffer) == HB_DIRECTION_RTL) {
        for (unsigned int i = 0; i < length; i++) {
            x_offset -= pos[i].x_advance / 64.0f;
        }
    }

    float x_current = x + x_offset;
    float y_current = y;

    float *v = maintain_vertex_buffer(24 * length);

    int quad_count = 0;

    for (unsigned int i = 0; i < length; i++) {
        int glyph_id = cache_glyph(font_id, info[i].codepoint,
            pos[i].x_advance / 64.0f, pos[i].y_advance / 64.0f);

        if (glyph_id == -1) {
            continue;
        }

        struct font_glyph *glyph = &fonts[font_id].glyphs[glyph_id];

        float x0 = x_current + glyph->x_bearing;
        float y0 = y_current - glyph->y_bearing + fonts[font_id].height;
        float x1 = x0 + glyph->s1 - glyph->s0;
        float y1 = y0 + glyph->t1 - glyph->t0;

        float s0 = glyph->s0 / (float) fonts[font_id].atlas.width;
        float t0 = glyph->t0 / (float) fonts[font_id].atlas.height;
        float s1 = glyph->s1 / (float) fonts[font_id].atlas.width;
        float t1 = glyph->t1 / (float) fonts[font_id].atlas.height;

        *v++ = x0;  *v++ = y0;  *v++ = s0;  *v++ = t0;
        *v++ = x1;  *v++ = y0;  *v++ = s1;  *v++ = t0;
        *v++ = x1;  *v++ = y1;  *v++ = s1;  *v++ = t1;
        *v++ = x1;  *v++ = y1;  *v++ = s1;  *v++ = t1;
        *v++ = x0;  *v++ = y1;  *v++ = s0;  *v++ = t1;
        *v++ = x0;  *v++ = y0;  *v++ = s0;  *v++ = t0;

        x_current += glyph->x_advance;
        y_current += glyph->y_advance;

        quad_count++;
    }

    renderer->bind_texture(fonts[font_id].atlas.texture_id);
    renderer->set_draw_color(text_fill_color);
    renderer->draw_font(vertex_buffer, 6 * quad_count);

    hb_buffer_destroy(buffer);
}

void text_set_outline_color(tq_color outline_color)
{
    text_outline_color = outline_color;
}

void text_set_fill_color(tq_color fill_color)
{
    text_fill_color = fill_color;
}

//------------------------------------------------------------------------------
