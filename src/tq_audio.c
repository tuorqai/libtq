
//------------------------------------------------------------------------------
// tq library :3
//------------------------------------------------------------------------------

#include <string.h>

#include "tq_audio.h"
#include "tq_error.h"
#include "tq_log.h"

//------------------------------------------------------------------------------
// Declarations

typedef struct tq_audio
{
    tq_audio_impl_t     impl;
} tq_audio_t;

//------------------------------------------------------------------------------
// Definitions

static tq_audio_t audio;

//------------------------------------------------------------------------------

void tq_audio_initialize(void)
{
    memset(&audio, 0, sizeof(tq_audio_t));

#if defined(TQ_USE_OPENAL)
    tq_construct_al_audio(&audio.impl);
#else
    #error Invalid compilation flags: no audio implementation is enabled.
#endif

    audio.impl.initialize();
}

void tq_audio_terminate(void)
{
    audio.impl.terminate();
}

int32_t tq_audio_load_sound_from_file(char const *path)
{
    int32_t stream_id = tq_open_file_istream(path);

    if (stream_id == -1) {
        return -1;
    }

    int32_t sound_id = audio.impl.load_sound(stream_id);

    tq_istream_close(stream_id);

    return sound_id;
}

int32_t tq_audio_load_sound_from_memory(uint8_t const *buffer, size_t size)
{
    int32_t stream_id = tq_open_memory_istream(buffer, size);

    if (stream_id == -1) {
        return -1;
    }

    int32_t sound_id = audio.impl.load_sound(stream_id);

    tq_istream_close(stream_id);

    return sound_id;
}

void tq_audio_delete_sound(int32_t sound_id)
{
    audio.impl.delete_sound(sound_id);
}

int32_t tq_audio_play_sound(int32_t sound_id, int loop)
{
    return audio.impl.play_sound(sound_id, loop);
}

int32_t tq_audio_open_music_from_file(char const *path)
{
    int32_t stream_id = tq_open_file_istream(path);

    if (stream_id == -1) {
        return -1;
    }

    int32_t music_id = audio.impl.open_music(stream_id);

    if (music_id == -1) {
        tq_istream_close(stream_id);
    }

    return music_id;
}

int32_t tq_audio_open_music_from_memory(uint8_t const *buffer, size_t size)
{
    int32_t stream_id = tq_open_memory_istream(buffer, size);

    if (stream_id == -1) {
        return -1;
    }

    int32_t music_id = audio.impl.open_music(stream_id);

    if (music_id == -1) {
        tq_istream_close(stream_id);
    }

    return music_id;
}

void tq_audio_close_music(int32_t music_id)
{
    audio.impl.close_music(music_id);
}

int32_t tq_audio_play_music(int32_t music_id, int loop)
{
    return audio.impl.play_music(music_id, loop);
}

tq_channel_state_t tq_audio_get_channel_state(int32_t channel_id)
{
    return audio.impl.get_channel_state(channel_id);
}

void tq_audio_pause_channel(int32_t channel_id)
{
    audio.impl.pause_channel(channel_id);
}

void tq_audio_unpause_channel(int32_t channel_id)
{
    audio.impl.unpause_channel(channel_id);
}

void tq_audio_stop_channel(int32_t channel_id)
{
    audio.impl.stop_channel(channel_id);
}

//------------------------------------------------------------------------------
