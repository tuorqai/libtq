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