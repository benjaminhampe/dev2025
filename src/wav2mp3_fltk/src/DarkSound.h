#pragma once
#include <de/sound/Sound.h>

bool dbLoadSound( de::Sound & sound, const std::string& uri );




bool
dbSaveSound(
    const de::Sound & sound,
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


bool dbResampleSound(
        const de::Sound & src,
        de::Sound & dst,
        int32_t sampleRate,
        int32_t quality = 0);


int64_t dbCopySound(
        const de::Sound& src,
        de::Sound& dst,
        int64_t srcFrameCount,
        int64_t srcFrameStart = 0);

//🔥Deinterleave (interleaved → planar)

int64_t dbDeinterleaveSound(
        const de::Sound & src,
        de::Sound & dst);

//🔥Interleave (planar → interleaved)

int64_t dbInterleaveSound(
        const de::Sound & src,
        de::Sound & dst);

int64_t dbConvertSound(
        const de::Sound & src,
        de::Sound & dst,
        de::SampleType dstType);
