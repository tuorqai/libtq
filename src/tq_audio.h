
#ifndef TQ_AUDIO_H_INC
#define TQ_AUDIO_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"
#include "tq_log.h"
#include "tq_mixer.h"

//------------------------------------------------------------------------------

#if defined(NDEBUG)

#define TQ_CHECK_SOUND_HANDLE(handle_)
#define TQ_CHECK_SOUND_STREAM_HANDLE(handle_)
#define TQ_CHECK_WAVE_HANDLE(handle_)

#else

#define TQ_CHECK_SOUND_HANDLE(handle_) \
    do { \
        if (((handle_) < 0 ) || ((handle_) >= TQ_SOUND_LIMIT)) { \
            log_error("Invalid sound handle #%d (limit is %d)!\n", \
                (handle_), TQ_SOUND_LIMIT); \
        } \
    } while (0);

#define TQ_CHECK_SOUND_STREAM_HANDLE(handle_) \
    do { \
        if (((handle_) < 0 ) || ((handle_) >= TQ_SOUND_STREAM_LIMIT)) { \
            log_error("Invalid sound stream handle #%d (limit is %d)!\n", \
                (handle_), TQ_SOUND_STREAM_LIMIT); \
        } \
    } while (0);

#define TQ_CHECK_WAVE_HANDLE(handle_) \
    do { \
        if (((handle_) < 0 ) || ((handle_) >= TQ_WAVE_LIMIT)) { \
            log_error("Invalid wave handle #%d (limit is %d)!\n", \
                (handle_), TQ_WAVE_LIMIT); \
        } \
    } while (0);

#endif

//------------------------------------------------------------------------------

void audio_initialize(struct mixer const *mixer);
void audio_terminate(void);

tq_handle_t audio_load_sound_from_file(char const *path);
tq_handle_t audio_load_sound_from_memory(uint8_t const *buffer, size_t length);
void audio_delete_sound(tq_handle_t sound_handle);
tq_handle_t audio_play_sound(tq_handle_t sound_handle, float left_volume, float right_volume, int loop);

tq_handle_t audio_open_music_from_file(char const *path);
tq_handle_t audio_open_music_from_memory(uint8_t const *buffer, size_t length);
void audio_close_music(tq_handle_t music_handle);
tq_handle_t audio_play_music(tq_handle_t music_handle, int loop);

tq_wave_state_t audio_get_wave_state(tq_handle_t wave_handle);
void audio_pause_wave(tq_handle_t wave_handle);
void audio_unpause_wave(tq_handle_t wave_handle);
void audio_stop_wave(tq_handle_t wave_handle);

//------------------------------------------------------------------------------

#endif // TQ_AUDIO_H_INC
