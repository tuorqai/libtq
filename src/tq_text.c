
//------------------------------------------------------------------------------
// Copyright (c) 2021-2022 tuorqai
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//------------------------------------------------------------------------------

#if defined(TQ_USE_HARFBUZZ)
#   include <hb-ft.h>
#else
#   include <ft2build.h>
#   include FT_FREETYPE_H
#endif

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

/**
 * Texture atlas where glyphs are rendered to.
 */
struct font_atlas
{
    int texture_id;                 // texture identifier
    int width;                      // texture width
    int height;                     // texture height
    int cursor_x;                   // x position where next glyph will be added
    int cursor_y;                   // y position where next glyph will be added
    int line_height;                // max height of current line in the atlas
    int x_padding;                  // min x padding between glyphs
    int y_padding;                  // min y padding between glyphs
    unsigned char *bitmap;          // locally stored copy of bitmap
};

/**
 * Individual glyph descriptor.
 */
struct font_glyph
{
    unsigned long codepoint;        // glyph codepoint (unicode?)
    int s0, t0;                     // top-left position in atlas
    int s1, t1;                     // bottom-right position in atlas
    float x_advance;                // how much x to add after printing the glyph
    float y_advance;                // how much y to add (usually 0)
    int x_bearing;                  // additional x offset
    int y_bearing;                  // additional y offset (?)
};

/**
 * Font object.
 */
struct font
{
#if defined(TQ_USE_HARFBUZZ)
    hb_font_t *font;                // harfbuzz font handle
#endif

    FT_Face face;                   // FreeType font handle
    FT_StreamRec stream;            // FreeType font handle
    struct font_atlas atlas;        // texture atlas (TODO: multiple atlases)
    struct font_glyph *glyphs;      // dynamic array of glyphs
    int glyph_count;                // number of items in glyph array
    float height;                   // general height of font (glyphs may be taller)
};

/**
 * Private data for [text] module.
 */
struct tq_text_priv
{
    tq_renderer_impl *renderer;     // pointer to renderer
    FT_Library freetype;            // FreeType object
    struct font *fonts;             // dynamic array of font objects
    int font_count;                 // number of items in font array
    float *vertex_buffer;           // dynamic array of vertex data
    int vertex_buffer_size;         // size of vertex data
    tq_color text_color;            // basic color
    tq_color outline_color;         // outline color (not implemented yet)
};

static struct tq_text_priv priv;

//------------------------------------------------------------------------------

/**
 * io() callback for FreeType stream.
 */
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

/**
 * close() callback for FreeType stream.
 */
static void ft_stream_close(FT_Stream stream)
{
    libtq_stream_close(stream->descriptor.pointer);
}

/**
 * Get free font index.
 */
static int get_font_id(void)
{
    for (int i = 0; i < priv.font_count; i++) {
        if (priv.fonts[i].face == NULL) {
            return i;
        }
    }

    int next_count = TQ_MAX(priv.font_count * 2, INITIAL_FONT_COUNT);
    struct font *next_array = libtq_realloc(priv.fonts, sizeof(struct font) * next_count);

    if (!next_array) {
        libtq_out_of_memory();
    }

    int font_id = priv.font_count;

    for (int i = font_id; i < next_count; i++) {
        next_array[i].face = NULL;
#if defined(TQ_USE_HARFBUZZ)
        next_array[i].font = NULL;
#endif
    }

    priv.font_count = next_count;
    priv.fonts = next_array;

    return font_id;
}

/**
 * Get free glyph index for specific font.
 */
static int get_glyph_id(int font_id)
{
    for (int i = 0; i < priv.fonts[font_id].glyph_count; i++) {
        if (priv.fonts[font_id].glyphs[i].codepoint == 0) {
            return i;
        }
    }

    int next_count = TQ_MAX(priv.fonts[font_id].glyph_count * 2, INITIAL_GLYPH_COUNT);
    struct font_glyph *next_array = libtq_realloc(priv.fonts[font_id].glyphs,
        sizeof(struct font_glyph) * next_count);
    
    if (!next_array) {
        libtq_out_of_memory();
    }

    int glyph_id = priv.fonts[font_id].glyph_count;

    for (int i = glyph_id; i < next_count; i++) {
        next_array[i].codepoint = 0;
    }

    priv.fonts[font_id].glyph_count = next_count;
    priv.fonts[font_id].glyphs = next_array;

    return glyph_id;
}

