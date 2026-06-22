#pragma once
#include <de/AlignedMemory.h>
#include <de/MathMusic.h>

namespace de {
namespace math {
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
} // end namespace math.
} // end namespace de.

