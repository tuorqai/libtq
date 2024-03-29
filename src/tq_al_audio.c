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

#if defined(TQ_WIN32) || defined(TQ_LINUX) || defined(TQ_EMSCRIPTEN)

//------------------------------------------------------------------------------

#include <string.h>

#if defined(__EMSCRIPTEN__)
#   include <AL/al.h>
#   include <AL/alc.h>
#else
#   include <al.h>
#   include <alc.h>
#endif

#include "tq_audio.h"
#include "tq_core.h"
#include "tq_error.h"
#include "tq_mem.h"
#include "tq_log.h"
#include "tq_audio_dec.h"

//------------------------------------------------------------------------------
// OpenAL debug stuff

#if defined(NDEBUG)

#define CHECK_AL(call_) call_

#else

static char const *get_al_error_str(ALenum error)
{
    switch (error) {
    case AL_NO_ERROR:           return "No error, everything is OK.";
    case AL_INVALID_NAME:       return "Invalid name.";
    case AL_INVALID_ENUM:       return "Invalid enumeration.";
    case AL_INVALID_VALUE:      return "Invalid value.";
    case AL_INVALID_OPERATION:  return "Invalid operation.";
    case AL_OUT_OF_MEMORY:      return "Out of memory.";
    }

    return "Unknown error.";
}

static void check_al_errors(char const *call, char const *file, unsigned int line)
{
    ALenum error = alGetError();

    if (error != AL_NO_ERROR) {
        libtq_log(LIBTQ_LOG_WARNING, "OpenAL error(s) occured in %s at line %d:\n", file, line);
        libtq_log(LIBTQ_LOG_WARNING, "-- %d: %s\n", line, call);
    }

    while (error != AL_NO_ERROR) {
        libtq_log(LIBTQ_LOG_WARNING, ":: [0x%x] %s\n", error, get_al_error_str(error));
        error = alGetError();
    }
}

