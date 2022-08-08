
//------------------------------------------------------------------------------

#include <string.h>

#if defined(TQ_USE_OGG)
#   include <vorbis/vorbisfile.h>
#endif

#include "tq_error.h"
#include "tq_log.h"
#include "tq_mem.h"
#include "tq_audio_dec.h"

//------------------------------------------------------------------------------

struct libtq_audio_dec
{
    long            (*read)(libtq_audio_dec *dec, short *dst, long max_samples);
    void            (*seek)(libtq_audio_dec *dec, long sample_offset);
    void            (*close)(libtq_audio_dec *dec);

    libtq_stream    *stream;
    int             num_channels;
    long            num_samples;
    long            sample_rate;

    struct {
        short       bytes_per_sample;
        long        data_start;
        long        data_end;
    } wav;

#if defined(TQ_USE_OGG)
    OggVorbis_File  vorbis;
#endif
};

//------------------------------------------------------------------------------
// RIFF WAV

/**
 * Try to open a WAV file.
 */
static int open_wav(libtq_audio_dec *dec)
{
    libtq_stream_seek(dec->stream, 0);

    struct {
        char id[4];
        uint32_t size;
        char format[4];
    } chunk;

    if (libtq_stream_read(dec->stream, &chunk, sizeof(chunk)) < sizeof(chunk)) {
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

        if (libtq_stream_read(dec->stream, &subchunk, sizeof(subchunk)) < sizeof(subchunk)) {
            return -1;
        }

        long subchunk_start = libtq_stream_tell(dec->stream);

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

            if (libtq_stream_read(dec->stream, &fmt, sizeof(fmt)) < sizeof(fmt)) {
                return -1;
            }

            dec->wav.bytes_per_sample = fmt.bits_per_sample / 8;
            dec->num_channels = fmt.num_channels;
            dec->sample_rate = fmt.sample_rate;
        }

        /**
         * 'data' subchunk
         */

        if (strncmp("data", subchunk.id, 4) == 0) {
            dec->num_samples = subchunk.size / dec->wav.bytes_per_sample;

            dec->wav.data_start = libtq_stream_tell(dec->stream);
            dec->wav.data_end = dec->wav.data_start + subchunk.size;

            data_found = true;
        }

        /**
         * Force skip to next subchunk
         */

        if (libtq_stream_seek(dec->stream, subchunk_start + subchunk.size) == -1) {
            return -1;
        }
    }

    libtq_stream_seek(dec->stream, dec->wav.data_start);

    return 0;
}

/**
 * read() function for RIFF WAV decoder.
 */
