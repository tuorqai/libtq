
//------------------------------------------------------------------------------

#ifndef TQ_IMAGE_LOADER_H_INC
#define TQ_IMAGE_LOADER_H_INC

//------------------------------------------------------------------------------

#include "tq_stream.h"

//------------------------------------------------------------------------------

typedef enum pixel_format
{
    PIXEL_FORMAT_RGBA,
    PIXEL_FORMAT_RGB,
    PIXEL_FORMAT_GRAYSCALE_ALPHA,
    PIXEL_FORMAT_GRAYSCALE,
} pixel_format_t;

typedef struct image
{
    uint8_t         *pixels;
    uint32_t        width;
    uint32_t        height;
    pixel_format_t  pixel_format;
} image_t;

int32_t     image_load(image_t *image, stream_t const *stream);
void        image_free(image_t *image);

//------------------------------------------------------------------------------

#endif // TQ_IMAGE_LOADER_H_INC

//------------------------------------------------------------------------------
