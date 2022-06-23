//------------------------------------------------------------------------------

#ifndef TQ_STREAM_H_INC
#define TQ_STREAM_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"

//------------------------------------------------------------------------------

typedef struct stream
{
    void *data;

    char const *(*identifier)(void *);

    int64_t (*read)(void *, void *dst, int64_t size);
    int64_t (*seek)(void *, int64_t position);
    int64_t (*tell)(void *);
    int64_t (*get_size)(void *);
    void const *(*buffer)(void *);

    void (*close)(void *);
} stream_t;

//------------------------------------------------------------------------------

int64_t file_stream_open(stream_t *stream, char const *path);
int64_t memory_stream_open(stream_t *stream, uint8_t const *buffer, size_t length);

//------------------------------------------------------------------------------

#endif // TQ_STREAM_H_INC

//------------------------------------------------------------------------------