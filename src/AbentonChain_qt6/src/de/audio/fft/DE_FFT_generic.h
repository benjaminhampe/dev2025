#pragma once
#include <de/de_aligned_memory.h>
#include <vector>
//#include <cmath>

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

namespace de {
namespace audio {
namespace fft {
	
struct DE_FFT_generic
{
    DE_AlignedFloatVector m_imag;
    DE_AlignedFloatVector m_real;
    DE_AlignedFloatVector m_packed;

    void resize(size_t n);
    void fft(const DE_AlignedFloatVector& input, DE_AlignedFloatVector& output);
    void ifft(const std::vector<float>& spectrum, std::vector<float>& output);
};

} // end namespace fft.
} // end namespace audio.
} // end namespace de.

