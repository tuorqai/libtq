//------------------------------------------------------------------------------
// Copyright (c) 2021-2023 tuorqai
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
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
