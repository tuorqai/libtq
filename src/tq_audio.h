
//------------------------------------------------------------------------------

#ifndef TQ_AUDIO_H_INC
#define TQ_AUDIO_H_INC

//------------------------------------------------------------------------------

#include "tq_stream.h"

//------------------------------------------------------------------------------

struct libtq_audio_impl
{
    void                (*initialize)(void);
    void                (*terminate)(void);
    void                (*process)(void);

    int                 (*load_sound)(libtq_stream *stream);
    void                (*delete_sound)(int sound_id);
    int                 (*play_sound)(int sound_id, int loop);

    int                 (*open_music)(libtq_stream *stream);
    void                (*close_music)(int music_id);
    int                 (*play_music)(int music_id, int loop);

    tq_channel_state    (*get_channel_state)(int channel_id);
    void                (*pause_channel)(int channel_id);
    void                (*unpause_channel)(int channel_id);
    void                (*stop_channel)(int channel_id);
};

//------------------------------------------------------------------------------

void libtq_construct_null_audio(struct libtq_audio_impl *impl);

#if defined(TQ_USE_OPENAL)
    void libtq_construct_al_audio(struct libtq_audio_impl *impl);
#endif

//------------------------------------------------------------------------------

void                libtq_initialize_audio(void);
void                libtq_terminate_audio(void);
void                libtq_process_audio(void);

int                 libtq_load_sound(libtq_stream *stream);
void                libtq_delete_sound(int sound_id);
int                 libtq_play_sound(int sound_id, int loop);

int                 libtq_open_music(libtq_stream *stream);
void                libtq_close_music(int music_id);
int                 libtq_play_music(int music_id, int loop);

tq_channel_state    libtq_get_channel_state(int channel_id);
void                libtq_pause_channel(int channel_id);
void                libtq_unpause_channel(int channel_id);
void                libtq_stop_channel(int channel_id);

//------------------------------------------------------------------------------

#endif // TQ_AUDIO_H_INC

//------------------------------------------------------------------------------
