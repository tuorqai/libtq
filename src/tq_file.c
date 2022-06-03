
//------------------------------------------------------------------------------

#include <stdio.h>
#include "tq_file.h"

//------------------------------------------------------------------------------

struct cx_file
{
    FILE *stream;
};

//------------------------------------------------------------------------------

cx_file_t *cx_file_open(char const *path)
{
    FILE *stream = fopen(path, "rb");

    if (stream) {
        cx_file_t *file = malloc(sizeof(cx_file_t));

        if (file) {
            file->stream = stream;
            return file;
        }

        fclose(stream);
    }

    return NULL;
}

void cx_file_close(cx_file_t *file)
{
    fclose(file->stream);
    free(file);
}

int64_t cx_file_read(cx_file_t *file, void *data, int64_t size)
{
    return fread(data, 1, size, file->stream);
}

int64_t cx_file_seek(cx_file_t *file, int64_t position)
{
    if (fseek(file->stream, position, SEEK_SET) == 0) {
        return (int64_t) ftell(file->stream);
    }

    return -1;
}

int64_t cx_file_tell(cx_file_t *file)
{
    return (int64_t) ftell(file->stream);
}

int64_t cx_file_size(cx_file_t *file)
{
    int64_t position = (int64_t) ftell(file->stream);

    fseek(file->stream, 0, SEEK_END);
    int64_t size = (int64_t) ftell(file->stream);

    fseek(file->stream, position, SEEK_SET);
    return size;
}

int file_load(char const *path, size_t *out_length, uint8_t **out_buffer)
{
    FILE *file = fopen(path, "rb");

    if (file != NULL) {
        fseek(file, 0, SEEK_END);

        size_t length = ftell(file);
        uint8_t *buffer = malloc(length);

        if (buffer != NULL) {
            fseek(file, 0, SEEK_SET);

            if (fread(buffer, length, 1, file) == 1) {
                *out_length = length;
                *out_buffer = buffer;

                return 0;
            }

            free(buffer);
        }

        fclose(file);
    }

    return -1;
}

//------------------------------------------------------------------------------
