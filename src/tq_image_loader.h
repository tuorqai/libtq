
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

//------------------------------------------------------------------------------

#endif // TQ_IMAGE_LOADER_H_INC

//------------------------------------------------------------------------------
