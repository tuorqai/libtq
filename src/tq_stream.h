
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
 * Maximum number of input streams (opened simultaneosly).
 */
#define TQ_INPUT_STREAM_LIMIT       (128)

//------------------------------------------------------------------------------

/**
 * Read block of data from an input stream.
 */
int64_t tq_istream_read(int32_t istream_id, void *dst, size_t size);

/**
 * Change the current reading position of an input stream.
 */
int64_t tq_istream_seek(int32_t istream_id, int64_t position);

/**
 * Get the current reading position of an input stream.
 */
int64_t tq_istream_tell(int32_t istream_id);

/**
 * Get the size of an input stream.
 */
int64_t tq_istream_size(int32_t istream_id);

/**
 * Get the whole data hold by the input stream.
 * For memory streams, this will just return the pointer to the buffer.
 * For file-based streams, this will load the contents of a file and keep it
 * in memory for further access.
 */
void const *tq_istream_buffer(int32_t istream_id);

/**
 * Close the input stream.
 */
int64_t tq_istream_close(int32_t istream_id);

/**
 * Get printable string that represents the input stream.
 */
char const *tq_istream_repr(int32_t istream_id);

//------------------------------------------------------------------------------

/**
 * Open input stream from file path.
 */
int32_t tq_open_file_istream(char const *path);

/**
 * Open input stream from memory buffer.
 * The buffer shouldn't be freed before the stream
 * is closed.
 */
int32_t tq_open_memory_istream(void const *buffer, size_t size);

#define input_stream_read               tq_istream_read
#define input_stream_seek               tq_istream_seek
#define input_stream_tell               tq_istream_tell
#define input_stream_size               tq_istream_size
#define input_stream_buffer             tq_istream_buffer
#define input_stream_close              tq_istream_close
#define input_stream_repr               tq_istream_repr

#define open_file_input_stream          tq_open_file_istream
#define open_memory_input_stream        tq_open_memory_istream

//------------------------------------------------------------------------------

#endif // TQ_STREAM_H_INC

//------------------------------------------------------------------------------