/**
 * Render the glyph (if it isn't already) and return its index.
 * TODO: consider optimizing glyph indexing.
 */
#if defined(TQ_USE_HARFBUZZ)
static int cache_glyph(int font_id, unsigned long codepoint, float x_advance, float y_advance)
#else
static int cache_glyph(int font_id, unsigned long codepoint)
#endif
{
    struct font *font = &priv.fonts[font_id];

    for (int i = 0; i < font->glyph_count; i++) {
        if (font->glyphs[i].codepoint == codepoint) {
            return i;
        }
    }

    int glyph_id = get_glyph_id(font_id);

    if (glyph_id == -1) {
        return -1;
    }

    if (FT_Load_Glyph(font->face, codepoint, FT_LOAD_DEFAULT)) {
        return -1;
    }

    if (FT_Render_Glyph(font->face->glyph, FT_RENDER_MODE_NORMAL)) {
        return -1;
    }

    unsigned char *bitmap = font->face->glyph->bitmap.buffer;
    int bitmap_width = font->face->glyph->bitmap.width;
    int bitmap_height = font->face->glyph->bitmap.rows;

    struct font_atlas *atlas = &font->atlas;
    struct font_glyph *glyph = &font->glyphs[glyph_id];

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

            priv.renderer->delete_texture(atlas->texture_id);
            atlas->texture_id = priv.renderer->create_texture(
                atlas->width, atlas->height, LIBTQ_GRAYSCALE
            );
            priv.renderer->update_texture(
                atlas->texture_id, 0, 0, atlas->width, atlas->height, atlas->bitmap
            );
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

    priv.renderer->update_texture(atlas->texture_id,
        atlas->cursor_x, atlas->cursor_y,
        bitmap_width, bitmap_height, bitmap);

    glyph->codepoint = codepoint;
    glyph->s0 = atlas->cursor_x;
    glyph->t0 = atlas->cursor_y;
    glyph->s1 = atlas->cursor_x + bitmap_width;
    glyph->t1 = atlas->cursor_y + bitmap_height;

#if defined(TQ_USE_HARFBUZZ)
    glyph->x_advance = x_advance;
    glyph->y_advance = y_advance;
#else
    glyph->x_advance = font->face->glyph->advance.x / 64.0f;
    glyph->y_advance = font->face->glyph->advance.y / 64.0f;
#endif

    glyph->x_bearing = font->face->glyph->bitmap_left;
    glyph->y_bearing = font->face->glyph->bitmap_top;

    atlas->cursor_x += bitmap_width + atlas->x_padding;

    if (atlas->line_height < bitmap_height) {
        atlas->line_height = bitmap_height;
    }

    return glyph_id;
}

/**
 * Get vertex buffer pointer, grow it if necessary.
 */
static float *maintain_vertex_buffer(int required_size)
{
    if (priv.vertex_buffer_size > required_size) {
        return priv.vertex_buffer;
    }

    int next_size = TQ_MAX(priv.vertex_buffer_size * 2, INITIAL_VERTEX_BUFFER_SIZE);

    while (next_size < required_size) {
        next_size *= 2;
    }

    float *next_buffer = libtq_realloc(priv.vertex_buffer, sizeof(float) * next_size);
    
    if (!next_buffer) {
        libtq_out_of_memory();
    }

    priv.vertex_buffer = next_buffer;
    priv.vertex_buffer_size = next_size;

    return priv.vertex_buffer;
}

#if !defined(TQ_USE_HARFBUZZ)
/**
 * My attempt to create UTF-8 -> code point conversion.
 * Very simplistic, used when harfbuzz is disabled.
 */
