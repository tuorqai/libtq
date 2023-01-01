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

#include <string.h>

#include "tq/tq.h"
#include "tq_audio.h"
#include "tq_error.h"
#include "tq_log.h"

//------------------------------------------------------------------------------

struct tq_audio_priv
{
    tq_audio_impl impl;
    int channel_source_id[TQ_CHANNEL_LIMIT];
};

static struct tq_audio_priv priv;

//------------------------------------------------------------------------------

static int encode_user_channel_id(int source_id, int real_channel_id)
{
    return ((source_id + 1) << 8) | (real_channel_id + 1);
}

static void decode_user_channel_id(int user_channel_id, int *source_id, int *real_channel_id)
{
    *source_id = ((user_channel_id >> 8) & 262143) - 1;
    *real_channel_id = (user_channel_id & 255) - 1;
}

static int load_sound(libtq_stream *stream)
{
    if (!stream) {
        return -1;
    }

    int sound_id = priv.impl.load_sound(stream);
    libtq_stream_close(stream);

    return sound_id;
}

static int open_music(libtq_stream *stream)
{
    if (!stream) {
        return -1;
    }

    int music_id = priv.impl.open_music(stream);

    if (music_id == -1) {
        libtq_stream_close(stream);
    }

    return music_id;
}

//------------------------------------------------------------------------------

void tq_initialize_audio(void)
{
#if defined(TQ_WIN32) || defined(TQ_LINUX)
    tq_construct_al_audio(&priv.impl);
#else
    tq_construct_null_audio(&priv.impl);
#endif

    priv.impl.initialize();
}

void tq_terminate_audio(void)
{
    priv.impl.terminate();
}

void tq_process_audio(void)
{
    priv.impl.process();
}

void tq_set_master_volume(float volume)
{
    priv.impl.set_master_volume(volume);
}

tq_sound tq_load_sound_from_file(char const *path)
{
    return (tq_sound) {
        .id = load_sound(libtq_open_file_stream(path)),
    };
}

tq_sound tq_load_sound_from_memory(void const *buffer, size_t size)
{
    return (tq_sound) {
        .id = load_sound(libtq_open_memory_stream(buffer, size)),
    };
}

void tq_delete_sound(tq_sound sound)
{
    priv.impl.delete_sound(sound.id);
}

tq_channel tq_play_sound(tq_sound sound, int loop)
{
    int impl_channel_id = priv.impl.play_sound(sound.id, loop);

    if (impl_channel_id == -1) {
        return (tq_channel) { 0 };
    }

    priv.channel_source_id[impl_channel_id] = sound.id;

    return (tq_channel) {
        .id = encode_user_channel_id(sound.id, impl_channel_id),
    };
}

tq_music tq_open_music_from_file(char const *path)
{
    return (tq_music) {
        .id = open_music(libtq_open_file_stream(path)),
    };
}

tq_music tq_open_music_from_memory(void const *buffer, size_t size)
{
    return (tq_music) {
        .id = open_music(libtq_open_memory_stream(buffer, size)),
    };
}

void tq_close_music(tq_music music)
{
    priv.impl.close_music(music.id);
}

tq_channel tq_play_music(tq_music music, int loop)
{
    int impl_channel_id = priv.impl.play_music(music.id, loop);

    if (impl_channel_id == -1) {
        return (tq_channel) { 0 };
    }

    priv.channel_source_id[impl_channel_id] = TQ_SOUND_LIMIT + music.id;

    return (tq_channel) {
        .id = encode_user_channel_id(TQ_SOUND_LIMIT + music.id, impl_channel_id),
    };
}

tq_channel_state tq_get_channel_state(tq_channel channel)
{
    int source_id, impl_channel_id;
    decode_user_channel_id(channel.id, &source_id, &impl_channel_id);

    if (priv.channel_source_id[impl_channel_id] != source_id) {
        return TQ_CHANNEL_INACTIVE;
    }

    return priv.impl.get_channel_state(impl_channel_id);
}

void tq_pause_channel(tq_channel channel)
{
    int source_id, impl_channel_id;
    decode_user_channel_id(channel.id, &source_id, &impl_channel_id);

    if (priv.channel_source_id[impl_channel_id] != source_id) {
        return;
    }

    priv.impl.pause_channel(impl_channel_id);
}

void tq_unpause_channel(tq_channel channel)
{
    int source_id, impl_channel_id;
    decode_user_channel_id(channel.id, &source_id, &impl_channel_id);

    if (priv.channel_source_id[impl_channel_id] != source_id) {
        return;
    }

    priv.impl.unpause_channel(impl_channel_id);
}

void tq_stop_channel(tq_channel channel)
{
    int source_id, impl_channel_id;
    decode_user_channel_id(channel.id, &source_id, &impl_channel_id);

    if (priv.channel_source_id[impl_channel_id] != source_id) {
        return;
    }

    priv.impl.stop_channel(impl_channel_id);
}

//------------------------------------------------------------------------------
