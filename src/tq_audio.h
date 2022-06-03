
#ifndef TQ_AUDIO_H_INC
#define TQ_AUDIO_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"
#include "tq_mixer.h"

//------------------------------------------------------------------------------

void audio_initialize(struct mixer const *mixer);
void audio_terminate(void);

tq_handle_t audio_load_sound_from_file(char const *path);
tq_handle_t audio_load_sound_from_memory(uint8_t const *buffer, size_t length);
void audio_delete_sound(tq_handle_t sound_handle);

void audio_play_sound(tq_handle_t sound_handle);
void audio_loop_sound(tq_handle_t sound_handle);
void audio_stop_sound(tq_handle_t sound_handle);

tq_handle_t audio_open_music_from_file(char const *path);
tq_handle_t audio_open_music_from_memory(uint8_t const *buffer, size_t length);
void audio_close_music(tq_handle_t music_handle);

void audio_play_music(tq_handle_t music_handle);
void audio_loop_music(tq_handle_t music_handle);
void audio_pause_music(tq_handle_t music_handle);
void audio_stop_music(tq_handle_t music_handle);

//------------------------------------------------------------------------------

#endif // TQ_AUDIO_H_INC
