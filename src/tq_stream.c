
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "tq_log.h"
#include "tq_stream.h"

//------------------------------------------------------------------------------

#define TQ_STREAM_NAME_LENGTH       (256)

typedef struct tq_file_istream_data
{
    FILE            *handle;
    uint8_t         *buffer;
    size_t          size;
    char            path[TQ_STREAM_NAME_LENGTH];
} tq_file_istream_data_t;

typedef struct tq_memory_istream_data
{
    uint8_t const   *buffer;
    int64_t         size;
    int64_t         position;
    char            repr[TQ_STREAM_NAME_LENGTH];
} tq_memory_istream_data_t;

typedef union tq_istream_data
{
    tq_file_istream_data_t      file;
    tq_memory_istream_data_t    memory;
} tq_istream_data_t;

typedef struct tq_istream
{
    int64_t     (*read)(tq_istream_data_t *data, void *dst, int64_t size);
    int64_t     (*seek)(tq_istream_data_t *data, int64_t position);
    int64_t     (*tell)(tq_istream_data_t *data);
    int64_t     (*size)(tq_istream_data_t *data);
    void const  *(*buffer)(tq_istream_data_t *data);
    int64_t     (*close)(tq_istream_data_t *data);
    char const  *(*repr)(tq_istream_data_t *data);

    tq_istream_data_t data;
} tq_istream_t;

//------------------------------------------------------------------------------

static tq_istream_t    *istream_array[TQ_INPUT_STREAM_LIMIT];

//------------------------------------------------------------------------------

