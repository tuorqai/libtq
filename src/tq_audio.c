
//------------------------------------------------------------------------------
// tq library :3
//------------------------------------------------------------------------------

#include <string.h>

#include "tq/tq.h"
#include "tq_audio.h"
#include "tq_error.h"
#include "tq_log.h"

//------------------------------------------------------------------------------

struct libtq_audio_priv
{
    struct libtq_audio_impl impl;
    int channel_source_id[TQ_CHANNEL_LIMIT];
};

static struct libtq_audio_priv priv;

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
    int real_channel_id = priv.impl.play_sound(sound_id, loop);

    if (real_channel_id == -1) {
        return 0;
    }

    priv.channel_source_id[real_channel_id] = sound_id;
    return encode_user_channel_id(sound_id, real_channel_id);
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
    int real_channel_id = priv.impl.play_music(music_id, loop);

    if (real_channel_id == -1) {
        return 0;
    }

    priv.channel_source_id[real_channel_id] = TQ_SOUND_LIMIT + music_id;

    return encode_user_channel_id(TQ_SOUND_LIMIT + music_id, real_channel_id);
}

tq_channel_state libtq_get_channel_state(int channel_id)
{
    int source_id, real_channel_id;
    decode_user_channel_id(channel_id, &source_id, &real_channel_id);

    if (priv.channel_source_id[real_channel_id] != source_id) {
        return TQ_CHANNEL_INACTIVE;
    }

    return priv.impl.get_channel_state(real_channel_id);
}

void libtq_pause_channel(int channel_id)
{
    int source_id, real_channel_id;
    decode_user_channel_id(channel_id, &source_id, &real_channel_id);

    if (priv.channel_source_id[real_channel_id] != source_id) {
        return;
    }

    priv.impl.pause_channel(real_channel_id);
}

void libtq_unpause_channel(int channel_id)
{
    int source_id, real_channel_id;
    decode_user_channel_id(channel_id, &source_id, &real_channel_id);

    if (priv.channel_source_id[real_channel_id] != source_id) {
        return;
    }

    priv.impl.unpause_channel(real_channel_id);
}

void libtq_stop_channel(int channel_id)
{
    int source_id, real_channel_id;
    decode_user_channel_id(channel_id, &source_id, &real_channel_id);

    if (priv.channel_source_id[real_channel_id] != source_id) {
        return;
    }

    priv.impl.stop_channel(real_channel_id);
}

//------------------------------------------------------------------------------
