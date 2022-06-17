
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

tq_handle_t audio_load_sound_from_file(char const *path)
{
    uint8_t *buffer;
    size_t length;

    if (file_load(path, &length, &buffer) >= 0) {
        tq_handle_t sound_handle = mixer->load_sound(buffer, length);
        free(buffer);

        return sound_handle;
    }

    return TQ_INVALID_HANDLE;
}

tq_handle_t audio_load_sound_from_memory(uint8_t const *buffer, size_t length)
{
    return mixer->load_sound(buffer, length);
}

void audio_delete_sound(tq_handle_t sound_handle)
{
    mixer->delete_sound(sound_handle);
}

tq_handle_t audio_play_sound(tq_handle_t sound_handle, float left_volume, float right_volume, int loop)
{
    return mixer->play_sound(sound_handle, left_volume, right_volume, loop);
}

tq_handle_t audio_open_music_from_file(char const *path)
{
    stream_t stream;

    if (file_stream_open(&stream, path) == -1) {
        return TQ_INVALID_HANDLE;
    }

    tq_handle_t music_handle = mixer->open_music(&stream);

    if (music_handle == TQ_INVALID_HANDLE) {
        stream.close(stream.data);
    }

    return music_handle;
}

tq_handle_t audio_open_music_from_memory(uint8_t const *buffer, size_t length)
{
    stream_t stream;

    if (memory_stream_open(&stream, buffer, length) == -1) {
        return TQ_INVALID_HANDLE;
    }

    tq_handle_t music_handle = mixer->open_music(&stream);

    if (music_handle == TQ_INVALID_HANDLE) {
        stream.close(stream.data);
    }

    return music_handle;
}

void audio_close_music(tq_handle_t music_handle)
{
    mixer->close_music(music_handle);
}

tq_handle_t audio_play_music(tq_handle_t music_handle, int loop)
{
    return mixer->play_music(music_handle, loop);
}

tq_wave_state_t audio_get_wave_state(tq_handle_t wave_handle)
{
    return mixer->get_wave_state(wave_handle);
}

void audio_pause_wave(tq_handle_t wave_handle)
{
    mixer->pause_wave(wave_handle);
}

void audio_unpause_wave(tq_handle_t wave_handle)
{
    mixer->unpause_wave(wave_handle);
}

void audio_stop_wave(tq_handle_t wave_handle)
{
    mixer->stop_wave(wave_handle);
}

//------------------------------------------------------------------------------