int32_t get_istream_id(void)
{
    for (int32_t id = 0; id < TQ_INPUT_STREAM_LIMIT; id++) {
        if (istream_array[id] == NULL) {
            return id;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
// File streams

static int64_t file_istream_read(tq_istream_data_t *data, void *dst, int64_t size)
{
    return (int64_t) fread(dst, 1, size, data->file.handle);
}

static int64_t file_istream_seek(tq_istream_data_t *data, int64_t position)
{
    return (int64_t) fseek(data->file.handle, position, SEEK_SET);
}

static int64_t file_istream_tell(tq_istream_data_t *data)
{
    return (int64_t) ftell(data->file.handle);
}

static int64_t file_istream_size(tq_istream_data_t *data)
{
    if (data->file.size == (size_t) -1) {
        int64_t prev_position = (int64_t) ftell(data->file.handle);

        fseek(data->file.handle, 0, SEEK_END);
        data->file.size = (int64_t) ftell(data->file.handle);

        fseek(data->file.handle, prev_position, SEEK_SET);
    }

    return data->file.size;
}

static void const *file_istream_buffer(tq_istream_data_t *data)
{
    if (data->file.buffer == NULL) {
        size_t size = file_istream_size(data);

        uint8_t *buffer = malloc(size);

        if (!buffer) {
            return NULL;
        }

        int64_t prev_position = (int64_t) ftell(data->file.handle);

        fseek(data->file.handle, 0, SEEK_SET);
        size_t read = fread(buffer, size, 1, data->file.handle);

        if (read == 1) {
            data->file.buffer = buffer;
        } else {
            free(buffer);
        }

        fseek(data->file.handle, prev_position, SEEK_SET);
    }

    return data->file.buffer;
}

static int64_t file_istream_close(tq_istream_data_t *data)
{
    fclose(data->file.handle);
    free(data->file.buffer);

    return 0;
}

static char const *file_istream_repr(tq_istream_data_t *data)
{
    return data->file.path;
}

//------------------------------------------------------------------------------
// Memory streams

static int64_t memory_istream_read(tq_istream_data_t *data, void *dst, int64_t size)
{
    if (data->memory.position > data->memory.size) {
        return -1;
    }

    int64_t bytes_left = data->memory.size - data->memory.position;
    int64_t bytes_to_copy = (bytes_left < size) ? bytes_left : size;

    memcpy(dst, data->memory.buffer + data->memory.position, bytes_to_copy);
    data->memory.position += bytes_to_copy;
    return bytes_to_copy;
}

static int64_t memory_istream_seek(tq_istream_data_t *data, int64_t position)
{
    if (position >= 0 && position <= data->memory.size) {
        data->memory.position = position;
        return 0;
    }

    return -1;
}

static int64_t memory_istream_tell(tq_istream_data_t *data)
{
    return data->memory.position;
}

static int64_t memory_istream_size(tq_istream_data_t *data)
{
    return data->memory.size;
}

static void const *memory_istream_buffer(tq_istream_data_t *data)
{
    return data->memory.buffer;
}

static int64_t memory_istream_close(tq_istream_data_t *data)
{
    return 0;
}

static char const *memory_istream_repr(tq_istream_data_t *data)
{
    return data->memory.repr;
}

//------------------------------------------------------------------------------

int64_t tq_istream_read(int32_t istream_id, void *dst, size_t size)
{
    tq_istream_t *istream = istream_array[istream_id];

    if (istream == NULL) {
        tq_log_warning("tq_istream_read(): istream is NULL.\n");
        return -1;
    }

    return istream->read(&istream->data, dst, size);
}

int64_t tq_istream_seek(int32_t istream_id, int64_t position)
{
    tq_istream_t *istream = istream_array[istream_id];

    if (istream == NULL) {
        tq_log_warning("tq_istream_seek(): istream is NULL.\n");
        return -1;
    }

    return istream->seek(&istream->data, position);
}

int64_t tq_istream_tell(int32_t istream_id)
{
    tq_istream_t *istream = istream_array[istream_id];

    if (istream == NULL) {
        tq_log_warning("tq_istream_tell(): istream is NULL.\n");
        return -1;
    }

    return istream->tell(&istream->data);
}

int64_t tq_istream_size(int32_t istream_id)
{
    tq_istream_t *istream = istream_array[istream_id];

    if (istream == NULL) {
        tq_log_warning("tq_istream_size(): istream is NULL.\n");
        return -1;
    }

    return istream->size(&istream->data);
}

void const *tq_istream_buffer(int32_t istream_id)
{
    tq_istream_t *istream = istream_array[istream_id];

    if (istream == NULL) {
        tq_log_warning("tq_istream_buffer(): istream is NULL.\n");
        return NULL;
    }

    return istream->buffer(&istream->data);
}

int64_t tq_istream_close(int32_t istream_id)
{
    tq_istream_t *istream = istream_array[istream_id];

    if (istream == NULL) {
        tq_log_warning("tq_istream_close(): istream is NULL.\n");
        return -1;
    }

    tq_log_debug("Closing i-stream: %s...\n", istream->repr(&istream->data));

    int64_t status = istream->close(&istream->data);

    free(istream);
    istream_array[istream_id] = NULL;

    return status;
}

char const *tq_istream_repr(int32_t istream_id)
{
    tq_istream_t *istream = istream_array[istream_id];

    if (istream == NULL) {
        return NULL;
    }

    return istream->repr(&istream->data);
}

//------------------------------------------------------------------------------

int32_t tq_open_file_istream(char const *path)
{
    int32_t id = get_istream_id();

    if (id == -1) {
        return -1;
    }

    FILE *handle = fopen(path, "rb");

    if (handle == NULL) {
        return -1;
    }

    tq_istream_t *istream = malloc(sizeof(tq_istream_t));

    if (istream == NULL) {
        return -1;
    }

    istream->read = file_istream_read;
    istream->seek = file_istream_seek;
    istream->tell = file_istream_tell;
    istream->size = file_istream_size;
    istream->buffer = file_istream_buffer;
    istream->close = file_istream_close;
    istream->repr = file_istream_repr;

    istream->data.file.handle = handle;
    istream->data.file.buffer = NULL;
    istream->data.file.size = (size_t) -1;
    strncpy(istream->data.file.path, path, TQ_STREAM_NAME_LENGTH);

    tq_log_debug("Opened file i-stream: %s\n", istream->repr(&istream->data));

    istream_array[id] = istream;
    return id;
}

int32_t tq_open_memory_istream(uint8_t const *buffer, size_t size)
{
    int32_t id = get_istream_id();

    if (id == -1) {
        return -1;
    }

    tq_istream_t *istream = malloc(sizeof(tq_istream_t));

    if (istream == NULL) {
        return -1;
    }

    istream->read = memory_istream_read;
    istream->seek = memory_istream_seek;
    istream->tell = memory_istream_tell;
    istream->size = memory_istream_size;
    istream->buffer = memory_istream_buffer;
    istream->close = memory_istream_close;
    istream->repr = memory_istream_repr;

    istream->data.memory.buffer = buffer;
    istream->data.memory.size = size;
    istream->data.memory.position = 0;
    snprintf("%p", TQ_STREAM_NAME_LENGTH, istream->data.memory.repr, (void *) buffer);

    tq_log_debug("Opened memory i-stream: %s\n", istream->repr(&istream->data));

    istream_array[id] = istream;
    return id;
}

//------------------------------------------------------------------------------