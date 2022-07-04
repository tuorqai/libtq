
//------------------------------------------------------------------------------

#include <string.h>

#include "tq_mem.h"
#include "tq_sound_loader.h"

//------------------------------------------------------------------------------
// WAV loader

typedef struct tq_riff_wav_header
{
    char        chunk_id[4];
    uint32_t    chunk_size;
    char        format[4];

    char        fmt_subchunk_id[4];
    uint32_t    fmt_subchunk_size;
    uint16_t    audio_format;
    uint16_t    num_channels;
    uint32_t    sample_rate;
    uint32_t    byte_rate;
    uint16_t    block_align;
    uint16_t    bits_per_sample;

    char        data_subchunk_id[4];
    uint32_t    data_subchunk_size;
} tq_riff_wav_header_t;

static bool is_wav(int32_t stream_id)
{
    tq_istream_seek(stream_id, 0);

    tq_riff_wav_header_t header;

    if (tq_istream_read(stream_id, &header, sizeof(tq_riff_wav_header_t)) == -1) {
        return false;
    }

    bool const valid = (strncmp("RIFF", header.chunk_id, 4) == 0)
        && (strncmp("WAVE", header.format, 4) == 0)
        && (strncmp("fmt ", header.fmt_subchunk_id, 4) == 0)
        && (strncmp("data", header.data_subchunk_id, 4) == 0);
    
    if (!valid) {
        return false;
    }

    return true;
}

static tq_sound_t *load_wav(int32_t stream_id)
{
    tq_istream_seek(stream_id, 0);

    tq_riff_wav_header_t header;

    if (tq_istream_read(stream_id, &header, sizeof(tq_riff_wav_header_t)) == -1) {
        return NULL;
    }

    uint8_t *samples = tq_mem_alloc(header.data_subchunk_size);

    if (tq_istream_read(stream_id, samples, header.data_subchunk_size) == -1) {
        tq_mem_free(samples);
        return NULL;
    }

    tq_sound_t *sound = tq_mem_alloc(sizeof(tq_sound_t));

    sound->num_channels = header.num_channels;
    sound->bytes_per_sample = header.bits_per_sample / 8;
    sound->sample_rate = header.sample_rate;
    sound->num_samples = header.data_subchunk_size / sound->bytes_per_sample;
    sound->samples = samples;

    return sound;
}

//------------------------------------------------------------------------------

tq_sound_t *tq_sound_load(int32_t stream_id)
{
    uint8_t const *buffer = tq_istream_buffer(stream_id);
    size_t size = tq_istream_size(stream_id);

    if (is_wav(stream_id)) {
        return load_wav(stream_id);
    }

    return NULL;
}

void tq_sound_destroy(tq_sound_t *sound)
{
    tq_mem_free(sound->samples);
    tq_mem_free(sound);
}

//------------------------------------------------------------------------------
