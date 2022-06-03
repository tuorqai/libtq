
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

void audio_play_sound(tq_handle_t sound_handle)
{
    mixer->play_sound(sound_handle);
}

void audio_loop_sound(tq_handle_t sound_handle)
{
    mixer->loop_sound(sound_handle);
}

void audio_stop_sound(tq_handle_t sound_handle)
{
    mixer->stop_sound(sound_handle);
}

tq_handle_t audio_open_music_from_file(char const *path)
{
    return mixer->open_music(path);
}

tq_handle_t audio_open_music_from_memory(uint8_t const *buffer, size_t length)
{
    return TQ_INVALID_HANDLE;
}

void audio_close_music(tq_handle_t music_handle)
{
    mixer->close_music(music_handle);
}

void audio_play_music(tq_handle_t music_handle)
{
    mixer->play_music(music_handle);
}

void audio_loop_music(tq_handle_t music_handle)
{
    mixer->loop_music(music_handle);
}

void audio_pause_music(tq_handle_t music_handle)
{
    mixer->pause_music(music_handle);
}

void audio_stop_music(tq_handle_t music_handle)
{
    mixer->stop_music(music_handle);
}

//------------------------------------------------------------------------------