static long read_wav(libtq_audio_dec *dec, short *samples, long max_samples)
{
    short bytes_per_sample = dec->wav.bytes_per_sample;
    long samples_read = 0;

    while (samples_read < max_samples) {
        long position = libtq_stream_tell(dec->stream);

        if (position >= dec->wav.data_end) {
            break;
        }

        unsigned char bytes[4];

        if (libtq_stream_read(dec->stream, bytes, bytes_per_sample) != bytes_per_sample) {
            break;
        }

        switch (bytes_per_sample) {
        case 1:
            /* Unsigned 8-bit PCM */
            *samples++ = (((short) bytes[0]) - 128) << 8;
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

/**
 * seek() function for RIFF WAV decoder.
 */
static void seek_wav(libtq_audio_dec *dec, long sample_offset)
{
    long data_start = dec->wav.data_start;
    long bytes_per_sample = dec->wav.bytes_per_sample;

    libtq_stream_seek(dec->stream, data_start + (sample_offset * bytes_per_sample));
}

/**
 * close() function for RIFF WAV decoder.
 */
static void close_wav(libtq_audio_dec *dec)
{
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
    libtq_stream *stream = (libtq_stream *) datasource;

    return libtq_stream_read(stream, ptr, size * nmemb);
}

/**
 * seek() callback for libvorbisfile.
 */
static int vorbis_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
    libtq_stream *stream = (libtq_stream *) datasource;

    long position = libtq_stream_tell(stream);
    long size = libtq_stream_size(stream);

    long next_position = -1;

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

    return libtq_stream_seek(stream, next_position);
}

/**
 * tell() callback for libvorbisfile.
 */
static long vorbis_tell_func(void *datasource)
{
    libtq_stream *stream = (libtq_stream *) datasource;

    return libtq_stream_tell(stream);
}

/**
 * Try to open Ogg Vorbis file.
 */
static int open_ogg(libtq_audio_dec *dec)
{
    libtq_stream_seek(dec->stream, 0);

    int test = ov_test_callbacks(
        dec->stream,
        &dec->vorbis,
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

    int status = ov_test_open(&dec->vorbis);

    if (status < 0) {
        libtq_log(LIBTQ_LOG_ERROR, "Failed to open Ogg Vorbis media: %s\n", ogg_err(status));
        return -1;
    }

    vorbis_info *info = ov_info(&dec->vorbis, -1);
    ogg_int64_t samples_per_channel = ov_pcm_total(&dec->vorbis, -1);

    dec->num_channels = info->channels;
    dec->num_samples = samples_per_channel * info->channels;
    dec->sample_rate = info->rate;

    return 0;
}

/**
 * read() function for Ogg Vorbis decoder.
 */
static long read_ogg(libtq_audio_dec *dec, short *samples, long max_samples)
{
    long samples_read = 0;

    while (samples_read < max_samples) {
        int bytes_left = (max_samples - samples_read) / sizeof(int16_t);
        long bytes_read = ov_read(&dec->vorbis, (char *) samples, bytes_left, 0, 2, 1, NULL);

        // End of file.
        if (bytes_read == 0) {
            break;
        }

        // Some error occured.
        if (bytes_read < 0) {
            break;
        }

        samples_read += bytes_read / sizeof(short);
        samples += bytes_read / sizeof(short);
    }

    return samples_read;
}

/**
 * seek() function for Ogg Vorbis decoder.
 */
static void seek_ogg(libtq_audio_dec *dec, long sample_offset)
{
    ov_pcm_seek(&dec->vorbis, sample_offset / dec->num_channels);
}

/**
 * close() function for Ogg Vorbis decoder.
 */
static void close_ogg(libtq_audio_dec *dec)
{
    ov_clear(&dec->vorbis);
}

#endif // defined(TQ_USE_OGG)

//------------------------------------------------------------------------------

libtq_audio_dec *libtq_open_audio_dec(libtq_stream *stream)
{
    libtq_audio_dec *dec = libtq_calloc(1, sizeof(libtq_audio_dec));

    if (!dec) {
        libtq_out_of_memory();
    }

    dec->stream = stream;

    if (open_wav(dec) == 0) {
        dec->read = read_wav;
        dec->seek = seek_wav;
        dec->close = close_wav;

        return dec;
    }
    
    #if defined(TQ_USE_OGG)
        if (open_ogg(dec) == 0) {
            dec->read = read_ogg;
            dec->seek = seek_ogg;
            dec->close = close_ogg;

            return dec;
        }
    #endif

    return NULL;
}

int libtq_audio_dec_get_num_channels(libtq_audio_dec *dec)
{
    return dec->num_channels;
}

long libtq_audio_dec_get_num_samples(libtq_audio_dec *dec)
{
    return dec->num_samples;
}

long libtq_audio_dec_get_sample_rate(libtq_audio_dec *dec)
{
    return dec->sample_rate;
}

long libtq_audio_dec_read(libtq_audio_dec *dec, short *samples, long max_samples)
{
    return dec->read(dec, samples, max_samples);
}

void libtq_audio_dec_seek(libtq_audio_dec *dec, long sample_offset)
{
    dec->seek(dec, sample_offset);
}

void libtq_audio_dec_close(libtq_audio_dec *dec)
{
    dec->close(dec);
}

//------------------------------------------------------------------------------
