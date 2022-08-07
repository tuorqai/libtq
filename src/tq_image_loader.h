
//------------------------------------------------------------------------------

#ifndef TQ_IMAGE_LOADER_H_INC
#define TQ_IMAGE_LOADER_H_INC

//------------------------------------------------------------------------------

#include "tq_stream.h"

//------------------------------------------------------------------------------

enum
{
    PIXEL_FORMAT_NONE,
    PIXEL_FORMAT_GRAYSCALE,
    PIXEL_FORMAT_GRAYSCALE_ALPHA,
    PIXEL_FORMAT_RGB,
    PIXEL_FORMAT_RGBA,
};

struct image
{
    unsigned char *pixels;
    int width;
    int height;
    int channels;
};

/**
 * Create an empty image using given parameters.
 */
struct image image_create(unsigned int width, unsigned int height, unsigned int channels);

/**
 * Load image from an input stream.
 */
struct image image_load(libtq_stream *stream);

//------------------------------------------------------------------------------

#endif // TQ_IMAGE_LOADER_H_INC

//------------------------------------------------------------------------------
