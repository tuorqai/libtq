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

#ifndef TQ_IMAGE_LOADER_H_INC
#define TQ_IMAGE_LOADER_H_INC

//------------------------------------------------------------------------------

#include "tq_stream.h"

//------------------------------------------------------------------------------

enum
{
    LIBTQ_GRAYSCALE = 1,
    LIBTQ_GRAYSCALE_ALPHA = 2,
    LIBTQ_RGB = 3,
    LIBTQ_RGBA = 4,
};

typedef struct libtq_image
{
    int width;
    int height;
    int channels;
    unsigned char pixels[];
} libtq_image;

/**
 * Create an empty image using given parameters.
 */
libtq_image *libtq_create_image(int width, int height, int channels);

/**
 * Load image from an input stream.
 */
libtq_image *libtq_load_image(libtq_stream *stream);

/**
 * Load image from an input stream with color key.
 */
libtq_image *libtq_load_image_with_key(libtq_stream *stream, tq_color key);

//------------------------------------------------------------------------------

#endif // TQ_IMAGE_LOADER_H_INC

//------------------------------------------------------------------------------
