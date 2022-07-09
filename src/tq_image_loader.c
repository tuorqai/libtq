
//------------------------------------------------------------------------------

#include "tq_mem.h"

#define STB_IMAGE_IMPLEMENTATION

#define STBI_MALLOC     tq_mem_alloc
#define STBI_REALLOC    tq_mem_realloc
#define STBI_FREE       tq_mem_free

#include <stb_image.h>

#include "tq_image_loader.h"
#include "tq_log.h"

//------------------------------------------------------------------------------
// Input stream adapter for stbi

static int stream_read(void *user, char *data, int size)
{
    int32_t stream_id = ((tq_image_t *) user)->stream_id;
    return (int) tq_istream_read(stream_id, data, size);
}

static void stream_skip(void *user, int n)
{
    int32_t stream_id = ((tq_image_t *) user)->stream_id;
    int64_t position = tq_istream_tell(stream_id);
    tq_istream_seek(stream_id, position + n);
}

static int stream_eof(void *user)
{
    int32_t stream_id = ((tq_image_t *) user)->stream_id;
    int64_t position = tq_istream_tell(stream_id);
    int64_t size = tq_istream_size(stream_id);

    return (position == size);
}

//------------------------------------------------------------------------------

tq_image_t *tq_image_load(int32_t stream_id)
{
    uint8_t const *buffer = tq_istream_buffer(stream_id);
    size_t size = tq_istream_size(stream_id);

    if (buffer == NULL || size == 0) {
        tq_log_error("Can't load image from empty stream: %s\n", tq_istream_repr(stream_id));
        return NULL;
    }

    stbi_io_callbacks callbacks = {
        .read = stream_read,
        .skip = stream_skip,
        .eof = stream_eof,
    };

    tq_image_t *image = tq_mem_alloc(sizeof(tq_image_t));

    int width;
    int height;
    int components;

    image->stream_id = stream_id;
    image->pixels = stbi_load_from_callbacks(&callbacks, image,
        &width, &height, &components, 0);

    if (image->pixels == NULL) {
        tq_log_error("stbi image loader error: %s\n", stbi_failure_reason());
        tq_mem_free(image);
        return NULL;
    }

    image->width = (unsigned int) width;
    image->height = (unsigned int) height;
    image->pixel_format = (unsigned int) components;

    return image;
}

tq_image_t *tq_image_cut(tq_image_t *src, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    unsigned int n = src->pixel_format;

    if (n == 0) {
        tq_log_error("tq_image_cut(): Unknown pixel format.\n");
        return NULL;
    }

    tq_image_t *image = tq_mem_alloc(sizeof(tq_image_t));

    image->stream_id = -1;
    image->pixels = tq_mem_alloc(w * h * n);

    for (uint32_t r = 0; r < h; r++) {
        memcpy(&image->pixels[r * w], &src->pixels[r * w + x], w * n);
    }

    image->width = w;
    image->height = h;
    image->pixel_format = src->pixel_format;

    return image;
}

void tq_image_destroy(tq_image_t *image)
{
    tq_mem_free(image->pixels);
    tq_mem_free(image);
}

//------------------------------------------------------------------------------
