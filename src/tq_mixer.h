
#ifndef TQ_MIXER_H_INC
#define TQ_MIXER_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"
#include "tq_file.h"
#include "tq_stream.h"

//------------------------------------------------------------------------------

struct mixer
{
    void            (*initialize)(void);
    void            (*terminate)(void);

    tq_handle_t     (*load_sound)(uint8_t const *, size_t);
    void            (*delete_sound)(tq_handle_t);
    tq_handle_t     (*play_sound)(tq_handle_t, float, float, int);

    tq_handle_t     (*open_music)(stream_t const *);
    void            (*close_music)(tq_handle_t);
    tq_handle_t     (*play_music)(tq_handle_t, int);

    tq_wave_state_t (*get_wave_state)(tq_handle_t);
    void            (*pause_wave)(tq_handle_t);
    void            (*unpause_wave)(tq_handle_t);
    void            (*stop_wave)(tq_handle_t);
};

//------------------------------------------------------------------------------

#if defined(TQ_USE_SFML)
    void construct_sf_mixer(struct mixer *mixer);
#endif

//------------------------------------------------------------------------------

#endif // TQ_MIXER_H_INC