static int conv_utf8(void const *s, unsigned long *char_code)
{
    unsigned char const *u = s;

    if ((u[0] >> 7) == 0x00) {
        *char_code = (u[0] & 0x7f);
        return 0;
    } else if (((u[0] >> 5) == 0x06) && ((u[1] >> 6) == 0x02)) {
        *char_code = ((u[0] & 0x1f) << 6) | (u[1] & 0x3f);
        return 1;
    } else if (((u[0] >> 4) == 0x0e) && ((u[1] >> 6) == 0x02) && ((u[2] >> 6) == 0x02)) {
        *char_code = ((u[0] & 0x0f) << 12) | ((u[1] & 0x3f) << 6) | (u[2] & 0x3f);
        return 2;
    } else if (((u[0] >> 3) == 0x1e) && ((u[1] >> 6) == 0x02) && ((u[2] >> 6) == 0x02) && ((u[3] >> 6) == 0x02)) {
        *char_code = ((u[0] & 0x07) << 18) | ((u[1] & 0x3f) << 12) | ((u[2] & 0x3f) << 6) | (u[3] & 0x3f);
        return 3;
    }

    *char_code = 0;
    return 0;
}
#endif

//------------------------------------------------------------------------------

/**
 * Initialize [text] module.
 */
void tq_initialize_text(tq_renderer_impl *renderer)
{
    priv.renderer = renderer;

    priv.fonts = NULL;
    priv.font_count = 0;

    FT_Error error = FT_Init_FreeType(&priv.freetype);

    if (error) {
        //libtq_error("Failed to initialize Freetype library. Reason: %s\n", FT_Error_String(error));
        libtq_error("Failed to initialize Freetype library.\n");
    }

    priv.vertex_buffer = NULL;
    priv.vertex_buffer_size = 0;

#if !defined(TQ_USE_HARFBUZZ)
    libtq_log(LIBTQ_WARNING, "HarfBuzz support was disabled during compile time.\n");
    libtq_log(LIBTQ_WARNING, "Text rendering functions are limited to ASCII charset.\n");
#endif
}

/**
 * Terminate [text] module.
 */
void tq_terminate_text(void)
{
    libtq_free(priv.vertex_buffer);

    for (int i = 0; i < priv.font_count; i++) {
        tq_delete_font((tq_font) { i });
    }

    libtq_free(priv.fonts);
}

/**
 * Loads font.
 * TODO: Weight is not implemented yet.
 */
