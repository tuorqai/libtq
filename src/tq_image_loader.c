
//------------------------------------------------------------------------------

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "tq_image_loader.h"
#include "tq_log.h"

//------------------------------------------------------------------------------

int32_t image_load(image_t *image, stream_t const *stream)
{
    uint8_t const *buffer = stream->buffer(stream->data);
    size_t length = stream->get_size(stream->data);

    if (buffer == NULL || length == 0) {
        log_error("Can't load image from empty stream: %s\n", stream->identifier(stream->data));
        return -1;
    }

    int x, y, n;
    unsigned char *pixels = stbi_load_from_memory(buffer, length, &x, &y, &n, 0);

    if (pixels == NULL) {
        log_error("stbi image loader error: %s\n", stbi_failure_reason());
        return -1;
    }

    image->pixels = pixels;
    image->width = x;
    image->height = y;

    switch (n) {
    case 1:
        image->pixel_format = PIXEL_FORMAT_GRAYSCALE;
        break;
    case 2:
        image->pixel_format = PIXEL_FORMAT_GRAYSCALE_ALPHA;
        break;
    case 3:
        image->pixel_format = PIXEL_FORMAT_RGB;
        break;
    case 4:
        image->pixel_format = PIXEL_FORMAT_RGBA;
        break;
    }

    return 0;
}

int32_t image_cut(image_t *image, image_t *src, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    unsigned int n = 0;

    switch (src->pixel_format) {
    case PIXEL_FORMAT_RGBA:
        n = 4;
        break;
    case PIXEL_FORMAT_RGB:
        n = 3;
        break;
    case PIXEL_FORMAT_GRAYSCALE_ALPHA:
        n = 2;
        break;
    case PIXEL_FORMAT_GRAYSCALE:
        n = 1;
        break;
    }

    if (n == 0) {
        log_error("image_cut(): Unknown pixel format.\n");
        return -1;
    }

    image->pixels = malloc(w * h * n);

    for (int r = 0; r < h; r++) {
        memcpy(&image->pixels[r * w], &src->pixels[r * w + x], w * n);
    }

    image->width = w;
    image->height = h;
    image->pixel_format = src->pixel_format;

    return 0;
}

void image_free(image_t *image)
{
    stbi_image_free(image->pixels);
}

//------------------------------------------------------------------------------
