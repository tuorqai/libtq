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

#include "tq_audio.h"

//------------------------------------------------------------------------------

static bool initialize(void)
{
    return true;
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

void tq_construct_null_audio(tq_audio_impl *impl)
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