static int load_font(libtq_stream *stream, float pt, int weight)
{
    int font_id = get_font_id();

    if (font_id == -1) {
        libtq_stream_close(stream);
        return -1;
    }

    struct font *font = &priv.fonts[font_id];
    memset(font, 0, sizeof(struct font));

    font->stream.base = NULL;
    font->stream.size = libtq_stream_size(stream);
    font->stream.pos = libtq_stream_tell(stream);
    font->stream.descriptor.pointer = stream;
    font->stream.pathname.pointer = (void *) libtq_stream_repr(stream);
    font->stream.read = ft_stream_io;
    font->stream.close = ft_stream_close;

    FT_Open_Args args = {
        .flags = FT_OPEN_STREAM,
        .stream = &font->stream,
    };

    FT_Error error = FT_Open_Face(priv.freetype, &args, 0, &font->face);

    if (error) {
        // libtq_log(LIBTQ_LOG_WARNING, "Failed to open font %s. Reason: %s\n",
        //     libtq_stream_repr(stream), FT_Error_String(error));
        libtq_log(LIBTQ_LOG_WARNING, "Failed to open font %s.\n",
            libtq_stream_repr(stream));

        font->face = NULL;
        return -1;
    }

    FT_Set_Char_Size(font->face, 0, (int) (pt * 64.0f), 0, 0);

#if defined(TQ_USE_HARFBUZZ)
    font->font = hb_ft_font_create_referenced(font->face);

    if (!font->font) {
        FT_Done_Face(font->face);
        font->face = NULL;

        return -1;
    }
#endif

    int width = 4096;
    int height = 16;

    while (height < (pt * 4)) {
        height *= 2;
    }

    font->atlas.texture_id = priv.renderer->create_texture(width, height, LIBTQ_GRAYSCALE);
    font->atlas.bitmap = libtq_malloc(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            font->atlas.bitmap[y * width + x] = 0;
        }
    }

    if (font->atlas.texture_id == -1 || !font->atlas.bitmap) {
        libtq_free(font->atlas.bitmap);
        font->face = NULL;

#if defined(TQ_USE_HARFBUZZ)
        hb_font_destroy(font->font);
        font->font = NULL;
#endif

        return -1;
    }

    font->atlas.width = width;
    font->atlas.height = height;

    font->atlas.x_padding = 4;
    font->atlas.y_padding = 4;

    font->atlas.cursor_x = font->atlas.x_padding;
    font->atlas.cursor_y = font->atlas.y_padding;

    font->atlas.line_height = 0;

    for (int i = 0x20; i <= 0xFF; i++) {
#if defined(TQ_USE_HARFBUZZ)
        hb_codepoint_t codepoint;

        if (!hb_font_get_glyph(font->font, i, 0, &codepoint)) {
            continue;
        }

        hb_position_t x_advance, y_advance;
        hb_font_get_glyph_advance_for_direction(font->font, codepoint,
            HB_DIRECTION_LTR, &x_advance, &y_advance);

        cache_glyph(font_id, codepoint, x_advance / 64.0f, y_advance / 64.0f);
#else
        FT_UInt char_index = FT_Get_Char_Index(font->face, i);

        if (!char_index) {
            continue;
        }

        cache_glyph(font_id, char_index);
#endif
    }

    FT_F26Dot6 ascender = font->face->size->metrics.ascender;
    FT_F26Dot6 descender = font->face->size->metrics.descender;

    font->height = (ascender - descender) / 64.0f;

    return font_id;
}

/**
 * API entry: tq_load_font_from_file()
 */
tq_font tq_load_font_from_file(char const *path, float pt, int weight)
{
    libtq_stream *stream = libtq_open_file_stream(path);

    if (!stream) {
        return (tq_font) { -1 };
    }

    return (tq_font) { load_font(stream, pt, weight) };
}

/**
 * API entry: tq_load_font_from_memory()
 */
tq_font tq_load_font_from_memory(uint8_t const *buffer, size_t size, float pt, int weight)
{
    libtq_stream *stream = libtq_open_memory_stream(buffer, size);

    if (!stream) {
        return (tq_font) { -1 };
    }

    return (tq_font) { load_font(stream, pt, weight) };
}

/**
 * API entry: tq_delete_font()
 */
void tq_delete_font(tq_font font)
{
    if (font.id < 0 || font.id >= priv.font_count) {
        return;
    }

    struct font *fontp = &priv.fonts[font.id];

    if (!fontp->face) {
        return;
    }

    libtq_free(fontp->glyphs);
    libtq_free(fontp->atlas.bitmap);
    priv.renderer->delete_texture(fontp->atlas.texture_id);

#if defined(TQ_USE_HARFBUZZ)
    hb_font_destroy(fontp->font);
    fontp->font = NULL;
#endif

    libtq_stream_close(fontp->stream.descriptor.pointer);
    fontp->face = NULL;
}

/**
 * API entry: tq_get_font_texture()
 */
tq_texture tq_get_font_texture(tq_font font)
{
    if (font.id < 0 || font.id >= priv.font_count || !priv.fonts[font.id].face) {
        return (tq_texture) { -1 };
    }

    return (tq_texture) { priv.fonts[font.id].atlas.texture_id };
}

/**
 * API entry: tq_draw_text()
 */
