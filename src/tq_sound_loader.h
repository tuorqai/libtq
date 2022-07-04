
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

/**
 * Load sound from an input stream.
 */
tq_sound_t *tq_sound_load(int32_t stream_id);

/**
 * Destroy previously loaded sound.
 */
void tq_sound_destroy(tq_sound_t *sound);

//------------------------------------------------------------------------------

#endif // TQ_SOUND_LOADER_H

//------------------------------------------------------------------------------
