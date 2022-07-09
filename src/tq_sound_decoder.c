
//------------------------------------------------------------------------------

#include <string.h>

#if defined(TQ_USE_OGG)
#   include <vorbis/vorbisfile.h>
#endif

#include "tq_log.h"
#include "tq_mem.h"
#include "tq_sound_decoder.h"

//------------------------------------------------------------------------------

typedef union tq_sound_decoder_info
{
    struct {
        uint16_t    bytes_per_sample;
        int64_t     data_start;
        int64_t     data_end;
    } wav;

#if defined(TQ_USE_OGG)
    OggVorbis_File vorbis;
#endif
} tq_sound_decoder_info_t;

typedef void (*tq_sound_decoder_close_func)(int32_t id);
typedef void (*tq_sound_decoder_seek_func)(int32_t id, uint64_t sample_offset);
typedef uint64_t (*tq_sound_decoder_read_func)(int32_t id, int16_t *samples, uint64_t max_samples);

typedef struct tq_sound_decoders
{
    uint8_t     usage[TQ_SOUND_DECODER_LIMIT / 8];

    int32_t     stream_id[TQ_SOUND_DECODER_LIMIT];
    uint16_t    num_channels[TQ_SOUND_DECODER_LIMIT];
    uint32_t    num_samples[TQ_SOUND_DECODER_LIMIT];
    uint32_t    sample_rate[TQ_SOUND_DECODER_LIMIT];

    tq_sound_decoder_info_t         info[TQ_SOUND_DECODER_LIMIT];
    tq_sound_decoder_close_func     close[TQ_SOUND_DECODER_LIMIT];
    tq_sound_decoder_seek_func      seek[TQ_SOUND_DECODER_LIMIT];
    tq_sound_decoder_read_func      read[TQ_SOUND_DECODER_LIMIT];
} tq_sound_decoders_t;

//------------------------------------------------------------------------------

static tq_sound_decoders_t decoders;

//------------------------------------------------------------------------------

static void mark_id(int32_t id)
{
    uint8_t index = id / 8;
    uint8_t mask = (1 << (id % 8));

    decoders.usage[index] |= mask;
}

static void unmark_id(int32_t id)
{
    uint8_t index = id / 8;
    uint8_t mask = (1 << (id % 8));

    decoders.usage[index] &= ~mask;
}

