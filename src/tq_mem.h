
//------------------------------------------------------------------------------
// tq library :3
//------------------------------------------------------------------------------

#ifndef TQ_MEM_H_INC
#define TQ_MEM_H_INC

//------------------------------------------------------------------------------

#include <stdlib.h>

//------------------------------------------------------------------------------
// This doesn't have much sense at this point.
// But at some point in the future this will help a lot.

#define tq_mem_alloc    malloc
#define tq_mem_realloc  realloc
#define tq_mem_free     free

#define mem_malloc(size) \
    malloc(size)

#define mem_calloc(nmemb, size) \
    calloc(nmemb, size)

#define mem_realloc(ptr, size) \
    realloc(ptr, size)

#define mem_free(ptr) \
    free(ptr)

//------------------------------------------------------------------------------

#endif // TQ_MEM_H_INC
