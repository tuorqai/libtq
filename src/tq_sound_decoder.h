
//------------------------------------------------------------------------------
// tq library :3
//------------------------------------------------------------------------------

#ifndef TQ_SOUND_DECODER_H_INC
#define TQ_SOUND_DECODER_H_INC

//------------------------------------------------------------------------------

#include "tq_stream.h"

//------------------------------------------------------------------------------

enum { TQ_SOUND_DECODER_LIMIT = 32 };

//------------------------------------------------------------------------------

int32_t     tq_sound_decoder_open(int32_t stream_id);
void        tq_sound_decoder_close(int32_t decoder_id);

uint16_t    tq_sound_decoder_get_num_channels(int32_t decoder_id);
uint32_t    tq_sound_decoder_get_num_samples(int32_t decoder_id);
uint32_t    tq_sound_decoder_get_sample_rate(int32_t decoder_id);

void        tq_sound_decoder_seek(int32_t decoder_id, uint64_t sample_offset);
uint64_t    tq_sound_decoder_read(int32_t decoder_id, int16_t *dst, uint64_t max_samples);

//------------------------------------------------------------------------------

#endif // TQ_SOUND_DECODER_H_INC
