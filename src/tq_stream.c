
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "tq_stream.h"

//------------------------------------------------------------------------------
// File streams

static int64_t file_stream_read(void *data, void *dst, int64_t size)
{
    FILE *file = (FILE *) data;
    return (int64_t) fread(dst, 1, size, file);
}

static int64_t file_stream_seek(void *data, int64_t position)
{
    FILE *file = (FILE *) data;

    if (fseek(file, position, SEEK_SET) == 0) {
        return (int64_t) ftell(file);
    }

    return -1;
}

static int64_t file_stream_tell(void *data)
{
    FILE *file = (FILE *) data;
    return (int64_t) ftell(file);
}

static int64_t file_stream_get_size(void *data)
{
    FILE *file = (FILE *) data;
    int64_t position = (int64_t) ftell(file);

    fseek(file, 0, SEEK_END);
    int64_t size = (int64_t) ftell(file);

    fseek(file, position, SEEK_SET);
    return size;
}

static void file_stream_close(void *data)
{
    fclose((FILE *) data);
}

int64_t file_stream_open(stream_t *stream, char const *path)
{
    FILE *file = fopen(path, "rb");

    if (!file) {
        return -1;
    }

    *stream = (stream_t) {
        .data = file,
        .read = file_stream_read,
        .seek = file_stream_seek,
        .tell = file_stream_tell,
        .get_size = file_stream_get_size,
        .close = file_stream_close,
    };

    return 0;
}

//------------------------------------------------------------------------------
// Memory streams

struct memory_stream_info
{
    uint8_t const *buffer;
    size_t length;
    size_t position;
};

static int64_t memory_stream_read(void *data, void *dst, int64_t size)
{
    struct memory_stream_info *info = (struct memory_stream_info *) data;

    if (info->position > info->length) {
        return -1;
    }

    size_t bytes_left = info->length - info->position;
    size_t bytes_to_copy = (bytes_left < size) ? bytes_left : size;

    memcpy(dst, info->buffer + info->position, bytes_to_copy);
    info->position += bytes_to_copy;
    return bytes_to_copy;
}

static int64_t memory_stream_seek(void *data, int64_t position)
{
    struct memory_stream_info *info = (struct memory_stream_info *) data;

    if (position < 0) {
        info->position = 0;
    } else if (position >= info->length) {
        info->position = info->length;
    } else {
        info->position = position;
    }

    return info->position;
}

static int64_t memory_stream_tell(void *data)
{
    struct memory_stream_info *info = (struct memory_stream_info *) data;
    return info->position;
}

static int64_t memory_stream_get_size(void *data)
{
    struct memory_stream_info *info = (struct memory_stream_info *) data;
    return info->length;
}

static void memory_stream_close(void *data)
{
    free(data);
}

int64_t memory_stream_open(stream_t *stream, uint8_t const *buffer, size_t length)
{
    struct memory_stream_info *info = calloc(1, sizeof(struct memory_stream_info));

    if (!info) {
        return -1;
    }

    *info = (struct memory_stream_info) {
        .buffer = buffer,
        .length = length,
        .position = 0,
    };

    *stream = (stream_t) {
        .data = info,
        .read = memory_stream_read,
        .seek = memory_stream_seek,
        .tell = memory_stream_tell,
        .get_size = memory_stream_get_size,
        .close = memory_stream_close,
    };

    return 0;
}

//------------------------------------------------------------------------------