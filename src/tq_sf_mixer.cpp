
//------------------------------------------------------------------------------

#include <cstring>

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/InputStream.hpp>

extern "C" {
    #include "tq_audio.h"
    #include "tq_log.h"
    #include "tq_mixer.h"
}

//------------------------------------------------------------------------------

class input_stream_adapter : public sf::InputStream
{
public:
    input_stream_adapter(stream_t const *stream)
    {
        std::memcpy(&stream_, stream, sizeof(stream_t));
    }

    ~input_stream_adapter() noexcept
    {
        stream_.close(stream_.data);
    }

    sf::Int64 read(void *data, sf::Int64 size) override
    {
        return stream_.read(stream_.data, data, size);
    }

    sf::Int64 seek(sf::Int64 position) override
    {
        return stream_.seek(stream_.data, position);
    }

    sf::Int64 tell() override
    {
        return stream_.tell(stream_.data);
    }

    sf::Int64 getSize() override
    {
        return stream_.get_size(stream_.data);
    }

private:
    stream_t stream_;
};

//------------------------------------------------------------------------------

namespace
{
    sf::SoundBuffer     *sound_array[TQ_SOUND_LIMIT];
    sf::InputStream     *music_array[TQ_MUSIC_LIMIT];
    sf::SoundSource     *wave_array[TQ_WAVE_LIMIT];

    std::int32_t        current_sound_index;
    std::int32_t        current_music_index;
    std::int32_t        current_wave_index;

    std::int32_t get_sound_index()
    {
        std::int32_t index = current_sound_index++;

        if (current_sound_index == TQ_SOUND_LIMIT) {
            current_sound_index = 0;
        }

        if (sound_array[index]) {
            delete sound_array[index];
        }

        return index;
    }

    std::int32_t get_music_index()
    {
        std::int32_t index = current_music_index++;

        if (current_music_index == TQ_MUSIC_LIMIT) {
            current_music_index = 0;
        }

        if (music_array[index]) {
            delete music_array[index];
        }

        return index;
    }

    std::int32_t get_wave_index()
    {
        std::int32_t index = current_wave_index++;

        if (current_wave_index == TQ_WAVE_LIMIT) {
            current_wave_index = 0;
        }

        if (wave_array[index]) {
            delete wave_array[index];
        }

        return index;
    }

    void initialize()
    {
        log_info("SFML-based audio mixer is initialized.\n");
    }

    void terminate()
    {
        for (std::int32_t id = 0; id < TQ_WAVE_LIMIT; id++) {
            delete wave_array[id];
        }

        for (std::int32_t id = 0; id < TQ_MUSIC_LIMIT; id++) {
            delete music_array[id];
        }

        for (std::int32_t id = 0; id < TQ_SOUND_LIMIT; id++) {
            delete sound_array[id];
        }

        log_info("SFML-based audio mixer is terminated.\n");
    }

    tq_handle_t load_sound(std::uint8_t const *buffer, std::size_t length)
    {
        auto sound = new sf::SoundBuffer();

        if (!sound) {
            return TQ_INVALID_HANDLE;
        }

        if (!sound->loadFromMemory(buffer, length)) {
            delete sound;
            return TQ_INVALID_HANDLE;
        }

        std::int32_t index = get_sound_index();
        sound_array[index] = sound;

        return static_cast<tq_handle_t>(index);
    }

    void delete_sound(tq_handle_t sound_handle)
    {
        delete sound_array[sound_handle];
        sound_array[sound_handle] = nullptr;
    }

    tq_handle_t play_sound(tq_handle_t sound_handle, float left_volume, float right_volume, int loop)
    {
        TQ_CHECK_SOUND_HANDLE(sound_handle);

        if (!sound_array[sound_handle]) {
            return TQ_INVALID_HANDLE;
        }

        auto wave = new sf::Sound(*sound_array[sound_handle]);

        if (!wave) {
            return TQ_INVALID_HANDLE;
        }

        // ugly temporary solution #1
        if ((left_volume != 1.0f) || (right_volume != 1.0f)) {
            float x = (right_volume * 100.0f) - (left_volume * 100.0f);

            wave->setAttenuation(1000.0f);
            wave->setPosition({ x, 0.0f, 0.0f });
        }

        // ugly temporary solution #2
        if (loop == -1) {
            wave->setLoop(true);
        }

        wave->play();

        std::int32_t index = get_wave_index();
        wave_array[index] = wave;

        return static_cast<tq_handle_t>(index);
    }

    tq_handle_t open_music(stream_t const *stream)
    {
        auto music = new input_stream_adapter(stream);

        if (!music) {
            return TQ_INVALID_HANDLE;
        }

        std::int32_t index = get_music_index();
        music_array[index] = music;

        return static_cast<tq_handle_t>(index);
    }

    void close_music(tq_handle_t music_handle)
    {
        delete music_array[music_handle];
        music_array[music_handle] = nullptr;
    }

    tq_handle_t play_music(tq_handle_t music_handle, int loop)
    {
        if (!music_array[music_handle]) {
            return TQ_INVALID_HANDLE;
        }

        auto wave = new sf::Music();

        if (wave) {
            if (wave->openFromStream(*music_array[music_handle])) {
                if (loop == -1) {
                    wave->setLoop(true);
                }

                wave->play();

                std::int32_t index = get_wave_index();
                wave_array[index] = wave;
                return static_cast<tq_handle_t>(index);
            }

            delete wave;
        }

        return TQ_INVALID_HANDLE;
    }

    tq_wave_state_t get_wave_state(tq_handle_t wave_handle)
    {
        TQ_CHECK_WAVE_HANDLE(wave_handle);

        auto wave = wave_array[wave_handle];

        if (!wave) {
            return TQ_WAVE_STATE_INACTIVE;
        }

        switch (wave->getStatus()) {
        case sf::SoundSource::Paused:
            return TQ_WAVE_STATE_PAUSED;
        case sf::SoundSource::Playing:
            return TQ_WAVE_STATE_PLAYING;
        default:
            return TQ_WAVE_STATE_INACTIVE;
        }
    }

    void pause_wave(tq_handle_t wave_handle)
    {
        TQ_CHECK_WAVE_HANDLE(wave_handle);

        wave_array[wave_handle]->pause();
    }

    void unpause_wave(tq_handle_t wave_handle)
    {
        TQ_CHECK_WAVE_HANDLE(wave_handle);

        wave_array[wave_handle]->play();
    }

    void stop_wave(tq_handle_t wave_handle)
    {
        TQ_CHECK_WAVE_HANDLE(wave_handle);

        delete wave_array[wave_handle];
        wave_array[wave_handle] = nullptr;
    }
}

//------------------------------------------------------------------------------

void construct_sf_mixer(struct mixer *mixer)
{
    mixer->initialize       = ::initialize;
    mixer->terminate        = ::terminate;

    mixer->load_sound       = ::load_sound;
    mixer->delete_sound     = ::delete_sound;
    mixer->play_sound       = ::play_sound;

    mixer->open_music       = ::open_music;
    mixer->close_music      = ::close_music;
    mixer->play_music       = ::play_music;

    mixer->get_wave_state   = ::get_wave_state;
    mixer->pause_wave       = ::pause_wave;
    mixer->unpause_wave     = ::unpause_wave;
    mixer->stop_wave        = ::stop_wave;
}

//------------------------------------------------------------------------------