#pragma "AudioFile.h"

#if 0

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"


#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include <iostream>

static std::string to_lower(const std::string & s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return r;
}

bool load_audio_file_f32(const std::string & path,
                         std::vector<float> & pcm_out,
                         AudioInfo & info_out)
{
    const std::string p = to_lower(path);

    std::vector<float> pcm;
    uint32_t sample_rate = 0;
    uint32_t channels    = 0;

    // ------------------------------------------------------------
    // WAV
    // ------------------------------------------------------------
    if (p.ends_with(".wav")) {
        drwav wav;
        if (!drwav_init_file(&wav, path.c_str(), nullptr)) {
            std::cerr << "Fehler: WAV konnte nicht geöffnet werden\n";
            return false;
        }

        sample_rate = wav.sampleRate;
        channels    = wav.channels;

        size_t total_frames = wav.totalPCMFrameCount;
        pcm.resize(total_frames * channels);

        size_t frames_read = drwav_read_pcm_frames_f32(&wav, total_frames, pcm.data());
        drwav_uninit(&wav);

        if (frames_read == 0) {
            std::cerr << "Fehler: WAV enthält keine Daten\n";
            return false;
        }
    }

    // ------------------------------------------------------------
    // MP3
    // ------------------------------------------------------------
    else if (p.ends_with(".mp3")) {
        drmp3 mp3;
        if (!drmp3_init_file(&mp3, path.c_str(), nullptr)) {
            std::cerr << "Fehler: MP3 konnte nicht geöffnet werden\n";
            return false;
        }

        sample_rate = mp3.sampleRate;
        channels    = mp3.channels;

        // MP3: wir lesen alles in einen temporären Buffer
        std::vector<float> tmp;
        tmp.resize(10 * 60 * sample_rate * channels); // 10 Minuten max

        size_t frames = drmp3_read_pcm_frames_f32(&mp3,
                                                  tmp.size() / channels,
                                                  tmp.data());
        drmp3_uninit(&mp3);

        if (frames == 0) {
            std::cerr << "Fehler: MP3 enthält keine Daten\n";
            return false;
        }

        tmp.resize(frames * channels);
        pcm = std::move(tmp);
    }

    else {
        std::cerr << "Fehler: Unbekanntes Audioformat: " << path << "\n";
        return false;
    }

    // ------------------------------------------------------------
    // Falls Stereo → Mono mischen
    // ------------------------------------------------------------
    if (channels > 1) {
        std::vector<float> mono;
        mono.resize(pcm.size() / channels);

        for (size_t i = 0; i < mono.size(); i++) {
            float sum = 0.0f;
            for (uint32_t c = 0; c < channels; c++) {
                sum += pcm[i * channels + c];
            }
            mono[i] = sum / channels;
        }

        pcm = std::move(mono);
        channels = 1;
    }

    // ------------------------------------------------------------
    // Resampling auf 16 kHz (Whisper erwartet 16000 Hz)
    // ------------------------------------------------------------
    const uint32_t target_rate = 16000;

    if (sample_rate != target_rate) {
        const double ratio = double(target_rate) / double(sample_rate);
        const size_t new_len = size_t(pcm.size() * ratio);

        std::vector<float> resampled(new_len);

        for (size_t i = 0; i < new_len; i++) {
            double src_index = double(i) / ratio;
            size_t idx = size_t(src_index);

            if (idx + 1 < pcm.size()) {
                double frac = src_index - idx;
                resampled[i] = float((1.0 - frac) * pcm[idx] + frac * pcm[idx + 1]);
            } else {
                resampled[i] = pcm.back();
            }
        }

        pcm = std::move(resampled);
        sample_rate = target_rate;
    }

    // ------------------------------------------------------------
    // Ausgabe
    // ------------------------------------------------------------
    pcm_out = std::move(pcm);

    info_out.sample_rate = sample_rate;
    info_out.channels    = channels;
    info_out.is_f32      = true;
    info_out.interleaved = true;

    return true;
}

#endif
