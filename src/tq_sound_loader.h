
//------------------------------------------------------------------------------

#ifndef TQ_SOUND_LOADER_H
#define TQ_SOUND_LOADER_H

//------------------------------------------------------------------------------

#include "tq_stream.h"

//------------------------------------------------------------------------------

typedef struct tq_sound
{
    uint16_t    num_channels;
    uint16_t    bytes_per_sample;
    uint32_t    sample_rate;
    uint32_t    num_samples;
    uint8_t     *samples;
} tq_sound_t;

//------------------------------------------------------------------------------

int32_t tq_sound_load(tq_sound_t *sound, stream_t const *stream);
void tq_sound_free(tq_sound_t *sound);

//------------------------------------------------------------------------------

#endif // TQ_SOUND_LOADER_H

//------------------------------------------------------------------------------
