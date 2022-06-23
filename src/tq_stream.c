
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "tq_stream.h"

//------------------------------------------------------------------------------
// File streams

struct file_stream_info
{
    FILE *handle;
    size_t size;
    uint8_t *buffer;
    char path[256];
};

static char const *file_stream_identifier(void *data)
{
    struct file_stream_info *info = (struct file_stream_info *) data;
    return info->path;
}

static int64_t file_stream_read(void *data, void *dst, int64_t size)
{
    struct file_stream_info *info = (struct file_stream_info *) data;
    return (int64_t) fread(dst, 1, size, info->handle);
}

static int64_t file_stream_seek(void *data, int64_t position)
{
    struct file_stream_info *info = (struct file_stream_info *) data;

    if (fseek(info->handle, position, SEEK_SET) == 0) {
        return (int64_t) ftell(info->handle);
    }

    return -1;
}

static int64_t file_stream_tell(void *data)
{
    struct file_stream_info *info = (struct file_stream_info *) data;
    return (int64_t) ftell(info->handle);
}

static int64_t file_stream_get_size(void *data)
{
    struct file_stream_info *info = (struct file_stream_info *) data;

    if (info->size == -1) {
        int64_t prev_position = (int64_t) ftell(info->handle);

        fseek(info->handle, 0, SEEK_END);
        info->size = (int64_t) ftell(info->handle);

        fseek(info->handle, prev_position, SEEK_SET);
    }

    return info->size;
}

static void const *file_stream_buffer(void *data)
{
    struct file_stream_info *info = (struct file_stream_info *) data;

    if (info->buffer == NULL) {
        size_t size = file_stream_get_size(data);

        uint8_t *buffer = malloc(size);

        if (!buffer) {
            return NULL;
        }

        int64_t prev_position = (int64_t) ftell(info->handle);

        fseek(info->handle, 0, SEEK_SET);
        size_t read = fread(buffer, size, 1, info->handle);

        if (read == 1) {
            info->buffer = buffer;
        } else {
            free(buffer);
        }

        fseek(info->handle, prev_position, SEEK_SET);
    }

    return info->buffer;
}

static void file_stream_close(void *data)
{
    struct file_stream_info *info = (struct file_stream_info *) data;

    fclose(info->handle);
    free(info->buffer);
}

int64_t file_stream_open(stream_t *stream, char const *path)
{
    struct file_stream_info *info = calloc(1, sizeof(struct file_stream_info));

    if (info) {
        info->handle = fopen(path, "rb");

        if (info->handle) {
            info->buffer = NULL;
            info->size = -1;
            strncpy(info->path, path, sizeof(info->path));

            *stream = (stream_t) {
                .data = info,
                .identifier = file_stream_identifier,
                .read = file_stream_read,
                .seek = file_stream_seek,
                .tell = file_stream_tell,
                .get_size = file_stream_get_size,
                .buffer = file_stream_buffer,
                .close = file_stream_close,
            };

            return 0;
        }

        free(info);
    }

    return -1;
}

//------------------------------------------------------------------------------
// Memory streams

struct memory_stream_info
{
    uint8_t const *buffer;
    size_t length;
    size_t position;
    char identifier[32];
};

static char const *memory_stream_identifier(void *data)
{
    struct memory_stream_info *info = (struct memory_stream_info *) data;
    return info->identifier;
}

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

static void const *memory_stream_buffer(void *data)
{
    struct memory_stream_info *info = (struct memory_stream_info *) data;
    return info->buffer;
}

static void memory_stream_close(void *data)
{
    free(data);
}

int64_t memory_stream_open(stream_t *stream, uint8_t const *buffer, size_t length)
{
    struct memory_stream_info *info = calloc(1, sizeof(struct memory_stream_info));

    if (info) {
        info->buffer = buffer;
        info->length = length;
        info->position = 0;
        sprintf("%p", info->identifier, (void *) buffer);

        *stream = (stream_t) {
            .data = info,
            .read = memory_stream_read,
            .seek = memory_stream_seek,
            .tell = memory_stream_tell,
            .get_size = memory_stream_get_size,
            .buffer = memory_stream_buffer,
            .close = memory_stream_close,
        };

        return 0;
    }

    return -1;
}

//------------------------------------------------------------------------------