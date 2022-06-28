
//------------------------------------------------------------------------------

#include "tq_audio.h"
#include "tq_file.h"

//------------------------------------------------------------------------------

static struct mixer const *mixer;

//------------------------------------------------------------------------------

void audio_initialize(struct mixer const *mixer_)
{
    mixer = mixer_;
    mixer->initialize();
}

void audio_terminate(void)
{
    mixer->terminate();
}

int32_t audio_load_sound_from_file(char const *path)
{
    stream_t stream;

    if (file_stream_open(&stream, path) == -1) {
        return -1;
    }

    int32_t sound_id = mixer->load_sound(&stream);
    stream.close(stream.data);

    return sound_id;
}

int32_t audio_load_sound_from_memory(uint8_t const *buffer, size_t length)
{
    stream_t stream;

    if (memory_stream_open(&stream, buffer, length) == -1) {
        return -1;
    }

    int32_t sound_id = mixer->load_sound(&stream);
    stream.close(stream.data);

    return sound_id;
}

void audio_delete_sound(int32_t sound_id)
{
    mixer->delete_sound(sound_id);
}

int32_t audio_play_sound(int32_t sound_id, int loop)
{
    return mixer->play_sound(sound_id, loop);
}

int32_t audio_open_music_from_file(char const *path)
{
    stream_t stream;

    if (file_stream_open(&stream, path) == -1) {
        return -1;
    }

    int32_t music_id = mixer->open_music(&stream);

    if (music_id == -1) {
        stream.close(stream.data);
    }

    return music_id;
}

int32_t audio_open_music_from_memory(uint8_t const *buffer, size_t length)
{
    stream_t stream;

    if (memory_stream_open(&stream, buffer, length) == -1) {
        return -1;
    }

    int32_t music_id = mixer->open_music(&stream);

    if (music_id == -1) {
        stream.close(stream.data);
    }

    return music_id;
}

void audio_close_music(int32_t music_id)
{
    mixer->close_music(music_id);
}

int32_t audio_play_music(int32_t music_id, int loop)
{
    return mixer->play_music(music_id, loop);
}

tq_channel_state_t audio_get_channel_state(int32_t channel_id)
{
    return mixer->get_channel_state(channel_id);
}

void audio_pause_channel(int32_t channel_id)
{
    mixer->pause_channel(channel_id);
}

void audio_unpause_channel(int32_t channel_id)
{
    mixer->unpause_channel(channel_id);
}

void audio_stop_channel(int32_t channel_id)
{
    mixer->stop_channel(channel_id);
}

//------------------------------------------------------------------------------
