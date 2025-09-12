#pragma once
#include <kissfft/kiss_fft.hpp>
#include <cstdint>
#include <complex>
#include <vector>

namespace de {

struct KissFFT
{
    typedef float T;
    typedef std::complex< T > TComplex;
    typedef std::vector< TComplex > TComplexVector;

    uint32_t m_fftSize;
    bool m_inverse;
    kissfft< T > m_fft;
    TComplexVector m_fftInput;
    TComplexVector m_fftOutput;

    KissFFT( uint32_t fftSize, bool bInverse );

    // void
    // setFFTSize( uint32_t fftSize );

    uint32_t
    size() const;

    void
    fft();

    // Input is one channel of float audio data.
    void
    setInput( float const* src, uint32_t srcCount );

    void
    setInput( TComplex const* src, uint32_t srcCount );

    TComplex const*
    getOutput() const;

    TComplex*
    getOutput();

    void
    getOutput( TComplex* dst, uint32_t dstCount ) const;

    void
    getOutputAbs( float* dst, uint32_t dstCount ) const;

    void
    getOutputInDecibel( float* dst, uint32_t dstCount );
};

} // end namespace de
