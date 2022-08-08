
//------------------------------------------------------------------------------
// tq library :3
//------------------------------------------------------------------------------

#ifndef TQ_AUDIO_DEC_H_INC
#define TQ_AUDIO_DEC_H_INC

//------------------------------------------------------------------------------

#include "tq_stream.h"

//------------------------------------------------------------------------------

typedef struct libtq_audio_dec libtq_audio_dec;

//------------------------------------------------------------------------------

libtq_audio_dec *libtq_open_audio_dec(libtq_stream *stream);

int         libtq_audio_dec_get_num_channels(libtq_audio_dec *dec);
long        libtq_audio_dec_get_num_samples(libtq_audio_dec *dec);
long        libtq_audio_dec_get_sample_rate(libtq_audio_dec *dec);

long        libtq_audio_dec_read(libtq_audio_dec *dec, short *dst, long max_samples);
void        libtq_audio_dec_seek(libtq_audio_dec *dec, long sample_offset);
void        libtq_audio_dec_close(libtq_audio_dec *dec);

//------------------------------------------------------------------------------

#endif // TQ_AUDIO_DEC_H_INC

//------------------------------------------------------------------------------
