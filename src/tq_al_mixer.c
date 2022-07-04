
//------------------------------------------------------------------------------

#if defined(TQ_USE_OPENAL)

//------------------------------------------------------------------------------

#include <string.h>

#include <al.h>
#include <alc.h>

#include "tq_error.h"
#include "tq_log.h"
#include "tq_mixer.h"
#include "tq_sound_loader.h"

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
        log_error("OpenAL error(s) occured in %s at line %d:\n", file, line);
        log_error("-- %d: %s\n", line, call);
    }

    while (error != AL_NO_ERROR) {
        log_error(":: [0x%x] %s\n", error, get_al_error_str(error));
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
// Declarations

typedef struct tq_al_mixer_priv
{
    ALCdevice       *device;
    ALCcontext      *context;

    ALuint          buffers[TQ_SOUND_LIMIT];
    ALuint          sources[TQ_CHANNEL_LIMIT];
} tq_al_mixer_priv_t;

//------------------------------------------------------------------------------
// Definitions

static tq_al_mixer_priv_t al;

//------------------------------------------------------------------------------
// Utility functions

static int32_t get_buffer_index(void)
{
    for (int32_t index = 0; index < TQ_SOUND_LIMIT; index++) {
        if (al.buffers[index] == 0) {
            return index;
        }
    }

    return -1;
}

static int32_t get_source_index(void)
{
    for (int32_t index = 0; index < TQ_CHANNEL_LIMIT; index++) {
        ALint state;
        
        CHECK_AL(alGetSourcei(al.sources[index], AL_SOURCE_STATE, &state));

        if (state == AL_INITIAL || state == AL_STOPPED) {
            return index;
        }
    }

    return -1;
}

static ALenum choose_format(tq_sound_t const *sound)
{
    if (sound->num_channels == 1) {
        if (sound->bytes_per_sample == 1) {
            return AL_FORMAT_MONO8;
        } else if (sound->bytes_per_sample == 2) {
            return AL_FORMAT_MONO16;
        }
    } else if (sound->num_channels == 2) {
        if (sound->bytes_per_sample == 1) {
            return AL_FORMAT_STEREO8;
        } else if (sound->bytes_per_sample == 2) {
            return AL_FORMAT_STEREO16;
        }
    }

    return AL_INVALID_ENUM;
}

//------------------------------------------------------------------------------
// Implementation

static void initialize(void)
{
    memset(&al, 0, sizeof(tq_al_mixer_priv_t));

    al.device = alcOpenDevice(NULL);

    if (al.device == NULL) {
        tq_error("OpenAL: failed to open audio device.");
    }

    al.context = alcCreateContext(al.device, NULL);

    if (al.context == NULL) {
        tq_error("OpenAL: failed to create context.");
    }

    if (!alcMakeContextCurrent(al.context)) {
        tq_error("OpenAL: failed to activate context.");
    }

    CHECK_AL(alGenSources(TQ_CHANNEL_LIMIT, al.sources));

    log_info("OpenAL audio module is initialized.\n");
}

static void terminate(void)
{
    CHECK_AL(alDeleteSources(TQ_CHANNEL_LIMIT, al.sources));
    CHECK_AL(alDeleteBuffers(TQ_SOUND_LIMIT, al.buffers));

    alcMakeContextCurrent(NULL);
    alcDestroyContext(al.context);
    alcCloseDevice(al.device);

    log_info("OpenAL audio module is terminated.\n");
}

static int32_t load_sound(int32_t stream_id)
{
    int32_t index = get_buffer_index();

    if (index == -1) {
        return -1;
    }

    tq_sound_t *sound = tq_sound_load(stream_id);

    if (sound == NULL) {
        return -1;
    }

    ALenum format = choose_format(sound);

    if (format == AL_INVALID_ENUM) {
        tq_sound_destroy(sound);
        return -1;
    }

    CHECK_AL(alGenBuffers(1, &al.buffers[index]));

    CHECK_AL(alBufferData(al.buffers[index], format,
        sound->samples, sound->num_samples * sound->bytes_per_sample,
        sound->sample_rate));

    tq_sound_destroy(sound);

    return index;
}

static void delete_sound(int32_t sound_id)
{
    if (sound_id < 0 || sound_id >= TQ_SOUND_LIMIT) {
        return;
    }

    int32_t const buffer_id = sound_id;

    if (al.buffers[buffer_id] == 0) {
        return;
    }

    for (int32_t source_id = 0; source_id < TQ_CHANNEL_LIMIT; source_id++) {
        if (al.sources[source_id] == 0) {
            continue;
        }

        ALint buffer;
        CHECK_AL(alGetSourcei(al.sources[source_id], AL_BUFFER, &buffer));

        if (al.buffers[buffer_id] == buffer) {
            CHECK_AL(alDeleteSources(1, &al.sources[source_id]));
            al.sources[source_id] = 0;
        }
    }

    CHECK_AL(alDeleteBuffers(1, &al.buffers[sound_id]));

    al.buffers[sound_id] = 0;
}

static int32_t play_sound(int32_t sound_id, int loop)
{
    int32_t index = get_source_index();

    if (index == -1) {
        return -1;
    }

    CHECK_AL(alSourcei(al.sources[index], AL_BUFFER, al.buffers[sound_id]));
    CHECK_AL(alSourcei(al.sources[index], AL_LOOPING, (loop == -1) ? AL_TRUE : AL_FALSE));

    CHECK_AL(alSourcePlay(al.sources[index]));

    return index;
}

static int32_t open_music(int32_t stream_id)
{
    return 0;
}

static void close_music(int32_t music_id)
{
}

static int32_t play_music(int32_t music_id, int loop)
{
    return 0;
}

static tq_channel_state_t get_channel_state(int32_t channel_id)
{
    if (channel_id < 0 || channel_id >= TQ_CHANNEL_LIMIT) {
        return TQ_CHANNEL_STATE_INACTIVE;
    }

    if (al.sources[channel_id] == 0) {
        return TQ_CHANNEL_STATE_INACTIVE;
    }

    ALint state;
    CHECK_AL(alGetSourcei(al.sources[channel_id], AL_SOURCE_STATE, &state));

    if (state == AL_PLAYING) {
        return TQ_CHANNEL_STATE_PLAYING;
    } else if (state == AL_PAUSED) {
        return TQ_CHANNEL_STATE_PAUSED;
    }

    return TQ_CHANNEL_STATE_INACTIVE;
}

static void pause_channel(int32_t channel_id)
{
    if (channel_id < 0 || channel_id >= TQ_CHANNEL_LIMIT) {
        return;
    }

    if (al.sources[channel_id] == 0) {
        return;
    }

    CHECK_AL(alSourcePause(al.sources[channel_id]));
}

static void unpause_channel(int32_t channel_id)
{
    if (channel_id < 0 || channel_id >= TQ_CHANNEL_LIMIT) {
        return;
    }

    if (al.sources[channel_id] == 0) {
        return;
    }

    ALint state;
    CHECK_AL(alGetSourcei(al.sources[channel_id], AL_SOURCE_STATE, &state));

    if (state == AL_PAUSED) {
        CHECK_AL(alSourcePlay(al.sources[channel_id]));
    }
}

static void stop_channel(int32_t channel_id)
{
    if (channel_id < 0 || channel_id >= TQ_CHANNEL_LIMIT) {
        return;
    }

    if (al.sources[channel_id] == 0) {
        return;
    }

    CHECK_AL(alSourceStop(al.sources[channel_id]));
}

//------------------------------------------------------------------------------
// Constructor

void tq_construct_al_mixer(struct mixer *mixer)
{
    *mixer = (struct mixer) {
        .initialize             = initialize,
        .terminate              = terminate,
        .load_sound             = load_sound,
        .delete_sound           = delete_sound,
        .play_sound             = play_sound,
        .open_music             = open_music,
        .close_music            = close_music,
        .play_music             = play_music,
        .get_channel_state      = get_channel_state,
        .pause_channel          = pause_channel,
        .unpause_channel        = unpause_channel,
        .stop_channel           = stop_channel,
    };
}

//------------------------------------------------------------------------------

#endif // defined(TQ_USE_OPENAL)