static int32_t get_sound_decoder_id(void)
{
    for (int32_t id = 0; id < TQ_INPUT_STREAM_LIMIT; id++) {
        uint8_t u = id / 8;
        uint8_t v = (1 << (id % 8));

        if ((decoders.usage[u] & v) == 0) {
            return id;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
// RIFF WAV

/**
 * Try to open a WAV file.
 */
static int open_wav(int32_t decoder_id)
{
    int32_t stream_id = decoders.stream_id[decoder_id];
    tq_istream_seek(stream_id, 0);

    struct {
        char id[4];
        uint32_t size;
        char format[4];
    } chunk;

    if (tq_istream_read(stream_id, &chunk, sizeof(chunk)) < sizeof(chunk)) {
        return -1;
    }

    if (strncmp("RIFF", chunk.id, 4) || strncmp("WAVE", chunk.format, 4)) {
        return -1;
    }

    bool data_found = false;

    while (!data_found) {
        struct {
            char id[4];
            uint32_t size;
        } subchunk;

        if (tq_istream_read(stream_id, &subchunk, sizeof(subchunk)) < sizeof(subchunk)) {
            return -1;
        }

        int64_t subchunk_start = tq_istream_tell(stream_id);

        /**
         * 'fmt ' subchunk
         */

        if (strncmp("fmt ", subchunk.id, 4) == 0) {
            struct {
                uint16_t audio_format;
                uint16_t num_channels;
                uint32_t sample_rate;
                uint32_t byte_rate;
                uint16_t block_align;
                uint16_t bits_per_sample;
            } fmt;

            if (tq_istream_read(stream_id, &fmt, sizeof(fmt)) < sizeof(fmt)) {
                return -1;
            }

            decoders.info[decoder_id].wav.bytes_per_sample = fmt.bits_per_sample / 8;
            decoders.num_channels[decoder_id] = fmt.num_channels;
            decoders.sample_rate[decoder_id] = fmt.sample_rate;
        }

        /**
         * 'data' subchunk
         */

        if (strncmp("data", subchunk.id, 4) == 0) {
            decoders.num_samples[decoder_id] = subchunk.size / decoders.info[decoder_id].wav.bytes_per_sample;

            decoders.info[decoder_id].wav.data_start = tq_istream_tell(stream_id);
            decoders.info[decoder_id].wav.data_end = decoders.info[decoder_id].wav.data_start + subchunk.size;

            data_found = true;
        }

        /**
         * Force skip to next subchunk
         */

        if (tq_istream_seek(stream_id, subchunk_start + subchunk.size) == -1) {
            return -1;
        }
    }

    tq_istream_seek(stream_id, decoders.info[decoder_id].wav.data_start);

    return 0;
}

/**
 * close() function for RIFF WAV decoder.
 */
static void close_wav(int32_t decoder_id)
{
}

/**
 * seek() function for RIFF WAV decoder.
 */
static void seek_wav(int32_t decoder_id, uint64_t sample_offset)
{
    int32_t stream_id = decoders.stream_id[decoder_id];
    int64_t data_start = decoders.info[decoder_id].wav.data_start;
    int64_t bytes_per_sample = decoders.info[decoder_id].wav.bytes_per_sample;

    tq_istream_seek(stream_id, data_start + (sample_offset * bytes_per_sample));
}

/**
 * read() function for RIFF WAV decoder.
 */
static uint64_t read_wav(int32_t decoder_id, int16_t *samples, uint64_t max_samples)
{
    int32_t stream_id = decoders.stream_id[decoder_id];
    uint16_t bytes_per_sample = decoders.info[decoder_id].wav.bytes_per_sample;

    int samples_read = 0;

    while (samples_read < max_samples) {
        int64_t position = tq_istream_tell(stream_id);

        if (position >= decoders.info[decoder_id].wav.data_end) {
            break;
        }

        uint8_t bytes[4];

        if (tq_istream_read(stream_id, bytes, bytes_per_sample) != bytes_per_sample) {
            break;
        }

        switch (bytes_per_sample) {
        case 1:
            /* Unsigned 8-bit PCM */
            *samples++ = (((int16_t) bytes[0]) - 128) << 8;
            break;
        case 2:
            /* Signed 16-bit PCM */
            *samples++ = (bytes[1] << 8) | bytes[0];
            break;
        case 3:
            /* Signed 24-bit PCM */
            *samples++ = (bytes[2] << 8) | bytes[1];
            break;
        case 4:
            /* Signed 32-bit PCM */
            *samples++ = (bytes[3] << 8) | bytes[2];
            break;
        }

        samples_read++;
    }

    return samples_read;
}

//------------------------------------------------------------------------------
// Ogg Vorbis

#if defined(TQ_USE_OGG)

/**
 * Convert libogg error code to printable string.
 */
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

/**
 * read() callback for libvorbisfile.
 */
static size_t vorbis_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
    int32_t stream_id = *(int32_t *) datasource;

    return tq_istream_read(stream_id, ptr, size * nmemb);
}

/**
 * seek() callback for libvorbisfile.
 */
static int vorbis_seek_func(void *datasource, ogg_int64_t offset, int whence)
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

/**
 * tell() callback for libvorbisfile.
 */
static long vorbis_tell_func(void *datasource)
{
    int32_t stream_id = *(int32_t *) datasource;

    return tq_istream_tell(stream_id);
}

/**
 * Try to open Ogg Vorbis file.
 */
static int open_ogg(int32_t decoder_id)
{
    tq_istream_seek(decoders.stream_id[decoder_id], 0);

    int test = ov_test_callbacks(
        &decoders.stream_id[decoder_id],
        &decoders.info[decoder_id].vorbis,
        NULL, 0,
        (ov_callbacks) {
            .read_func = vorbis_read_func,
            .seek_func = vorbis_seek_func,
            .close_func = NULL,
            .tell_func = vorbis_tell_func,
        }
    );

    if (test < 0) {
        return -1;
    }

    int status = ov_test_open(&decoders.info[decoder_id].vorbis);

    if (status < 0) {
        tq_log_error("Failed to open Ogg Vorbis media: %s\n", ogg_err(status));
        return -1;
    }

    vorbis_info *info = ov_info(&decoders.info[decoder_id].vorbis, -1);
    ogg_int64_t samples_per_channel = ov_pcm_total(&decoders.info[decoder_id].vorbis, -1);

    decoders.num_channels[decoder_id] = info->channels;
    decoders.num_samples[decoder_id] = samples_per_channel * info->channels;
    decoders.sample_rate[decoder_id] = info->rate;

    return 0;
}

/**
 * close() function for Ogg Vorbis decoder.
 */
static void close_ogg(int32_t decoder_id)
{
    ov_clear(&decoders.info[decoder_id].vorbis);
}

/**
 * seek() function for Ogg Vorbis decoder.
 */
static void seek_ogg(int32_t decoder_id, uint64_t sample_offset)
{
    ov_pcm_seek(&decoders.info[decoder_id].vorbis, sample_offset / decoders.num_channels[decoder_id]);
}

/**
 * read() function for Ogg Vorbis decoder.
 */
static uint64_t read_ogg(int32_t decoder_id, int16_t *samples, uint64_t max_samples)
{
    uint64_t samples_read = 0;

    while (samples_read < max_samples) {
        int bytes_left = (max_samples - samples_read) / sizeof(int16_t);
        long bytes_read = ov_read(&decoders.info[decoder_id].vorbis, (char *) samples, bytes_left,
            0, 2, 1, NULL);

        // End of file.
        if (bytes_read == 0) {
            break;
        }

        // Some error occured.
        if (bytes_read < 0) {
            break;
        }

        samples_read += bytes_read / sizeof(int16_t);
        samples += bytes_read / sizeof(int16_t);
    }

    return samples_read;
}

#endif // defined(TQ_USE_OGG)

//------------------------------------------------------------------------------

int32_t tq_sound_decoder_open(int32_t stream_id)
{
    int32_t decoder_id = get_sound_decoder_id();

    if (decoder_id == -1) {
        return -1;
    }

    decoders.stream_id[decoder_id] = stream_id;
    decoders.num_channels[decoder_id] = 0;
    decoders.num_samples[decoder_id] = 0;
    decoders.sample_rate[decoder_id] = 0;

    if (open_wav(decoder_id) == 0) {
        decoders.close[decoder_id] = close_wav;
        decoders.seek[decoder_id] = seek_wav;
        decoders.read[decoder_id] = read_wav;

        mark_id(decoder_id);
        return decoder_id;
    }
    
#if defined(TQ_USE_OGG)
    if (open_ogg(decoder_id) == 0) {
        decoders.close[decoder_id] = close_ogg;
        decoders.seek[decoder_id] = seek_ogg;
        decoders.read[decoder_id] = read_ogg;

        mark_id(decoder_id);
        return decoder_id;
    }
#endif

    return -1;
}

void tq_sound_decoder_close(int32_t decoder_id)
{
    unmark_id(decoder_id);
    decoders.close[decoder_id](decoder_id);
}

uint16_t tq_sound_decoder_get_num_channels(int32_t decoder_id)
{
    return decoders.num_channels[decoder_id];
}

uint32_t tq_sound_decoder_get_num_samples(int32_t decoder_id)
{
    return decoders.num_samples[decoder_id];
}

uint32_t tq_sound_decoder_get_sample_rate(int32_t decoder_id)
{
    return decoders.sample_rate[decoder_id];
}

void tq_sound_decoder_seek(int32_t decoder_id, uint64_t sample_offset)
{
    decoders.seek[decoder_id](decoder_id, sample_offset);
}

uint64_t tq_sound_decoder_read(int32_t decoder_id, int16_t *samples, uint64_t max_samples)
{
    return decoders.read[decoder_id](decoder_id, samples, max_samples);
}

//------------------------------------------------------------------------------