#define CHECK_AL(call_) \
    do { \
        call_; \
        check_al_errors(#call_, __FILE__, __LINE__); \
    } while (0);

#endif // defined(NDEBUG)

//------------------------------------------------------------------------------

enum {
    SOUND_BIT_LOADED = (1 << 0),
};

enum {
    MUSIC_BIT_OPEN = (1 << 0),
};

enum {
    CHANNEL_BIT_USED = (1 << 0),
    CHANNEL_BIT_STREAMING = (1 << 1),
};

struct openal_sounds
{
    uint8_t     bits[TQ_SOUND_LIMIT];
    ALuint      buffer[TQ_SOUND_LIMIT];
    int16_t     *samples[TQ_SOUND_LIMIT];
};

struct openal_music
{
    uint8_t     bits[TQ_MUSIC_LIMIT];
    libtq_stream *stream[TQ_MUSIC_LIMIT];
    libtq_audio_dec *dec[TQ_MUSIC_LIMIT];
};

struct openal_channels
{
    uint8_t     bits[TQ_CHANNEL_LIMIT];
    ALuint      source[TQ_CHANNEL_LIMIT];
    libtq_thread thread[TQ_CHANNEL_LIMIT];

    libtq_stream *stream[TQ_CHANNEL_LIMIT];
    libtq_audio_dec *dec[TQ_MUSIC_LIMIT];
    int         loop[TQ_CHANNEL_LIMIT];
};

struct openal
{
    ALCdevice *device;
    ALCcontext *context;

    libtq_mutex mutex;
    struct openal_sounds sounds;
    struct openal_music music;
    struct openal_channels channels;
};

//------------------------------------------------------------------------------

static struct openal openal;

//------------------------------------------------------------------------------
// Utility functions

/**
 * Find free sound identifier.
 */
static int32_t get_sound_id(void)
{
    for (int32_t sound_id = 0; sound_id < TQ_SOUND_LIMIT; sound_id++) {
        if ((openal.sounds.bits[sound_id] & SOUND_BIT_LOADED) == 0) {
            return sound_id;
        }
    }

    return -1;
}

/**
 * Find free music identifier.
 */
static int32_t get_music_id(void)
{
    for (int32_t music_id = 0; music_id < TQ_MUSIC_LIMIT; music_id++) {
        if ((openal.music.bits[music_id] & MUSIC_BIT_OPEN) == 0) {
            return music_id;
        }
    }

    return -1;
}

/**
 * Find free channel identifier.
 */
static int32_t get_channel_id(void)
{
    for (int32_t channel_id = 0; channel_id < TQ_CHANNEL_LIMIT; channel_id++) {
        if ((openal.channels.bits[channel_id] & CHANNEL_BIT_USED) == 0) {
            return channel_id;
        }

        if ((openal.channels.bits[channel_id] & CHANNEL_BIT_STREAMING) == 0) {
            ALint state = 0;
            CHECK_AL(alGetSourcei(openal.channels.source[channel_id], AL_SOURCE_STATE, &state));

            if (state == AL_INITIAL || state == AL_STOPPED) {
                return channel_id;
            }
        }
    }

    return -1;
}

/**
 * Choose appropriate audio format.
 */
static ALenum choose_format(int num_channels)
{
    switch (num_channels) {
    case 1:
        return AL_FORMAT_MONO16;
    case 2:
        return AL_FORMAT_STEREO16;
    }

    return AL_INVALID_ENUM;
}

//------------------------------------------------------------------------------

static bool initialize(void)
{
    memset(&openal, 0, sizeof(openal));

    openal.device = alcOpenDevice(NULL);

    if (openal.device == NULL) {
        libtq_log(LIBTQ_ERROR, "OpenAL: failed to open audio device.\n");
        return false;
    }

    openal.context = alcCreateContext(openal.device, NULL);

    if (openal.context == NULL) {
        libtq_log(LIBTQ_ERROR, "OpenAL: failed to create context.\n");
        alcCloseDevice(openal.device);
        return false;
    }

    if (!alcMakeContextCurrent(openal.context)) {
        libtq_log(LIBTQ_ERROR, "OpenAL: failed to activate context.\n");
        alcDestroyContext(openal.context);
        alcCloseDevice(openal.device);
        return false;
    }

    CHECK_AL(alGenSources(TQ_CHANNEL_LIMIT, openal.channels.source));

    openal.mutex = libtq_create_mutex();

    libtq_log(0, "OpenAL audio module is initialized.\n");
    return true;
}

static void terminate(void)
{
    /**
     * Stop sound channels.
     */
    for (int32_t channel_id = 0; channel_id < TQ_CHANNEL_LIMIT; channel_id++) {
        if (openal.channels.bits[channel_id] & CHANNEL_BIT_USED) {
            libtq_lock_mutex(openal.mutex);
            {
                CHECK_AL(alSourceStop(openal.channels.source[channel_id]));
            }
            libtq_unlock_mutex(openal.mutex);

            if (openal.channels.bits[channel_id] & CHANNEL_BIT_STREAMING) {
                libtq_wait_thread(openal.channels.thread[channel_id]);
            }
        }
    }

    /**
     * Delete sources (we need to do this before trying to delete buffers).
     */
    CHECK_AL(alDeleteSources(TQ_CHANNEL_LIMIT, openal.channels.source));

    /**
     * Close sound decoders and input streams opened by BGM.
     */
    for (int32_t music_id = 0; music_id < TQ_MUSIC_LIMIT; music_id++) {
        if (openal.music.bits[music_id] & MUSIC_BIT_OPEN) {
            libtq_audio_dec_close(openal.music.dec[music_id]);
            libtq_stream_close(openal.music.stream[music_id]);
        }
    }

    /**
     * Delete remaining sound buffers.
     */
    for (int32_t sound_id = 0; sound_id < TQ_SOUND_LIMIT; sound_id++) {
        if (openal.sounds.bits[sound_id] & SOUND_BIT_LOADED) {
            CHECK_AL(alDeleteBuffers(1, &openal.sounds.buffer[sound_id]));
            libtq_free(openal.sounds.samples[sound_id]);
        }
    }

    libtq_destroy_mutex(openal.mutex);

    alcMakeContextCurrent(NULL);
    alcDestroyContext(openal.context);
    alcCloseDevice(openal.device);

    libtq_log(0, "OpenAL audio module is terminated.\n");
}

static void process(void)
{
}

//------------------------------------------------------------------------------
// Options

static void set_master_volume(float volume)
{
    CHECK_AL(alListenerf(AL_GAIN, volume));
}

//------------------------------------------------------------------------------
// Sounds

static int32_t load_sound(libtq_stream *stream)
{
    int32_t sound_id = get_sound_id();

    if (sound_id == -1) {
        return -1;
    }

    libtq_audio_dec *dec = libtq_open_audio_dec(stream);

    if (!dec) {
        return -1;
    }

    ALenum format = choose_format(libtq_audio_dec_get_num_channels(dec));

    if (format == AL_INVALID_ENUM) {
        libtq_audio_dec_close(dec);
        return -1;
    }

    ALuint buffer = 0;
    CHECK_AL(alGenBuffers(1, &buffer));

    long long num_samples = libtq_audio_dec_get_num_samples(dec);
    short *samples = libtq_malloc(sizeof(short) * num_samples);

    libtq_audio_dec_read(dec, samples, num_samples);

    CHECK_AL(alBufferData(buffer, format,
        samples, sizeof(short) * num_samples,
        libtq_audio_dec_get_sample_rate(dec)));

    libtq_audio_dec_close(dec);

    openal.sounds.bits[sound_id] = SOUND_BIT_LOADED;
    openal.sounds.buffer[sound_id] = buffer;
    openal.sounds.samples[sound_id] = samples;

    return sound_id;
}

static void delete_sound(int32_t sound_id)
{
    if (sound_id < 0 || sound_id >= TQ_SOUND_LIMIT) {
        return;
    }

    if ((openal.sounds.bits[sound_id] & SOUND_BIT_LOADED) == 0) {
        return;
    }

    /**
     * Find all channels which still play this sound and shut them down.
     */
    for (int32_t channel_id = 0; channel_id < TQ_CHANNEL_LIMIT; channel_id++) {
        int bits = openal.channels.bits[channel_id];

        if ((bits & CHANNEL_BIT_USED) && !(bits & CHANNEL_BIT_STREAMING)) {
            ALint buffer;
            CHECK_AL(alGetSourcei(openal.channels.source[channel_id], AL_BUFFER, &buffer));

            if ((ALuint) buffer == openal.sounds.buffer[sound_id]) {
                CHECK_AL(alSourceStop(openal.channels.source[channel_id]));
                openal.channels.bits[channel_id] = 0;
            }
        }
    }

    CHECK_AL(alDeleteBuffers(1, &openal.sounds.buffer[sound_id]));
    libtq_free(openal.sounds.samples[sound_id]);

    openal.sounds.bits[sound_id] = 0;
    openal.sounds.buffer[sound_id] = 0;
    openal.sounds.samples[sound_id] = NULL;
}

static int32_t play_sound(int32_t sound_id, int loop)
{
    if (sound_id < 0 || sound_id >= TQ_SOUND_LIMIT) {
        return -1;
    }

    if ((openal.sounds.bits[sound_id] & SOUND_BIT_LOADED) == 0) {
        return -1;
    }

    int32_t channel_id = get_channel_id();

    if (channel_id == -1) {
        return -1;
    }

    openal.channels.bits[channel_id] = CHANNEL_BIT_USED;

    CHECK_AL(alSourcei(openal.channels.source[channel_id], AL_BUFFER,
        openal.sounds.buffer[sound_id]));

    CHECK_AL(alSourcei(openal.channels.source[channel_id], AL_LOOPING,
        (loop == -1) ? AL_TRUE : AL_FALSE));

    CHECK_AL(alSourcePlay(openal.channels.source[channel_id]));

    return channel_id;
}

//------------------------------------------------------------------------------
// Music streaming thread

/**
 * Music streaming constants.
 */
enum {
    MUSIC_BUFFER_COUNT = 4,
    MUSIC_BUFFER_LENGTH = 8192,
};

/**
 * Decode piece of music and fill the specified buffer.
 * Called from music streaming thread.
 */
static int fill_buffer(ALuint buffer, ALenum format,
    int16_t *samples, int sample_rate,
    libtq_audio_dec *dec, int *loops_left)
{
    long long samples_read = libtq_audio_dec_read(dec, samples, MUSIC_BUFFER_LENGTH);

    if (samples_read == 0) {
        if (*loops_left == -1 || *loops_left > 0) {
            libtq_audio_dec_seek(dec, 0);
            samples_read = libtq_audio_dec_read(dec, samples, MUSIC_BUFFER_LENGTH);
            
            if (*loops_left > 0) {
                (*loops_left)--;
            }
        }
    }

    if (samples_read == 0) {
        return -1;
    }

    CHECK_AL(alBufferData(buffer, format, samples, sizeof(short) * samples_read, sample_rate));

    return 0;
}

/**
 * Clear buffer queue for the specified source.
 * Called from music streaming thread.
 */
static void clear_buffer_queue(ALuint source)
{
    ALint buffers_queued;
    CHECK_AL(alGetSourcei(source, AL_BUFFERS_QUEUED, &buffers_queued));

    for (int n = 0; n < buffers_queued; n++) {
        ALuint buffer;
        CHECK_AL(alSourceUnqueueBuffers(source, 1, &buffer));
    }
}

/**
 * Main subroutine of music streaming thread.
 */
static int music_main(void *data)
{
    int32_t channel_id = (int32_t) ((intptr_t) data);
    libtq_audio_dec *dec = openal.channels.dec[channel_id];
    ALuint source = openal.channels.source[channel_id];

    int16_t *samples = libtq_malloc(sizeof(int16_t) * MUSIC_BUFFER_LENGTH);
    int sample_rate = libtq_audio_dec_get_sample_rate(dec);
    int num_channels = libtq_audio_dec_get_num_channels(dec);

    ALenum format = choose_format(num_channels);

    if (format == AL_INVALID_ENUM) {
        libtq_lock_mutex(openal.mutex);
        {
            openal.channels.bits[channel_id] &= ~CHANNEL_BIT_STREAMING;
        }
        libtq_unlock_mutex(openal.mutex);

        return -1;
    }

    int loops_left = openal.channels.loop[channel_id];

    CHECK_AL(alSourcei(source, AL_BUFFER, 0));
    CHECK_AL(alSourcei(source, AL_LOOPING, AL_FALSE));

    ALuint buffers[MUSIC_BUFFER_COUNT];
    CHECK_AL(alGenBuffers(MUSIC_BUFFER_COUNT, buffers));

    libtq_audio_dec_seek(dec, 0);
    clear_buffer_queue(source);

    /**
     * Fill the entire queue of buffers before we start streaming.
     */
    for (int n = 0; n < MUSIC_BUFFER_COUNT; n++) {
        fill_buffer(buffers[n], format, samples, sample_rate, dec, &loops_left);
        CHECK_AL(alSourceQueueBuffers(source, 1, &buffers[n]));
    }

    CHECK_AL(alSourcePlay(source));

    bool streaming = true;

    while (true) {
        bool paused = false;

        libtq_lock_mutex(openal.mutex);
        {
            ALint state;
            CHECK_AL(alGetSourcei(source, AL_SOURCE_STATE, &state));

            if (state == AL_PAUSED) {
                paused = true;
            } else if (state == AL_STOPPED) {
                streaming = false;
            }
        }
        libtq_unlock_mutex(openal.mutex);

        if (paused) {
            libtq_sleep(0.1);
            continue;
        }

        if (!streaming) {
            break;
        }

        ALint buffers_processed;
        CHECK_AL(alGetSourcei(source, AL_BUFFERS_PROCESSED, &buffers_processed));

        for (int n = 0; n < buffers_processed; n++) {
            ALuint buffer;
            CHECK_AL(alSourceUnqueueBuffers(source, 1, &buffer));

            if (fill_buffer(buffer, format, samples, sample_rate, dec, &loops_left) == -1) {
                break;
            }

            CHECK_AL(alSourceQueueBuffers(source, 1, &buffer));
        }

        libtq_sleep(0.1);
    }

    clear_buffer_queue(source);
    CHECK_AL(alDeleteBuffers(MUSIC_BUFFER_COUNT, buffers));
    libtq_free(samples);

    libtq_lock_mutex(openal.mutex);
    {
        openal.channels.bits[channel_id] &= ~CHANNEL_BIT_STREAMING;
    }
    libtq_unlock_mutex(openal.mutex);

    return 0;
}

//------------------------------------------------------------------------------
// Music

static int32_t open_music(libtq_stream *stream)
{
    int32_t music_id = get_music_id();

    if (music_id == -1) {
        return -1;
    }

    libtq_audio_dec *dec = libtq_open_audio_dec(stream);

    if (!dec) {
        return -1;
    }

    openal.music.bits[music_id] = MUSIC_BIT_OPEN;
    openal.music.stream[music_id] = stream;
    openal.music.dec[music_id] = dec;

    return music_id;
}

static void close_music(int32_t music_id)
{
    if (music_id < 0 || music_id >= TQ_SOUND_LIMIT) {
        return;
    }

    if ((openal.music.bits[music_id] & MUSIC_BIT_OPEN) == 0) {
        return;
    }

    for (int32_t channel_id = 0; channel_id < TQ_CHANNEL_LIMIT; channel_id++) {
        if (openal.channels.bits[channel_id] & CHANNEL_BIT_STREAMING) {
            if (openal.channels.stream[channel_id] == openal.music.stream[music_id]) {
                libtq_lock_mutex(openal.mutex);
                {
                    CHECK_AL(alSourceStop(openal.channels.source[channel_id]));
                }
                libtq_unlock_mutex(openal.mutex);

                libtq_wait_thread(openal.channels.thread[channel_id]);
            }
        }
    }

    libtq_audio_dec_close(openal.music.dec[music_id]);
    libtq_stream_close(openal.music.stream[music_id]);

    openal.music.bits[music_id] = 0;
    openal.music.stream[music_id] = NULL;
    openal.music.dec[music_id] = NULL;
}

static int32_t play_music(int32_t music_id, int loop)
{
    if (music_id < 0 || music_id >= TQ_SOUND_LIMIT) {
        return -1;
    }

    if ((openal.music.bits[music_id] & MUSIC_BIT_OPEN) == 0) {
        return -1;
    }

    /**
     * You can't play the same music more than once at a time.
     */
    for (int32_t channel_id = 0; channel_id < TQ_CHANNEL_LIMIT; channel_id++) {
        if (openal.channels.bits[channel_id] & CHANNEL_BIT_STREAMING) {
            if (openal.channels.stream[channel_id] == openal.music.stream[music_id]) {
                return -1;
            }
        }
    }

    int32_t channel_id = get_channel_id();

    if (channel_id == -1) {
        return -1;
    }

    openal.channels.bits[channel_id] = CHANNEL_BIT_USED | CHANNEL_BIT_STREAMING;

    openal.channels.stream[channel_id] = openal.music.stream[music_id];
    openal.channels.dec[channel_id] = openal.music.dec[music_id];
    openal.channels.loop[channel_id] = loop;

    openal.channels.thread[channel_id] = libtq_create_thread("music",
        music_main, (void *) ((intptr_t) channel_id));

    return 0;
}

//------------------------------------------------------------------------------
// Channels

static tq_channel_state get_channel_state(int32_t channel_id)
{
    if (channel_id < 0 || channel_id >= TQ_CHANNEL_LIMIT) {
        return TQ_CHANNEL_INACTIVE;
    }

    if ((openal.channels.bits[channel_id] & CHANNEL_BIT_USED) == 0) {
        return TQ_CHANNEL_INACTIVE;
    }

    tq_channel_state result = TQ_CHANNEL_INACTIVE;

    libtq_lock_mutex(openal.mutex);
    {
        ALint state;
        CHECK_AL(alGetSourcei(openal.channels.source[channel_id], AL_SOURCE_STATE, &state));

        if (state == AL_PLAYING) {
            result = TQ_CHANNEL_PLAYING;
        } else if (state == AL_PAUSED) {
            result = TQ_CHANNEL_PAUSED;
        }
    }
    libtq_unlock_mutex(openal.mutex);

    return result;
}

static void pause_channel(int32_t channel_id)
{
    if (channel_id < 0 || channel_id >= TQ_CHANNEL_LIMIT) {
        return;
    }

    if ((openal.channels.bits[channel_id] & CHANNEL_BIT_USED) == 0) {
        return;
    }

    libtq_lock_mutex(openal.mutex);
    {
        CHECK_AL(alSourcePause(openal.channels.source[channel_id]));
    }
    libtq_unlock_mutex(openal.mutex);
}

static void unpause_channel(int32_t channel_id)
{
    if (channel_id < 0 || channel_id >= TQ_CHANNEL_LIMIT) {
        return;
    }

    if ((openal.channels.bits[channel_id] & CHANNEL_BIT_USED) == 0) {
        return;
    }

    libtq_lock_mutex(openal.mutex);
    {
        ALint state;
        CHECK_AL(alGetSourcei(openal.channels.source[channel_id], AL_SOURCE_STATE, &state));

        if (state == AL_PAUSED) {
            CHECK_AL(alSourcePlay(openal.channels.source[channel_id]));
        }
    }
    libtq_unlock_mutex(openal.mutex);
}

static void stop_channel(int32_t channel_id)
{
    if (channel_id < 0 || channel_id >= TQ_CHANNEL_LIMIT) {
        return;
    }

    if ((openal.channels.bits[channel_id] & CHANNEL_BIT_USED) == 0) {
        return;
    }

    libtq_lock_mutex(openal.mutex);
    {
        CHECK_AL(alSourceStop(openal.channels.source[channel_id]));
        openal.channels.bits[channel_id] &= ~CHANNEL_BIT_USED;
    }
    libtq_unlock_mutex(openal.mutex);
}

//------------------------------------------------------------------------------
// Constructor

void tq_construct_al_audio(tq_audio_impl *impl)
{
    impl->initialize            = initialize;
    impl->terminate             = terminate;
    impl->process               = process;
    impl->set_master_volume     = set_master_volume;
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

#endif // defined(TQ_WIN32) || defined(TQ_LINUX)