void tq_draw_text(tq_font font, tq_vec2f position, char const *text)
{
    if (font.id < 0 || font.id >= priv.font_count || !priv.fonts[font.id].face) {
        return;
    }

    struct font *fontp = &priv.fonts[font.id];

    if (!fontp->face) {
        return;
    }

    float x_offset = 0.0f;
    unsigned int length = 0;
    int quad_count = 0;

    // TODO: add bidi

#if defined(TQ_USE_HARFBUZZ)
    hb_buffer_t *buffer = hb_buffer_create();

    hb_buffer_add_utf8(buffer, text, -1, 0, -1);
    hb_buffer_guess_segment_properties(buffer);

    hb_shape(fontp->font, buffer, NULL, 0);

    length = hb_buffer_get_length(buffer);
    hb_glyph_info_t *info = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *pos = hb_buffer_get_glyph_positions(buffer, NULL);

    // if (hb_buffer_get_direction(buffer) == HB_DIRECTION_RTL) {
    //     for (unsigned int i = 0; i < length; i++) {
    //         x_offset -= pos[i].x_advance / 64.0f;
    //     }
    // }
#else
    length = strlen(text);
#endif

    float x_current = position.x + x_offset;
    float y_current = position.y;

    float *v = maintain_vertex_buffer(24 * length);

    for (unsigned int i = 0; i < length; i++) {
        // Special case for newline character.
        if (text[i] == '\n') {
            x_current = position.x + x_offset;
            y_current += fontp->height;
            continue;
        }

#if defined(TQ_USE_HARFBUZZ)
        float x_adv = pos[i].x_advance / 64.0f;
        float y_adv = pos[i].y_advance / 64.0f;
        int glyph_id = cache_glyph(font.id, info[i].codepoint, x_adv, y_adv);
#else
        unsigned long char_code;
        i += conv_utf8(&text[i], &char_code);

        FT_UInt char_index = FT_Get_Char_Index(fontp->face, char_code);
        int glyph_id = cache_glyph(font.id, char_index);
#endif

        if (glyph_id == -1) {
            continue;
        }

        struct font_glyph *glyph = &fontp->glyphs[glyph_id];

        float x0 = x_current + glyph->x_bearing;
        float y0 = y_current - glyph->y_bearing + fontp->height;
        float x1 = x0 + glyph->s1 - glyph->s0;
        float y1 = y0 + glyph->t1 - glyph->t0;

        float s0 = glyph->s0 / (float) fontp->atlas.width;
        float t0 = glyph->t0 / (float) fontp->atlas.height;
        float s1 = glyph->s1 / (float) fontp->atlas.width;
        float t1 = glyph->t1 / (float) fontp->atlas.height;

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

#if defined(TQ_USE_HARFBUZZ)
    hb_buffer_destroy(buffer);
#endif

    priv.renderer->bind_texture(fontp->atlas.texture_id);
    priv.renderer->set_draw_color(priv.text_color);
    priv.renderer->draw_font(priv.vertex_buffer, 6 * quad_count);
}

/**
 * API entry: tq_print_text()
 */
void tq_print_text(tq_font font, tq_vec2f position, char const *fmt, ...)
{
    static int buffer_size = 0;
    static char *buffer = NULL;

    va_list ap;

    va_start(ap, fmt);
    {
        int bytes_required = vsnprintf(buffer, buffer_size, fmt, ap);

        if (bytes_required >= buffer_size) {
            if (buffer_size == 0) {
                buffer_size = 64;
            }

            while (buffer_size < (bytes_required + 1)) {
                buffer_size *= 2;
            }

            buffer = libtq_realloc(buffer, buffer_size);
            vsnprintf(buffer, buffer_size, fmt, ap);
        }
    }
    va_end(ap);

    tq_draw_text(font, position, buffer);
}

/**
 * Called from [graphics] on draw color change.
 */
void tq_set_text_color(tq_color text_color)
{
    priv.text_color = text_color;
}

/**
 * Called from [graphics] on outline color change.
 */
void tq_set_text_outline_color(tq_color outline_color)
{
    priv.outline_color = outline_color;
}

//------------------------------------------------------------------------------
