
//------------------------------------------------------------------------------

#if defined(TQ_USE_SFML)

//------------------------------------------------------------------------------

#include <SFML/Audio.h>

#include "tq_handle_list.h"
#include "tq_mixer.h"

//------------------------------------------------------------------------------

typedef struct sf2_sound_pair
{
    sfSoundBuffer *buffer;
    sfSound *sound;
} sf2_sound_pair_t;

static handle_list_t *sound_list;
static handle_list_t *music_list;

//------------------------------------------------------------------------------

static void sound_dtor(void *item)
{
    sf2_sound_pair_t *pair = (sf2_sound_pair_t *) item;

    sfSound_destroy(pair->sound);
    sfSoundBuffer_destroy(pair->buffer);
}

static void music_dtor(void *item)
{
    sfMusic_destroy(*(sfMusic **) item);
}

//------------------------------------------------------------------------------

static void sf2_mixer_initialize(void)
{
    sound_list = handle_list_create(sizeof(sf2_sound_pair_t), sound_dtor);
    music_list = handle_list_create(sizeof(sfMusic *), music_dtor);
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

static tq_handle_t sf2_mixer_open_music(char const *path)
{
    sfMusic *music = sfMusic_createFromFile(path);

    if (!music) {
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
    sfMusic **music = (sfMusic **) handle_list_get(music_list, music_handle);

    if (!music) {
        return;
    }

    sfMusic_setLoop(*music, sfFalse);
    sfMusic_play(*music);
}

static void sf2_mixer_loop_music(tq_handle_t music_handle)
{
    sfMusic **music = (sfMusic **) handle_list_get(music_list, music_handle);

    if (!music) {
        return;
    }

    sfMusic_setLoop(*music, sfTrue);
    sfMusic_play(*music);
}

static void sf2_mixer_pause_music(tq_handle_t music_handle)
{
    sfMusic **music = (sfMusic **) handle_list_get(music_list, music_handle);

    if (!music) {
        return;
    }

    sfMusic_pause(*music);
}

static void sf2_mixer_stop_music(tq_handle_t music_handle)
{
    sfMusic **music = (sfMusic **) handle_list_get(music_list, music_handle);

    if (!music) {
        return;
    }

    sfMusic_stop(*music);
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