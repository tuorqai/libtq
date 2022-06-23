
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

void image_free(image_t *image)
{
    stbi_image_free(image->pixels);
}

//------------------------------------------------------------------------------
