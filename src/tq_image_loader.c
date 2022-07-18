
//------------------------------------------------------------------------------

#include "tq_mem.h"

#define STB_IMAGE_IMPLEMENTATION

#define STBI_MALLOC     mem_malloc
#define STBI_REALLOC    mem_realloc
#define STBI_FREE       mem_free

#include <stb_image.h>

#include "tq_image_loader.h"
#include "tq_log.h"

//------------------------------------------------------------------------------
// Input stream adapter for stbi

static int stream_read(void *user, char *data, int size)
{
    int stream_id = *(int *) user;
    return (int) tq_istream_read(stream_id, data, size);
}

static void stream_skip(void *user, int n)
{
    int stream_id = *(int *) user;
    int64_t position = tq_istream_tell(stream_id);
    tq_istream_seek(stream_id, position + n);
}

static int stream_eof(void *user)
{
    int stream_id = *(int *) user;
    int64_t position = tq_istream_tell(stream_id);
    int64_t size = tq_istream_size(stream_id);

    return (position == size);
}

//------------------------------------------------------------------------------

struct image image_create(unsigned int width, unsigned int height, unsigned int channels)
{
    return (struct image) {
        .pixels = mem_calloc(1, width * height * channels),
        .width = width,
        .height = height,
        .channels = channels,
    };
}

struct image image_load(int stream_id)
{
    struct image image = { 0 };

    void const *buffer = input_stream_buffer(stream_id);
    size_t size = input_stream_size(stream_id);

    if (buffer == NULL || size == 0) {
        log_error("Can't load image from empty stream: %s\n", tq_istream_repr(stream_id));
        return image;
    }

    stbi_io_callbacks callbacks = {
        .read = stream_read,
        .skip = stream_skip,
        .eof = stream_eof,
    };

    image.pixels = stbi_load_from_callbacks(
        &(stbi_io_callbacks) {
            .read = stream_read,
            .skip = stream_skip,
            .eof = stream_eof,
        },
        &stream_id,
        &image.width,
        &image.height,
        &image.channels,
        0
    );

    if (image.pixels == NULL) {
        log_error("stbi image loader error: %s\n", stbi_failure_reason());
        return image;
    }

    return image;
}

//------------------------------------------------------------------------------
