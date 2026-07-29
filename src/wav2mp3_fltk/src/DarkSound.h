#pragma once
#include <de/sound/Sound.h>

bool dbLoadSound( de::Sound & sound,
                  const std::string& uri,
                  const de::SoundLoadOptions& options = {});

bool dbSaveSound( const de::Sound & sound,
                  const std::string& uri,
                  const de::SoundSaveOptions& options = {});

/*
+----------------------+------------+---------------------------+----------------------+-------------------+-------------------------------+-------------------------------+
| Resampler            | License    | SIMD / CPU Model          | Quality (Music)      | Latency / CPU     | Architecture Cleanliness      | Best Use Case                 |
+----------------------+------------+---------------------------+----------------------+-------------------+-------------------------------+-------------------------------+
| Soxr                 | LGPL-2.1   | SSE/AVX (macro-based)     | ***** (excellent)    | Medium-High       | Bad (macro hell, dead FFT)    | Offline HQ, mastering         |
| SpeexDSP             | BSD        | SSE2 + fixed-point        | ** (speech OK)       | Low               | Clean, simple                 | Real-time, VoIP, drift fix    |
| libsamplerate        | GPL-2      | Mostly scalar             | ***** (excellent)    | High              | Clean API, heavy internals    | Offline HQ                    |
| Signalsmith          | MIT        | AVX2 (modern SIMD)        | ***** (excellent)    | Medium            | Very clean, header-only       | Real-time plugins, DAWs       |
| r8brain-free         | BSD        | SSE2/AVX                  | ***** (excellent)    | Medium            | Clean C++                     | HQ offline + real-time        |
| FFmpeg swresample    | LGPL/GPL   | Mixed                     | **** (very good)     | Medium            | Integrated, not standalone    | Media pipelines               |
| SoundTouch           | LGPL       | Scalar/SSE                | ** (low)             | Low               | Clean but old                 | Time-stretch/pitch-shift      |
| RubberBand           | GPL        | SSE/AVX                   | **** (very good)     | Medium            | Clean                         | Time-stretch/pitch-shift      |
| libresample-1.8      | LGPL-2.1   | Scalar FIR (no SIMD)      | * (poor)             | Very Low          | Clean but ancient             | Embedded, fixed-ratio         |
+----------------------+------------+---------------------------+----------------------+-------------------+-------------------------------+-------------------------------+
*/

bool dbResampleSound( const de::Sound & src,
                      de::Sound & dst,
                      int32_t sampleRate,
                      int32_t quality = 0);

int64_t dbCopySound( const de::Sound& src,
                     de::Sound& dst,
                     int64_t srcFrameCount,
                     int64_t srcFrameStart = 0);

//🔥Deinterleave (interleaved → planar)

int64_t dbDeinterleaveSound( const de::Sound & src, de::Sound & dst );

//🔥Interleave (planar → interleaved)

int64_t dbInterleaveSound( const de::Sound & src, de::Sound & dst );

int64_t dbConvertSound( const de::Sound & src,
                        de::Sound & dst,
                        de::SampleType dstType );

#if 0
inline void testDarkSound
{
    // =============================================================
    // Test 1:
    // =============================================================
    de::Sound snd1;
    dbLoadSound(snd1,"C:/_media/Music/wav/piano-space.wav");
    dbSaveSound(snd1,"C:/_media/Music/wav/piano-space_test1.mp3");

    auto saveBase = dbStr("C:/_media/Music/test/piano-space_",snd1.m_sampleRate);
    dbSaveSound(snd1,saveBase + ".mp3");
    dbSaveSound(snd1,saveBase + ".wav");
    dbSaveSound(snd1,saveBase + ".flac");
    dbSaveSound(snd1,saveBase + ".vorbis.ogg");
    dbSaveSound(snd1,saveBase + ".opus");

    // =============================================================
    // Test 2:
    // =============================================================
    de::Sound snd2;
    dbCopySound(snd1,snd2,snd1.m_frames);
    dbSaveSound(snd2,"C:/_media/Music/wav/piano-space_test2_copy.mp3");

    // =============================================================
    // Test 3:
    // =============================================================
    de::Sound snd3;
    de::Sound snd4;
    dbDeinterleaveSound(snd2,snd3);
    dbSaveSound(snd3,"C:/_media/Music/wav/piano-space_test3_1.planar.mp3");
    dbInterleaveSound(snd3,snd4);
    dbSaveSound(snd4,"C:/_media/Music/wav/piano-space_test3_2.interleaved.mp3");

    // =============================================================
    // Test 4:
    // =============================================================
    de::Sound snd5;
    de::Sound snd6;
    dbConvertSound(snd1,snd5,de::SampleType::F32);
    dbSaveSound(snd5,"C:/_media/Music/wav/piano-space_test4_convertF32.mp3");
    dbConvertSound(snd5,snd6,de::SampleType::F64);
    dbSaveSound(snd6,"C:/_media/Music/wav/piano-space_test4_convertF64.mp3");

    // =============================================================
    // Test 5:
    // =============================================================
    de::Sound snd7;
    dbResampleSound(snd5,snd7,48000);
    dbSaveSound(snd7,"C:/_media/Music/wav/piano-space_test5_resample_48Hz_r8brain.mp3");
}
#endif
