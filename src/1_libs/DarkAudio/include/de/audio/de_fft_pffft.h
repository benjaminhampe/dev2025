#pragma once
//#include <de/fft/de_FFT_Windows.h>

/*
┌────────────────────────────────────────────────────────────────────────────┐
│ PFFFT Real FFT Output (N floats)                                           │
├────────────┬────────────┬────────────┬────────────┬────────────┬───────────┤
│ Index      │   [0]      │   [1]      │   [2]      │   [3]      │   [4]     │
│ Content    │   DC       │  Nyquist   │   Re[1]    │   Im[1]    │   Re[2]   │
│ Frequency  │   0 Hz     │   fs/2     │   fs/N     │   fs/N     │  2fs/N    │
├────────────┴────────────┴────────────┴────────────┴────────────┴───────────┤
│   ... continued interleaved Re[k], Im[k] for bins k = 1 to N/2 - 1         │
├────────────┬────────────┬────────────┬────────────┬────────────┬───────────┤
│ Index      │ [N-4]      │ [N-3]      │ [N-2]      │ [N-1]      │           │
│ Content    │ Re[N/2-2]  │ Im[N/2-2]  │ Re[N/2-1]  │ Im[N/2-1]  │           │
│ Frequency  │ (N/2-2)fs/N│ (N/2-2)fs/N│ (N/2-1)fs/N│ (N/2-1)fs/N│           │
└────────────┴────────────┴────────────┴────────────┴────────────┴───────────┘
*/

#include <de/audio/de_aligned_memory.h>

struct DE_FFT_Private_PFFFT;

struct DE_FFT_PFFFT
{
    DE_FFT_Private_PFFFT* _d;

    DE_FFT_PFFFT();
    ~DE_FFT_PFFFT();
    void fft(const float* __restrict__ src, float* __restrict__ dst, size_t n);
};
