
//------------------------------------------------------------------------------

#ifndef TQ_IMAGE_LOADER_H_INC
#define TQ_IMAGE_LOADER_H_INC

//------------------------------------------------------------------------------

#include "tq_stream.h"

//------------------------------------------------------------------------------

typedef enum tq_pixel_format
{
    TQ_PIXEL_FORMAT_INVALID,
    TQ_PIXEL_FORMAT_GRAYSCALE,
    TQ_PIXEL_FORMAT_GRAYSCALE_ALPHA,
    TQ_PIXEL_FORMAT_RGB,
    TQ_PIXEL_FORMAT_RGBA,
} tq_pixel_format_t;

typedef struct tq_image
{
    int32_t stream_id;
    uint8_t *pixels;
    uint32_t width;
    uint32_t height;
    uint32_t pixel_format;
} tq_image_t;

/**
 * Load image from an input stream.
 */
tq_image_t *tq_image_load(int32_t stream_id);

/**
 * Create a new image from the rectangle part of another one.
 */
tq_image_t *tq_image_cut(tq_image_t *src, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

/**
 * Destroy loaded image and its data.
 */
void tq_image_destroy(tq_image_t *image);

//------------------------------------------------------------------------------

#endif // TQ_IMAGE_LOADER_H_INC

//------------------------------------------------------------------------------
