
//------------------------------------------------------------------------------

#include "tq_audio.h"

//------------------------------------------------------------------------------

static void initialize(void)
{
    return;
}

static void terminate(void)
{
    return;
}

static void process(void)
{
    return;
}

static int load_sound(libtq_stream *stream)
{
    return 0;
}

static void delete_sound(int sound_id)
{
    return;
}

static int play_sound(int sound_id, int loop)
{
    return 0;
}

static int open_music(libtq_stream *stream)
{
    return 0;
}

static void close_music(int music_id)
{
    return;
}

static int play_music(int music_id, int loop)
{
    return 0;
}

static tq_channel_state get_channel_state(int channel_id)
{
    return TQ_CHANNEL_INACTIVE;
}

static void pause_channel(int channel_id)
{
    return;
}

static void unpause_channel(int channel_id)
{
    return;
}

static void stop_channel(int channel_id)
{
    return;
}

//------------------------------------------------------------------------------

void libtq_construct_null_audio(struct libtq_audio_impl *impl)
{
    impl->initialize            = initialize;
    impl->terminate             = terminate;
    impl->process               = process;
    impl->load_sound            = load_sound;
    impl->delete_sound          = delete_sound;
    impl->play_sound            = play_sound;
    impl->open_music            = open_music;
    impl->close_music           = close_music;
    impl->play_music            = play_music;
    impl->get_channel_state     = get_channel_state;
    impl->pause_channel         = pause_channel;
    impl->unpause_channel       = unpause_channel;
    impl->stop_channel          = stop_channel;
}

//------------------------------------------------------------------------------
