
//------------------------------------------------------------------------------

#if defined(TQ_USE_SFML) && !defined(TQ_USE_OPENAL)

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
    sf::SoundSource     *channel_array[TQ_CHANNEL_LIMIT];

    int32_t get_sound_index()
    {
        for (int32_t index = 0; index < TQ_SOUND_LIMIT; index++) {
            if (sound_array[index] == nullptr) {
                return index;
            }
        }

        return -1;
    }

    int32_t get_music_index()
    {
        for (int32_t index = 0; index < TQ_MUSIC_LIMIT; index++) {
            if (music_array[index] == nullptr) {
                return index;
            }
        }

        return -1;
    }

    int32_t get_channel_index()
    {
        for (int32_t index = 0; index < TQ_MUSIC_LIMIT; index++) {
            if (channel_array[index] == nullptr) {
                return index;
            }

            if (channel_array[index]->getStatus() == sf::SoundSource::Stopped) {
                delete channel_array[index];
                return index;
            }
        }

        return -1;
    }

    void initialize()
    {
        std::memset(sound_array, 0, sizeof(sound_array));
        std::memset(music_array, 0, sizeof(music_array));
        std::memset(channel_array, 0, sizeof(channel_array));

        log_info("SFML-based audio mixer is initialized.\n");
    }

    void terminate()
    {
        for (int32_t index = 0; index < TQ_CHANNEL_LIMIT; index++) {
            delete channel_array[index];
        }

        for (int32_t index = 0; index < TQ_MUSIC_LIMIT; index++) {
            delete music_array[index];
        }

        for (int32_t index = 0; index < TQ_SOUND_LIMIT; index++) {
            delete sound_array[index];
        }

        log_info("SFML-based audio mixer is terminated.\n");
    }

    int32_t load_sound(stream_t const *stream)
    {
        int32_t index = get_sound_index();

        if (index == -1) {
            return -1;
        }

        auto sound = new sf::SoundBuffer();

        if (sound == nullptr) {
            return -1;
        }

        uint8_t const *buffer = (uint8_t const *) stream->buffer(stream->data);
        size_t size = stream->get_size(stream->data);

        if (!sound->loadFromMemory(buffer, size)) {
            delete sound;
            return -1;
        }

        sound_array[index] = sound;
        return index;
    }

    void delete_sound(int32_t sound_id)
    {
        delete sound_array[sound_id];
        sound_array[sound_id] = nullptr;
    }

    int32_t play_sound(int32_t sound_id, int loop)
    {
        if (sound_id < 0 || sound_id >= TQ_SOUND_LIMIT) {
            return -1;
        }

        if (sound_array[sound_id] == nullptr) {
            return -1;
        }

        int32_t index = get_channel_index();

        if (index == -1) {
            return -1;
        }

        auto channel = new sf::Sound(*sound_array[sound_id]);

        if (channel == nullptr) {
            return -1;
        }

        // ugly temporary solution #2
        if (loop == -1) {
            channel->setLoop(true);
        }

        channel->play();

        channel_array[index] = channel;
        return index;
    }

    int32_t open_music(stream_t const *stream)
    {
        int32_t index = get_music_index();

        if (index == -1) {
            return -1;
        }

        auto music = new input_stream_adapter(stream);

        if (!music) {
            return -1;
        }

        music_array[index] = music;
        return index;
    }

    void close_music(int32_t music_id)
    {
        delete music_array[music_id];
        music_array[music_id] = nullptr;
    }

    int32_t play_music(int32_t music_id, int loop)
    {
        if (music_id < 0 || music_id >= TQ_MUSIC_LIMIT) {
            return -1;
        }
    
        if (music_array[music_id] == nullptr) {
            return -1;
        }

        int32_t index = get_channel_index();

        if (index == -1) {
            return -1;
        }

        auto channel = new sf::Music();

        if (channel == nullptr) {
            return -1;
        }

        if (!channel->openFromStream(*music_array[music_id])) {
            delete channel;
            return -1;
        }

        if (loop == -1) {
            channel->setLoop(true);
        }

        channel->play();

        channel_array[index] = channel;
        return index;
    }

    tq_channel_state_t get_channel_state(int32_t channel_id)
    {
        if (channel_id < 0 || channel_id >= TQ_CHANNEL_LIMIT) {
            return TQ_CHANNEL_STATE_INACTIVE;
        }

        if (channel_array[channel_id] == nullptr) {
            return TQ_CHANNEL_STATE_INACTIVE;
        }

        switch (channel_array[channel_id]->getStatus()) {
        case sf::SoundSource::Paused:
            return TQ_CHANNEL_STATE_PAUSED;
        case sf::SoundSource::Playing:
            return TQ_CHANNEL_STATE_PLAYING;
        default:
            return TQ_CHANNEL_STATE_INACTIVE;
        }
    }

    void pause_channel(int32_t channel_id)
    {
        if (channel_id < 0 || channel_id >= TQ_CHANNEL_LIMIT) {
            return;
        }

        if (channel_array[channel_id] == nullptr) {
            return;
        }

        channel_array[channel_id]->pause();
    }

    void unpause_channel(int32_t channel_id)
    {
        if (channel_id < 0 || channel_id >= TQ_CHANNEL_LIMIT) {
            return;
        }

        if (channel_array[channel_id] == nullptr) {
            return;
        }

        channel_array[channel_id]->play();
    }

    void stop_channel(int32_t channel_id)
    {
        if (channel_id < 0 || channel_id >= TQ_CHANNEL_LIMIT) {
            return;
        }

        if (channel_array[channel_id] == nullptr) {
            return;
        }

        delete channel_array[channel_id];
        channel_array[channel_id] = nullptr;
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

    mixer->get_channel_state    = ::get_channel_state;
    mixer->pause_channel        = ::pause_channel;
    mixer->unpause_channel      = ::unpause_channel;
    mixer->stop_channel         = ::stop_channel;
}

//------------------------------------------------------------------------------

#endif // defined(TQ_USE_SFML) && !defined(TQ_USE_OPENAL)
