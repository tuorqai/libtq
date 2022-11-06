
//------------------------------------------------------------------------------
// tq library :3
//------------------------------------------------------------------------------

#include <string.h>

#include "tq_audio.h"
#include "tq_error.h"
#include "tq_log.h"

//------------------------------------------------------------------------------
// Declarations

struct libtq_audio_priv
{
    struct libtq_audio_impl     impl;
};

//------------------------------------------------------------------------------
// Definitions

static struct libtq_audio_priv priv;

//------------------------------------------------------------------------------

void libtq_initialize_audio(void)
{
#if defined(TQ_WIN32) || defined(TQ_LINUX)
    libtq_construct_al_audio(&priv.impl);
#else
    libtq_construct_null_audio(&priv.impl);
#endif

    priv.impl.initialize();
}

void libtq_terminate_audio(void)
{
    priv.impl.terminate();
}

void libtq_process_audio(void)
{
    priv.impl.process();
}

void libtq_set_master_volume(float volume)
{
    priv.impl.set_master_volume(volume);
}

int libtq_load_sound(libtq_stream *stream)
{
    if (!stream) {
        return -1;
    }

    int sound_id = priv.impl.load_sound(stream);
    libtq_stream_close(stream);
    return sound_id;
}

void libtq_delete_sound(int sound_id)
{
    priv.impl.delete_sound(sound_id);
}

int libtq_play_sound(int sound_id, int loop)
{
    return priv.impl.play_sound(sound_id, loop);
}

int libtq_open_music(libtq_stream *stream)
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

void libtq_close_music(int music_id)
{
    priv.impl.close_music(music_id);
}

int libtq_play_music(int music_id, int loop)
{
    return priv.impl.play_music(music_id, loop);
}

tq_channel_state libtq_get_channel_state(int channel_id)
{
    return priv.impl.get_channel_state(channel_id);
}

void libtq_pause_channel(int channel_id)
{
    priv.impl.pause_channel(channel_id);
}

void libtq_unpause_channel(int channel_id)
{
    priv.impl.unpause_channel(channel_id);
}

void libtq_stop_channel(int channel_id)
{
    priv.impl.stop_channel(channel_id);
}

//------------------------------------------------------------------------------
