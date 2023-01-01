//------------------------------------------------------------------------------
// Copyright (c) 2021-2023 tuorqai
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

#include <stdio.h>
#include <string.h>

#include "tq_error.h"
#include "tq_mem.h"
#include "tq_log.h"
#include "tq_stream.h"

//------------------------------------------------------------------------------

#define STREAM_NAME_LENGTH          (256)

//------------------------------------------------------------------------------

struct libtq_file_stream
{
    FILE            *handle;
    void            *buffer;
    size_t          size;
    char            path[STREAM_NAME_LENGTH];
};

struct libtq_memory_stream
{
    void const      *buffer;
    intptr_t        size;
    intptr_t        position;
    char            repr[STREAM_NAME_LENGTH];
};

struct libtq_stream
{
    intptr_t        (*read)(libtq_stream *stream, void *dst, intptr_t size);
    intptr_t        (*seek)(libtq_stream *stream, intptr_t position);
    intptr_t        (*tell)(libtq_stream *stream);
    intptr_t        (*size)(libtq_stream *stream);
    intptr_t        (*close)(libtq_stream *stream);

    void const      *(*buffer)(libtq_stream *stream);
    char const      *(*repr)(libtq_stream *stream);

    union {
        struct libtq_file_stream    file;
        struct libtq_memory_stream  memory;
    } info;
};

//------------------------------------------------------------------------------
// File streams

static intptr_t file_stream_read(libtq_stream *stream, void *dst, intptr_t size)
{
    return (intptr_t) fread(dst, 1, size, stream->info.file.handle);
}

static intptr_t file_stream_seek(libtq_stream *stream, intptr_t position)
{
    return (intptr_t) fseek(stream->info.file.handle, position, SEEK_SET);
}

static intptr_t file_stream_tell(libtq_stream *stream)
{
    return (intptr_t) ftell(stream->info.file.handle);
}

static intptr_t file_stream_size(libtq_stream *stream)
{
    if (stream->info.file.size == (size_t) -1) {
        intptr_t prev_position = (intptr_t) ftell(stream->info.file.handle);

        fseek(stream->info.file.handle, 0, SEEK_END);
        stream->info.file.size = (intptr_t) ftell(stream->info.file.handle);

        fseek(stream->info.file.handle, prev_position, SEEK_SET);
    }

    return stream->info.file.size;
}

static void const *file_stream_buffer(libtq_stream *stream)
{
    if (stream->info.file.buffer == NULL) {
        size_t size = stream->size(stream);

        void *buffer = libtq_malloc(size);

        if (!buffer) {
            libtq_out_of_memory();
        }

        intptr_t prev_position = (intptr_t) ftell(stream->info.file.handle);

        fseek(stream->info.file.handle, 0, SEEK_SET);
        size_t read = fread(buffer, size, 1, stream->info.file.handle);

        if (read == 1) {
            stream->info.file.buffer = buffer;
        } else {
            libtq_free(buffer);
        }

        fseek(stream->info.file.handle, prev_position, SEEK_SET);
    }

    return stream->info.file.buffer;
}

static char const *file_stream_repr(libtq_stream *stream)
{
    return stream->info.file.path;
}

static intptr_t file_stream_close(libtq_stream *stream)
{
    fclose(stream->info.file.handle);
    libtq_free(stream->info.file.buffer);

    return 0;
}

//------------------------------------------------------------------------------
// Memory streams

static intptr_t memory_stream_read(libtq_stream *stream, void *dst, intptr_t size)
{
    struct libtq_memory_stream *memory = &stream->info.memory;

    if (memory->position > memory->size) {
        return -1;
    }

    intptr_t bytes_left = memory->size - memory->position;
    intptr_t bytes_to_copy = (bytes_left < size) ? bytes_left : size;

    void const *ptr = ((uint8_t const *) memory->buffer) + memory->position;
    memcpy(dst, ptr, bytes_to_copy);
    memory->position += bytes_to_copy;
    return bytes_to_copy;
}

static intptr_t memory_stream_seek(libtq_stream *stream, intptr_t position)
{
    struct libtq_memory_stream *memory = &stream->info.memory;

    if (position >= 0 && position <= memory->size) {
        memory->position = position;
        return 0;
    }

    return -1;
}

static intptr_t memory_stream_tell(libtq_stream *stream)
{
    return stream->info.memory.position;
}

