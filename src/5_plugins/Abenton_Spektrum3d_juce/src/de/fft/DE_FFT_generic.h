#pragma once
#include <de/MathMusic.h>

namespace de {
namespace audio {
namespace fft {

struct DE_FFT_generic
{
    AlignedFloatVector m_imag;
    AlignedFloatVector m_real;
    AlignedFloatVector m_packed;

    void resize(size_t n);
    void fft(const AlignedFloatVector& input, AlignedFloatVector& output);
    void ifft(const std::vector<float>& spectrum, std::vector<float>& output);
};

} // end namespace fft.
} // end namespace audio.
} // end namespace de.

