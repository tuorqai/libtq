
//------------------------------------------------------------------------------

#include <string.h>
#include <vorbis/vorbisfile.h>

#include "tq_log.h"
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
// OGG loader

static char const *ogg_err(int status)
{
    if (status >= 0) {
        return "(no error)";
    }

    switch (status) {
    case OV_HOLE:           return "OV_HOLE";
    case OV_EREAD:          return "OV_EREAD";
    case OV_EFAULT:         return "OV_EFAULT";
    case OV_EINVAL:         return "OV_EINVAL";
    case OV_ENOTVORBIS:     return "OV_ENOTVORBIS";
    case OV_EBADHEADER:     return "OV_EBADHEADER";
    case OV_EVERSION:       return "OV_EVERSION";
    case OV_EBADLINK:       return "OV_EBADLINK";
    }

    return "(unknown error)";
}

static size_t ogg_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
    int32_t stream_id = *(int32_t *) datasource;
    return tq_istream_read(stream_id, ptr, size * nmemb);
}

static int ogg_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
    int32_t stream_id = *(int32_t *) datasource;

    int64_t position = tq_istream_tell(stream_id);
    int64_t size = tq_istream_size(stream_id);

    int64_t next_position = -1;

    switch (whence) {
    case SEEK_SET:
        next_position = offset;
        break;
    case SEEK_CUR:
        next_position = position + offset;
        break;
    case SEEK_END:
        next_position = size - offset;
        break;
    }

    return tq_istream_seek(stream_id, next_position);
}

static int ogg_close_func(void *datasource)
{
    int32_t stream_id = *(int32_t *) datasource;
    return tq_istream_close(stream_id);
}

static long ogg_tell_func(void *datasource)
{
    int32_t stream_id = *(int32_t *) datasource;
    return tq_istream_tell(stream_id);
}

static bool is_ogg(int32_t *stream_id, OggVorbis_File *vorbis)
{
    tq_istream_seek(*stream_id, 0);

    ov_callbacks callbacks = {
        .read_func = ogg_read_func,
        .seek_func = ogg_seek_func,
        .close_func = NULL,
        .tell_func = ogg_tell_func,
    };

    return (ov_test_callbacks(stream_id, vorbis, NULL, 0, callbacks) >= 0);
}

static tq_sound_t *load_ogg(OggVorbis_File *vorbis)
{
    int status = ov_test_open(vorbis);

    if (status < 0) {
        tq_log_error("Failed to open Ogg Vorbis media: %s\n", ogg_err(status));
        return NULL;
    }

    vorbis_info *info       = ov_info(vorbis, -1);
    tq_sound_t *sound       = tq_mem_alloc(sizeof(tq_sound_t));
    sound->num_channels     = info->channels;
    sound->bytes_per_sample = 2;
    sound->sample_rate      = info->rate;
    sound->num_samples      = ov_pcm_total(vorbis, -1) * info->channels;
    sound->samples          = tq_mem_alloc(sound->bytes_per_sample * sound->num_samples);

    int buffer_length       = 4096;
    int big_endian_packing  = 0; // 0 for little-endian, 1 for big-endian
    int word_size           = 2; // 1 for 8-bit samples, 2 for 16-bit
    int signed_data         = 1; // 0 for unsigned, 1 for signed
    int current_stream      = 0;
    size_t offset           = 0;
    long bytes              = 0;

    while (true) {
        bytes = ov_read(vorbis,
            (char *) &sound->samples[offset],
            buffer_length,
            big_endian_packing,
            word_size,
            signed_data,
            &current_stream);

        // Reached end of file
        if (bytes == 0) {
            break;
        }

        // Error occured, need to stop
        if (bytes < 0) {
            break;
        }

        offset += bytes;
    }

    ov_clear(vorbis);

    if (bytes < 0) {
        tq_mem_free(sound->samples);
        tq_mem_free(sound);

        tq_log_error("Failed to decode Ogg Vorbis sound: %s\n", ogg_err(status));

        return NULL;
    }

    return sound;
}

//------------------------------------------------------------------------------

tq_sound_t *tq_sound_load(int32_t stream_id)
{
    //
    // (1) RIFF WAV

    if (is_wav(stream_id)) {
        return load_wav(stream_id);
    }

    //
    // (2) Ogg Vorbis

    {
        OggVorbis_File vorbis;

        if (is_ogg(&stream_id, &vorbis)) {
            return load_ogg(&vorbis);
        }
    }

    return NULL;
}

void tq_sound_destroy(tq_sound_t *sound)
{
    tq_mem_free(sound->samples);
    tq_mem_free(sound);
}

//------------------------------------------------------------------------------