static intptr_t memory_stream_size(libtq_stream *stream)
{
    return stream->info.memory.size;
}

static void const *memory_stream_buffer(libtq_stream *stream)
{
    return stream->info.memory.buffer;
}

static intptr_t memory_stream_close(libtq_stream *stream)
{
    return 0;
}

static char const *memory_stream_repr(libtq_stream *stream)
{
    return stream->info.memory.repr;
}

//------------------------------------------------------------------------------

intptr_t libtq_stream_read(libtq_stream *stream, void *dst, size_t size)
{
    if (!stream) {
        libtq_log(LIBTQ_LOG_WARNING, "libtq_stream_read(): stream is NULL.\n");
        return -1;
    }

    return stream->read(stream, dst, size);
}

intptr_t libtq_stream_seek(libtq_stream *stream, intptr_t position)
{
    if (!stream) {
        libtq_log(LIBTQ_LOG_WARNING, "libtq_stream_seek(): stream is NULL.\n");
        return -1;
    }

    return stream->seek(stream, position);
}

intptr_t libtq_stream_tell(libtq_stream *stream)
{
    if (!stream) {
        libtq_log(LIBTQ_LOG_WARNING, "libtq_stream_tell(): stream is NULL.\n");
        return -1;
    }

    return stream->tell(stream);
}

intptr_t libtq_stream_size(libtq_stream *stream)
{
    if (!stream) {
        libtq_log(LIBTQ_LOG_WARNING, "libtq_stream_size(): stream is NULL.\n");
        return -1;
    }

    return stream->size(stream);
}

intptr_t libtq_stream_close(libtq_stream *stream)
{
    if (!stream) {
        return -1;
    }

    libtq_log(0, "Closing stream \"%s\"...\n", stream->repr(stream));
    intptr_t status = stream->close(stream);
    libtq_free(stream);
    return status;
}

void const *libtq_stream_buffer(libtq_stream *stream)
{
    if (!stream) {
        libtq_log(LIBTQ_LOG_WARNING, "libtq_stream_buffer(): stream is NULL.\n");
        return NULL;
    }

    return stream->buffer(stream);
}

char const *libtq_stream_repr(libtq_stream *stream)
{
    if (!stream) {
        libtq_log(LIBTQ_LOG_WARNING, "libtq_stream_repr(): stream is NULL.\n");
        return NULL;
    }

    return stream->repr(stream);
}

//------------------------------------------------------------------------------

libtq_stream *libtq_open_file_stream(char const *path)
{
    FILE *handle = fopen(path, "rb");

    if (handle == NULL) {
        libtq_log(LIBTQ_LOG_WARNING, "libtq_open_file_stream(): file %s not found.\n", path);
        return NULL;
    }

    libtq_stream *stream = libtq_calloc(1, sizeof(struct libtq_stream));

    if (!stream) {
        libtq_out_of_memory();
    }

    *stream = (libtq_stream) {
        .read = file_stream_read,
        .seek = file_stream_seek,
        .tell = file_stream_tell,
        .size = file_stream_size,
        .buffer = file_stream_buffer,
        .close = file_stream_close,
        .repr = file_stream_repr,

        .info.file = {
            .handle = handle,
            .buffer = NULL,
            .size = (size_t) -1,
        },
    };

    strncpy(stream->info.file.path, path, STREAM_NAME_LENGTH);

    libtq_log(0, "Opened file stream: %s\n", stream->repr(stream));
    return stream;
}

libtq_stream *libtq_open_memory_stream(void const *buffer, size_t size)
{
    libtq_stream *stream = libtq_calloc(1, sizeof(struct libtq_stream));

    if (!stream) {
        libtq_out_of_memory();
    }

    *stream = (libtq_stream) {
        .read = memory_stream_read,
        .seek = memory_stream_seek,
        .tell = memory_stream_tell,
        .size = memory_stream_size,
        .buffer = memory_stream_buffer,
        .close = memory_stream_close,
        .repr = memory_stream_repr,

        .info.memory = {
            .buffer = buffer,
            .size = size,
            .position = 0,
        },
    };

    snprintf("%p", STREAM_NAME_LENGTH, stream->info.memory.repr, buffer);

    libtq_log(0, "Opened memory stream: %s\n", stream->repr(stream));
    return stream;
}

//------------------------------------------------------------------------------