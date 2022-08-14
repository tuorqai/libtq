
//------------------------------------------------------------------------------

#include "tq_mem.h"

#define STB_IMAGE_IMPLEMENTATION

#define STBI_MALLOC     libtq_malloc
#define STBI_REALLOC    libtq_realloc
#define STBI_FREE       libtq_free

#include <stb_image.h>

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
        0
    );

    if (image.pixels == NULL) {
        libtq_log(LIBTQ_LOG_ERROR, "stbi image loader error: %s\n", stbi_failure_reason());
        return image;
    }

    return image;
}

//------------------------------------------------------------------------------
