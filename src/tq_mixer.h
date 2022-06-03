
#ifndef TQ_MIXER_H_INC
#define TQ_MIXER_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"
#include "tq_file.h"

//------------------------------------------------------------------------------

struct mixer
{
    void (*initialize)(void);
    void (*terminate)(void);
    tq_handle_t (*load_sound)(uint8_t const *, size_t);
    void (*delete_sound)(tq_handle_t);
    void (*play_sound)(tq_handle_t);
    void (*loop_sound)(tq_handle_t);
    void (*stop_sound)(tq_handle_t);
    tq_handle_t (*open_music)(char const *);
    void (*close_music)(tq_handle_t);
    void (*play_music)(tq_handle_t);
    void (*loop_music)(tq_handle_t);
    void (*pause_music)(tq_handle_t);
    void (*stop_music)(tq_handle_t);
};

//------------------------------------------------------------------------------

#if defined(TQ_USE_SFML)
    void construct_sf2_mixer(struct mixer *mixer);
#endif

//------------------------------------------------------------------------------

#endif // TQ_MIXER_H_INC
