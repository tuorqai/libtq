
//------------------------------------------------------------------------------

#include "tq_mem.h"

#define STB_IMAGE_IMPLEMENTATION

#define STBI_MALLOC     libtq_malloc
#define STBI_REALLOC    libtq_realloc
#define STBI_FREE       libtq_free

#include <stb_image.h>

#include "tq_graphics.h"
#include "tq_image_loader.h"
#include "tq_log.h"

//------------------------------------------------------------------------------
// Input stream adapter for stbi

static int stream_read(void *user, char *data, int size)
{
    libtq_stream *stream = (libtq_stream *) user;
    return (int) libtq_stream_read(stream, data, size);
}

static void stream_skip(void *user, int n)
{
    libtq_stream *stream = (libtq_stream *) user;
    intptr_t position = libtq_stream_tell(stream);
    libtq_stream_seek(stream, position + n);
}

static int stream_eof(void *user)
{
    libtq_stream *stream = (libtq_stream *) user;
    intptr_t position = libtq_stream_tell(stream);
    intptr_t size = libtq_stream_size(stream);

    return (position == size);
}

//------------------------------------------------------------------------------

static void copy_pixels_with_key(unsigned char *dst, unsigned char const *src,
    int width, int height, int channels, tq_color key)
{
    for (int y = 0; y < height; y++) {
        size_t row = 4 * width * y;

        for (int x = 0; x < width; x++) {
            size_t src_idx = (channels * width * y) + (channels * x);
            size_t dst_idx = (4 * width * y) + (4 * x);

            unsigned char r, g, b, a;

            switch (channels) {
            case 1:
                r = src[src_idx];
                g = src[src_idx];
                b = src[src_idx];
                a = 255;
                break;
            case 2:
                r = src[src_idx + 0];
                g = src[src_idx + 0];
                b = src[src_idx + 0];
                a = src[src_idx + 1];
                break;
            case 3:
                r = src[src_idx + 0];
                g = src[src_idx + 1];
                b = src[src_idx + 2];
                a = 255;
                break;
            case 4:
                r = src[src_idx + 0];
                g = src[src_idx + 1];
                b = src[src_idx + 2];
                a = src[src_idx + 3];
                break;
            }

            if (r == key.r && g == key.g && b == key.b) {
                dst[dst_idx + 0] = 0;
                dst[dst_idx + 1] = 0;
                dst[dst_idx + 2] = 0;
                dst[dst_idx + 3] = 0;
            } else {
                dst[dst_idx + 0] = r;
                dst[dst_idx + 1] = g;
                dst[dst_idx + 2] = b;
                dst[dst_idx + 3] = a;
            }
        }
    }
}

//------------------------------------------------------------------------------

libtq_image *libtq_create_image(int width, int height, int channels)
{
    size_t volume = width * height * channels;
    libtq_image *image = libtq_calloc(1, sizeof(libtq_image) + volume);

    if (!image) {
        return NULL;
    }

    image->width = width;
    image->height = height;
    image->channels = channels;

    return image;
}

libtq_image *libtq_load_image(libtq_stream *stream)
{
    if (!stream) {
        return NULL;
    }

    size_t stream_size = libtq_stream_size(stream);
    void const *stream_buffer = libtq_stream_buffer(stream);

    if (!stream_buffer || !stream_size) {
        return NULL;
    }

    int width;
    int height;
    int channels;

    unsigned char *pixels = stbi_load_from_callbacks(
        &(stbi_io_callbacks) {
            .read = stream_read,
            .skip = stream_skip,
            .eof = stream_eof,
        },
        stream, &width, &height, &channels, 0
    );

    if (!pixels) {
        libtq_log(LIBTQ_LOG_ERROR, "stbi image loader error: %s\n", stbi_failure_reason());
        return NULL;
    }

    size_t volume = width * height * channels;
    libtq_image *image = libtq_malloc(sizeof(libtq_image) + volume);

    if (!image) {
        stbi_image_free(pixels);
        return NULL;
    }

    image->width = width;
    image->height = height;
    image->channels = channels;
    memcpy(image->pixels, pixels, volume);

    stbi_image_free(pixels);

    libtq_log(LIBTQ_INFO, "Loaded %dx%dx%d image from stream %s.\n",
        width, height, channels, libtq_stream_repr(stream));

    return image;
}

libtq_image *libtq_load_image_with_key(libtq_stream *stream, tq_color key)
{
    if (!stream) {
        return NULL;
    }

    size_t stream_size = libtq_stream_size(stream);
    void const *stream_buffer = libtq_stream_buffer(stream);

    if (!stream_buffer || !stream_size) {
        return NULL;
    }

    int width;
    int height;
    int channels;

    unsigned char *pixels = stbi_load_from_callbacks(
        &(stbi_io_callbacks) {
            .read = stream_read,
            .skip = stream_skip,
            .eof = stream_eof,
        },
        stream, &width, &height, &channels, 0
    );

    if (!pixels) {
        libtq_log(LIBTQ_LOG_ERROR, "stbi image loader error: %s\n", stbi_failure_reason());
        return NULL;
    }

    size_t volume = width * height * 4;
    libtq_image *image = libtq_malloc(sizeof(libtq_image) + volume);

    if (!image) {
        stbi_image_free(pixels);
        return NULL;
    }

    image->width = width;
    image->height = height;
    image->channels = 4;
    copy_pixels_with_key(image->pixels, pixels, width, height, channels, key);

    stbi_image_free(pixels);

    libtq_log(LIBTQ_INFO, "Loaded %dx%dx%d image (keyed) from stream %s.\n",
        width, height, channels, libtq_stream_repr(stream));

    return image;
}

//------------------------------------------------------------------------------
