
#ifndef TQ_AUDIO_H_INC
#define TQ_AUDIO_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"
#include "tq_log.h"
#include "tq_mixer.h"

//------------------------------------------------------------------------------

void audio_initialize(struct mixer const *mixer);
void audio_terminate(void);

int32_t audio_load_sound_from_file(char const *path);
int32_t audio_load_sound_from_memory(uint8_t const *buffer, size_t length);
void audio_delete_sound(int32_t sound_id);
int32_t audio_play_sound(int32_t sound_id, int loop);

int32_t audio_open_music_from_file(char const *path);
int32_t audio_open_music_from_memory(uint8_t const *buffer, size_t length);
void audio_close_music(int32_t music_id);
int32_t audio_play_music(int32_t music_id, int loop);

tq_channel_state_t audio_get_channel_state(int32_t channel_id);
void audio_pause_channel(int32_t channel_id);
void audio_unpause_channel(int32_t channel_id);
void audio_stop_channel(int32_t channel_id);

//------------------------------------------------------------------------------

#endif // TQ_AUDIO_H_INC
