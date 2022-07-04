
#ifndef TQ_AUDIO_H_INC
#define TQ_AUDIO_H_INC

//------------------------------------------------------------------------------

#include "tq_stream.h"

//------------------------------------------------------------------------------

typedef struct tq_audio_impl
{
    void        (*initialize)(void);
    void        (*terminate)(void);

    int32_t     (*load_sound)(int32_t stream_id);
    void        (*delete_sound)(int32_t sound_id);
    int32_t     (*play_sound)(int32_t sound_id, int loop);

    int32_t     (*open_music)(int32_t stream_id);
    void        (*close_music)(int32_t music_id);
    int32_t     (*play_music)(int32_t music_id, int loop);

    tq_channel_state_t (*get_channel_state)(int32_t channel_id);
    void        (*pause_channel)(int32_t channel_id);
    void        (*unpause_channel)(int32_t channel_id);
    void        (*stop_channel)(int32_t channel_id);
} tq_audio_impl_t;

//------------------------------------------------------------------------------

#if defined(TQ_USE_OPENAL)
    void tq_construct_al_audio(tq_audio_impl_t *impl);
#endif

//------------------------------------------------------------------------------

void tq_audio_initialize(void);
void tq_audio_terminate(void);

int32_t tq_audio_load_sound_from_file(char const *path);
int32_t tq_audio_load_sound_from_memory(uint8_t const *buffer, size_t size);
void tq_audio_delete_sound(int32_t sound_id);
int32_t tq_audio_play_sound(int32_t sound_id, int loop);

int32_t tq_audio_open_music_from_file(char const *path);
int32_t tq_audio_open_music_from_memory(uint8_t const *buffer, size_t size);
void tq_audio_close_music(int32_t music_id);
int32_t tq_audio_play_music(int32_t music_id, int loop);

tq_channel_state_t tq_audio_get_channel_state(int32_t channel_id);
void tq_audio_pause_channel(int32_t channel_id);
void tq_audio_unpause_channel(int32_t channel_id);
void tq_audio_stop_channel(int32_t channel_id);

//------------------------------------------------------------------------------

#endif // TQ_AUDIO_H_INC
