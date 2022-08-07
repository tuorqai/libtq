
//------------------------------------------------------------------------------
// tq library :3
//
// Input streams
//------------------------------------------------------------------------------

#ifndef TQ_STREAM_H_INC
#define TQ_STREAM_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"

//------------------------------------------------------------------------------

/**
 * Opaque handle to input stream.
 */
typedef struct libtq_stream libtq_stream;

//------------------------------------------------------------------------------

/**
 * Open input stream from file path.
 */
libtq_stream *libtq_open_file_stream(char const *path);

/**
 * Open input stream from memory buffer.
 * The buffer shouldn't be freed before the stream
 * is closed.
 */
libtq_stream *libtq_open_memory_stream(void const *buffer, size_t size);

/**
 * Read block of data from an input stream.
 */
intptr_t libtq_stream_read(libtq_stream *stream, void *dst, size_t size);

/**
 * Change the current reading position of an input stream.
 */
intptr_t libtq_stream_seek(libtq_stream *stream, intptr_t position);

/**
 * Get the current reading position of an input stream.
 */
intptr_t libtq_stream_tell(libtq_stream *stream);

/**
 * Get the size of an input stream.
 */
intptr_t libtq_stream_size(libtq_stream *stream);

/**
 * Get the whole data hold by the input stream.
 * For memory streams, this will just return the pointer to the buffer.
 * For file-based streams, this will load the contents of a file and keep it
 * in memory for further access.
 */
void const *libtq_stream_buffer(libtq_stream *stream);

/**
 * Get printable string that represents the input stream.
 */
char const *libtq_stream_repr(libtq_stream *stream);

/**
 * Close the input stream.
 */
intptr_t libtq_stream_close(libtq_stream *stream);

//------------------------------------------------------------------------------

#endif // TQ_STREAM_H_INC

//------------------------------------------------------------------------------