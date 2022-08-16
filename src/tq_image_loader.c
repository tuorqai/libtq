
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

struct image image_create(unsigned int width, unsigned int height, unsigned int channels)
{
    return (struct image) {
        .pixels = libtq_calloc(1, width * height * channels),
        .width = width,
        .height = height,
        .channels = channels,
    };
}

struct image image_load(libtq_stream *stream)
{
    struct image image = { 0 };

    void const *buffer = libtq_stream_buffer(stream);
    size_t size = libtq_stream_size(stream);

    if (buffer == NULL || size == 0) {
        libtq_log(LIBTQ_LOG_ERROR, "Can't load image from empty stream: %s\n",
            libtq_stream_repr(stream));
        return image;
    }

    tq_color *color_key = libtq_get_color_key();

    image.pixels = stbi_load_from_callbacks(
        &(stbi_io_callbacks) {
            .read = stream_read,
            .skip = stream_skip,
            .eof = stream_eof,
        },
        stream,
        &image.width,
        &image.height,
        &image.channels,
        (color_key) ? 4 : 0
    );

    if (image.pixels == NULL) {
        libtq_log(LIBTQ_LOG_ERROR, "stbi image loader error: %s\n", stbi_failure_reason());
        return image;
    }

    if (color_key) {
        image.channels = 4;

        for (int y = 0; y < image.height; y++) {
            unsigned char *row = image.pixels + (4 * image.width * y);

            for (int x = 0; x < image.width; x++) {
                unsigned char r = row[4 * x + 0];
                unsigned char g = row[4 * x + 1];
                unsigned char b = row[4 * x + 2];

                if (r == color_key->r && g == color_key->g && b == color_key->b) {
                    row[4 * x + 0] = 0;
                    row[4 * x + 1] = 0;
                    row[4 * x + 2] = 0;
                    row[4 * x + 3] = 0;
                }
            }
        }
    }

    return image;
}

//------------------------------------------------------------------------------
