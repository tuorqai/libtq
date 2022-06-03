
//------------------------------------------------------------------------------

#if defined(TQ_USE_SFML)

//------------------------------------------------------------------------------

#include <string.h>

#include <SFML/Audio.h>

#include "tq_handle_list.h"
#include "tq_mixer.h"
#include "tq_stream.h"

//------------------------------------------------------------------------------

typedef struct sf2_sound_pair
{
    sfSoundBuffer *buffer;
    sfSound *sound;
} sf2_sound_pair_t;

struct music
{
    stream_t        *stream;
    sfInputStream   *stream_adapter;
    sfMusic         *music;
};

static handle_list_t *sound_list;
static handle_list_t *music_list;

//------------------------------------------------------------------------------

static sfInt64 sf2_stream_read(void *dst, sfInt64 size, void *stream)
{
    return ((stream_t *) stream)->read(((stream_t *) stream)->data, dst, size);
}

static sfInt64 sf2_stream_seek(sfInt64 position, void *stream)
{
    return ((stream_t *) stream)->seek(((stream_t *) stream)->data, position);
}

static sfInt64 sf2_stream_tell(void *stream)
{
    return ((stream_t *) stream)->tell(((stream_t *) stream)->data);
}

static sfInt64 sf2_stream_get_size(void *stream)
{
    return ((stream_t *) stream)->get_size(((stream_t *) stream)->data);
}

static void sf2_stream_adapt(sfInputStream *sf2_stream, stream_t *stream)
{
    *sf2_stream = (sfInputStream) {
        .userData = stream,
        .read = sf2_stream_read,
        .seek = sf2_stream_seek,
        .tell = sf2_stream_tell,
        .getSize = sf2_stream_get_size,
    };
}

//------------------------------------------------------------------------------

static void sound_dtor(void *item)
{
    sf2_sound_pair_t *pair = (sf2_sound_pair_t *) item;

    sfSound_destroy(pair->sound);
    sfSoundBuffer_destroy(pair->buffer);
}

static void music_dtor(void *item)
{
    struct music *music = (struct music *) item;

    music->stream->close(music->stream->data);

    sfMusic_destroy(music->music);
    free(music->stream_adapter);
    free(music->stream);
}

//------------------------------------------------------------------------------

static void sf2_mixer_initialize(void)
{
    sound_list = handle_list_create(sizeof(sf2_sound_pair_t), sound_dtor);
    music_list = handle_list_create(sizeof(struct music), music_dtor);
}

static void sf2_mixer_terminate(void)
{
    handle_list_destroy(music_list);
    handle_list_destroy(sound_list);
}

static tq_handle_t sf2_mixer_load_sound(uint8_t const *buffer, size_t length)
{
    sf2_sound_pair_t pair;

    if (!(pair.buffer = sfSoundBuffer_createFromMemory(buffer, length))) {
        return TQ_INVALID_HANDLE;
    }

    pair.sound = sfSound_create();
    sfSound_setBuffer(pair.sound, pair.buffer);

    return handle_list_append(sound_list, &pair);
}

static void sf2_mixer_delete_sound(tq_handle_t sound_handle)
{
    handle_list_erase(sound_list, sound_handle);
}

static void sf2_mixer_play_sound(tq_handle_t sound_handle)
{
    sf2_sound_pair_t *pair = (sf2_sound_pair_t *) handle_list_get(sound_list, sound_handle);

    if (!pair) {
        return;
    }

    sfSound_setLoop(pair->sound, sfFalse);
    sfSound_play(pair->sound);
}

static void sf2_mixer_loop_sound(tq_handle_t sound_handle)
{
    sf2_sound_pair_t *pair = (sf2_sound_pair_t *) handle_list_get(sound_list, sound_handle);

    if (!pair) {
        return;
    }

    sfSound_setLoop(pair->sound, sfTrue);
    sfSound_play(pair->sound);
}

static void sf2_mixer_stop_sound(tq_handle_t sound_handle)
{
    sf2_sound_pair_t *pair = (sf2_sound_pair_t *) handle_list_get(sound_list, sound_handle);

    if (!pair) {
        return;
    }

    sfSound_stop(pair->sound);
}

static tq_handle_t sf2_mixer_open_music(stream_t const *stream)
{
    struct music music = {
        .stream = malloc(sizeof(stream_t)),
        .stream_adapter = malloc(sizeof(sfInputStream)),
    };

    if (!music.stream || !music.stream_adapter) {
        return TQ_INVALID_HANDLE;
    }

    memcpy(music.stream, stream, sizeof(stream_t));
    sf2_stream_adapt(music.stream_adapter, music.stream);

    music.music = sfMusic_createFromStream(music.stream_adapter);

    if (!music.music) {
        free(music.stream_adapter);
        free(music.stream);

        return TQ_INVALID_HANDLE;
    }

    return handle_list_append(music_list, &music);
}

static void sf2_mixer_close_music(tq_handle_t music_handle)
{
    handle_list_destroy(music_list);
}

static void sf2_mixer_play_music(tq_handle_t music_handle)
{
    struct music *music = handle_list_get(music_list, music_handle);

    if (!music) {
        return;
    }

    sfMusic_setLoop(music->music, sfFalse);
    sfMusic_play(music->music);
}

static void sf2_mixer_loop_music(tq_handle_t music_handle)
{
    struct music *music = handle_list_get(music_list, music_handle);

    if (!music) {
        return;
    }

    sfMusic_setLoop(music->music, sfTrue);
    sfMusic_play(music->music);
}

static void sf2_mixer_pause_music(tq_handle_t music_handle)
{
    struct music *music = handle_list_get(music_list, music_handle);

    if (!music) {
        return;
    }

    sfMusic_pause(music->music);
}

static void sf2_mixer_stop_music(tq_handle_t music_handle)
{
    struct music *music = handle_list_get(music_list, music_handle);

    if (!music) {
        return;
    }

    sfMusic_stop(music->music);
}

//------------------------------------------------------------------------------

void construct_sf2_mixer(struct mixer *mixer)
{
    *mixer = (struct mixer) {
        .initialize     = sf2_mixer_initialize,
        .terminate      = sf2_mixer_terminate,
        .load_sound     = sf2_mixer_load_sound,
        .delete_sound   = sf2_mixer_delete_sound,
        .play_sound     = sf2_mixer_play_sound,
        .loop_sound     = sf2_mixer_loop_sound,
        .stop_sound     = sf2_mixer_stop_sound,
        .open_music     = sf2_mixer_open_music,
        .close_music    = sf2_mixer_close_music,
        .play_music     = sf2_mixer_play_music,
        .loop_music     = sf2_mixer_loop_music,
        .pause_music    = sf2_mixer_pause_music,
        .stop_music     = sf2_mixer_stop_music,
    };
}

//------------------------------------------------------------------------------

#endif // defined(TQ_USE_SFML)

//------------------------------------------------------------------------------