#include "AudioFileReader_WAV.h"
#include <iostream>

#pragma pack(push, 1)

struct WavRiffHeader {
    char     chunk_id[4];     // "RIFF"
    uint32_t chunk_size;
    char     format[4];       // "WAVE"
};

struct WavFmtChunk {
    char     subchunk1_id[4]; // "fmt "
    uint32_t subchunk1_size;  // 16 for PCM
    uint16_t audio_format;    // 1 = PCM, 3 = float
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

struct WavDataChunk {
    char     subchunk2_id[4]; // "data"
    uint32_t subchunk2_size;
};
#pragma pack(pop)


bool load_wav_f32(AudioFile & ret, const std::string & url )
{
    FILE * f = fopen(url.c_str(), "rb");
    if (!f) {
        std::cerr << "Fehler: WAV konnte nicht geöffnet werden\n";
        return false;
    }

    WavRiffHeader riff;
    if (fread(&riff, sizeof(riff), 1, f) != 1 ||
        strncmp(riff.chunk_id, "RIFF", 4) != 0 ||
        strncmp(riff.format, "WAVE", 4) != 0)
    {
        std::cerr << "Fehler: Keine gültige WAV-Datei\n";
        fclose(f);
        return false;
    }

    WavFmtChunk fmt;
    if (fread(&fmt, sizeof(fmt), 1, f) != 1 ||
        strncmp(fmt.subchunk1_id, "fmt ", 4) != 0)
    {
        std::cerr << "Fehler: WAV fmt-Chunk ungültig\n";
        fclose(f);
        return false;
    }

    // PCM oder Float
    if (fmt.audio_format != 1 && fmt.audio_format != 3)
    {
        std::cerr << "Fehler: WAV-Format nicht unterstützt (nur PCM oder Float)\n";
        fclose(f);
        return false;
    }

    // Suche nach "data"-Chunk
    WavDataChunk data;
    while (true)
    {
        if (fread(&data, sizeof(data), 1, f) != 1) {
            std::cerr << "Fehler: WAV data-Chunk nicht gefunden\n";
            fclose(f);
            return false;
        }
        if (strncmp(data.subchunk2_id, "data", 4) == 0)
            break;

        // Unbekannter Chunk → überspringen
        fseek(f, data.subchunk2_size, SEEK_CUR);
    }

    size_t nFrames = data.subchunk2_size / fmt.block_align;
    size_t nSamples = nFrames * fmt.num_channels;

    ret.channels = fmt.num_channels;
    ret.frame_count = nFrames;
    ret.sample_rate = fmt.sample_rate;
    ret.samples.resize( nSamples * sizeof(float) );
    float* dst = reinterpret_cast<float*>( ret.samples.data() );

    // PCM → float konvertieren
    if (fmt.audio_format == 1)
    {
        // Integer PCM
        if (fmt.bits_per_sample == 16)
        {
            ret.sample_type = AudioFile::ST_S16 | AudioFile::ST_Interleaved;

            // Read file in native format:
            std::vector< int16_t > src(nSamples);
            fread(src.data(), sizeof(int16_t), nSamples, f);

            // Convert to float:
            for (size_t i = 0; i < nSamples; i++)
            {
                dst[i] = src[i] / 32768.0f;
            }
        }
        else if (fmt.bits_per_sample == 24)
        {
            ret.sample_type = AudioFile::ST_S24 | AudioFile::ST_Interleaved;

            // Read file in native format:
            uint8_t b[3];
            for (size_t i = 0; i < nSamples; i++)
            {
                fread(b, 1, 3, f);
                int32_t v = (b[0] | (b[1] << 8) | (b[2] << 16));
                if (v & 0x800000) v |= 0xFF000000;
                dst[i] = float(v) / 8388608.0f;
            }
        }
        else if (fmt.bits_per_sample == 32)
        {
            ret.sample_type = AudioFile::ST_S32 | AudioFile::ST_Interleaved;

            // Read file in native format:
            std::vector< int32_t > src(nSamples);
            fread(src.data(), sizeof(int32_t), nSamples, f);

            // Convert to float:
            for (size_t i = 0; i < nSamples; i++)
            {
                dst[i] = src[i] / 2147483648.0f;
            }
        }
        else
        {
            std::cerr << "Fehler: PCM-Bittiefe nicht unterstützt\n";
            fclose(f);
            return false;
        }
    }
    else if (fmt.audio_format == 3)
    {
        ret.sample_type = AudioFile::ST_F32 | AudioFile::ST_Interleaved;

        // Read native float:
        fread(dst, sizeof(float), nSamples, f);
    }

    fclose(f);

    return true;
}
