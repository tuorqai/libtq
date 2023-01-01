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

#ifndef TQ_AUDIO_H_INC
#define TQ_AUDIO_H_INC

//------------------------------------------------------------------------------

#include "tq_stream.h"

//------------------------------------------------------------------------------

typedef struct tq_audio_impl
{
    void (*initialize)(void);
    void (*terminate)(void);
    void (*process)(void);

    void (*set_master_volume)(float volume);

    int (*load_sound)(libtq_stream *stream);
    void (*delete_sound)(int sound_id);
    int (*play_sound)(int sound_id, int loop);

    int (*open_music)(libtq_stream *stream);
    void (*close_music)(int music_id);
    int (*play_music)(int music_id, int loop);

    tq_channel_state (*get_channel_state)(int channel_id);
    void (*pause_channel)(int channel_id);
    void (*unpause_channel)(int channel_id);
    void (*stop_channel)(int channel_id);
} tq_audio_impl;

//------------------------------------------------------------------------------

#if defined(TQ_WIN32) || defined(TQ_LINUX)
    void tq_construct_al_audio(tq_audio_impl *impl);
#endif

void tq_construct_null_audio(tq_audio_impl *impl);

//------------------------------------------------------------------------------

void tq_initialize_audio(void);
void tq_terminate_audio(void);
void tq_process_audio(void);

//------------------------------------------------------------------------------

#endif // TQ_AUDIO_H_INC

//------------------------------------------------------------------------------
