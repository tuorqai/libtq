
//------------------------------------------------------------------------------

#include <string.h>

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

static bool is_wav(stream_t const *stream)
{
    stream->seek(stream->data, 0);

    tq_riff_wav_header_t header;

    if (stream->read(stream->data, &header, sizeof(tq_riff_wav_header_t)) == -1) {
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

static int32_t load_wav(tq_sound_t *sound, stream_t const *stream)
{
    stream->seek(stream->data, 0);

    tq_riff_wav_header_t header;

    if (stream->read(stream->data, &header, sizeof(tq_riff_wav_header_t)) == -1) {
        return -1;
    }

    uint8_t *samples = malloc(header.data_subchunk_size);

    if (!samples) {
        return -1;
    }

    if (stream->read(stream->data, samples, header.data_subchunk_size) == -1) {
        free(samples);
        return -1;
    }

    *sound = (tq_sound_t) {
        .num_channels = header.num_channels,
        .bytes_per_sample = header.bits_per_sample / 8,
        .sample_rate = header.sample_rate,
        .num_samples = header.data_subchunk_size / (header.bits_per_sample / 8),
        .samples = samples,
    };

    return 0;
}

//------------------------------------------------------------------------------

int32_t tq_sound_load(tq_sound_t *sound, stream_t const *stream)
{
    uint8_t const *buffer = stream->buffer(stream->data);
    size_t size = stream->get_size(stream->data);

    if (is_wav(stream)) {
        return load_wav(sound, stream);
    }

    return -1;
}

void tq_sound_free(tq_sound_t *sound)
{
    free(sound->samples);
}

//------------------------------------------------------------------------------
