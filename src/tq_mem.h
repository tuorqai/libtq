//------------------------------------------------------------------------------
// Copyright (c) 2021-2022 tuorqai
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

#ifndef TQ_MEM_H_INC
#define TQ_MEM_H_INC

//------------------------------------------------------------------------------

#include <stdlib.h>

//------------------------------------------------------------------------------
// This doesn't have much sense at this point.
// But at some point in the future this will help a lot.

#define libtq_malloc(size) \
    malloc(size)

#define libtq_calloc(nmemb, size) \
    calloc(nmemb, size)

#define libtq_realloc(ptr, size) \
    realloc(ptr, size)

#define libtq_free(ptr) \
    free(ptr)

//------------------------------------------------------------------------------
// Deprecated:

#define tq_mem_alloc            libtq_malloc
#define tq_mem_realloc          libtq_realloc
#define tq_mem_free             libtq_free
#define mem_malloc              libtq_malloc
#define mem_calloc              libtq_calloc
#define mem_realloc             libtq_realloc
#define mem_free                libtq_free

//------------------------------------------------------------------------------

#endif // TQ_MEM_H_INC
