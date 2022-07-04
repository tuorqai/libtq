
#ifndef TQ_MIXER_H_INC
#define TQ_MIXER_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"
#include "tq_stream.h"

//------------------------------------------------------------------------------

struct mixer
{
    void        (*initialize)(void);
    void        (*terminate)(void);

    int32_t     (*load_sound)(int32_t stream_id);
    void        (*delete_sound)(int32_t sound_id);
    int32_t     (*play_sound)(int32_t sound_id, int loop);

    int32_t     (*open_music)(int32_t stream_id);
    void        (*close_music)(int32_t music_id);
    int32_t     (*play_music)(int32_t music_id, int loop);

    tq_channel_state_t  (*get_channel_state)(int32_t channel_id);
    void                (*pause_channel)(int32_t channel_id);
    void                (*unpause_channel)(int32_t channel_id);
    void                (*stop_channel)(int32_t channel_id);
};

//------------------------------------------------------------------------------

#if defined(TQ_USE_OPENAL)
    void tq_construct_al_mixer(struct mixer *mixer);
#endif

//------------------------------------------------------------------------------

#endif // TQ_MIXER_H_INC
