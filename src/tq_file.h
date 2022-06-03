
//------------------------------------------------------------------------------

#ifndef TQ_FILE_H_INC
#define TQ_FILE_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"

//------------------------------------------------------------------------------

typedef struct cx_file cx_file_t;

//------------------------------------------------------------------------------

cx_file_t  *cx_file_open(char const *path);
void        cx_file_close(cx_file_t *file);
int64_t     cx_file_read(cx_file_t *file, void *data, int64_t size);
int64_t     cx_file_seek(cx_file_t *file, int64_t position);
int64_t     cx_file_tell(cx_file_t *file);
int64_t     cx_file_size(cx_file_t *file);

int file_load(char const *path, size_t *out_length, uint8_t **out_buffer);

//------------------------------------------------------------------------------

#endif // TQ_FILE_H_INC
