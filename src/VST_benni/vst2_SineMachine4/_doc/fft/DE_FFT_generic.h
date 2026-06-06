#pragma once
#include <de/de_aligned_memory.h>
#include <vector>
//#include <cmath>

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

