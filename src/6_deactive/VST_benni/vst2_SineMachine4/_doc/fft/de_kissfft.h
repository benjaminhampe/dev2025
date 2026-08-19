#pragma once
#include <de/de_aligned_memory.h>
#include <complex>
#include <kissfft/kiss_fft.hpp>

namespace de {

template <typename T>
struct KissFFT
{
    // typedef float T;
    uint32_t m_fftSize;
    bool m_inverse;
    kissfft< T > m_fft;

    // We store input and output!
    typedef std::complex<T> TComplex;
    TAlignedVector<TComplex> m_complexInput;
    TAlignedVector<TComplex> m_complexOutput;

    KissFFT()
        : m_fftSize( 1024 * 8 )
        , m_inverse( false )
        , m_fft( m_fftSize, m_inverse )
    {
        m_complexInput.resize( m_fftSize );
        m_complexOutput.resize( m_fftSize );
    }

    KissFFT( uint32_t fftSize, bool bInverse )
      : m_fftSize( fftSize )
      , m_inverse( bInverse )
      , m_fft( fftSize, bInverse )
    {
        m_complexInput.resize( m_fftSize );
        m_complexOutput.resize( m_fftSize );
    }

    // void
    // setFFTSize( uint32_t fftSize );

    void
    resize( uint32_t fftSize )
    {
        if (m_fftSize != fftSize)
        {
            // DE_OK("m_fftSize(",m_fftSize,") != fftSize(",fftSize,")")
            m_fftSize = fftSize;
            m_fft = kissfft< T >( m_fftSize, m_inverse );
            m_complexInput.resize( m_fftSize );
            m_complexOutput.resize( m_fftSize );
        }
    }

    uint32_t
    size() const { return m_fftSize; }

    /*
    TComplex*
    getOutput() { return m_complexOutput.data(); }

    TComplex const*
    getOutput() const { return m_complexOutput.data(); }
    */

    void
    fft_dB( const float* __restrict__ pSrc, float* __restrict__ pDst, uint32_t fftSize )
    {
        resize( fftSize );

        for ( size_t i = 0; i < fftSize; ++i )
        {
            m_complexInput[ i ] = TComplex( T( *pSrc++ ), T(0) );
        }

        m_fft.transform( m_complexInput.data(), m_complexOutput.data() );

        auto n = fftSize / 2;

        // T y_min = std::numeric_limits<T>::max();
        // T y_max = std::numeric_limits<T>::lowest();

        for ( size_t i = 0; i < n; i++ )
        {
            TComplex const & z = m_complexOutput[ i ];
            T a = z.real();	// Lesezugriff auf komplexe Zahl
            T b = z.imag();	// Lesezugriff auf komplexe Zahl
            T c = (a*a) + (b*b);	// Betrag der komplexen Zahl z

            // y_min = std::min( y_min, c );
            // y_max = std::min( y_max, c );

            //*pDst++ = c;

            T dB = 0; // T(320);       // Umwandlung in Dezibels, min = -16 * 20 dB
            if ( c > T(1e-16) )
            {
                //if ( sizeof( T ) < 8 )
                //{
                dB = log10f( c ); // Umwandlung in Dezibels
                //}
                //else
                //{
                //    dB = -log10( c ); // Umwandlung in Dezibels
                //}
                // Umwandlung in Dezibels ( incl. 1/2 for the removed sqrt(a^2+b^2) )
            }
            //dB *= T(1)/T(320); T(10) *
            //if ( dB < T(-20) ) dB = T(-320); // Benni filter
            //if ( dB > T(0) ) dB *= T(2); // Benni enhancer

            *pDst++ = dB;
        }

        // dst = pDst;
        // T y_scale = T(1) / (y_max - y_min);
        // for ( size_t i = 0; i < fftSize; i++ )
        // {
        //     T x = *dst;
        //     *dst++ = (x * y_scale) + y_min;
        // }
    }


    void
    getOutputIn( float* __restrict__ dst, uint32_t dstCount )
    {
        auto n = std::min( dstCount, uint32_t( m_complexOutput.size() ) / 2 );

        for ( size_t i = 0; i < n; i++ )
        {
            TComplex const & z = m_complexOutput[ i ];
            T a = z.real();	// Lesezugriff auf komplexe Zahl
            T b = z.imag();	// Lesezugriff auf komplexe Zahl
            T z_abs = (a*a) + (b*b);	// Betrag der komplexen Zahl z
            T dB = T(-320);       // Umwandlung in Dezibels, min = -16 * 20 dB
            if ( z_abs > T(1e-16) )
            {
                //if ( sizeof( T ) < 8 )
                {
                    dB = log10f( z_abs ); // Umwandlung in Dezibels
                }
                //else
                {
                    //   dB = log10( z_abs ); // Umwandlung in Dezibels
                }
                dB *= T(10); // Umwandlung in Dezibels ( incl. 1/2 for the removed sqrt(a^2+b^2) )
            }

            //if ( dB < T(-20) ) dB = T(-320); // Benni filter
            //if ( dB > T(0) ) dB *= T(2); // Benni enhancer

            *dst++ = dB;
        }

        //      for ( size_t i = n/2; i < n; i++ )
        //      {
        //         TComplex const & z = m_complexOutput[ i ];
        //         T a = z.real();	// Lesezugriff auf komplexe Zahl
        //         T b = z.imag();	// Lesezugriff auf komplexe Zahl
        //         *dst++ = float(180.0/M_PI) * atan2f( b, a );
        //      }
    }

    void
    fft() { m_fft.transform( m_complexInput.data(), m_complexOutput.data() ); }

    void
    ifft() { }

    void
    setInput( TComplex const* __restrict__ src, uint32_t srcCount )
    {
        auto fftSize = uint32_t( m_complexInput.size() );
        auto n = std::min( srcCount, fftSize );
        for ( size_t i = 0; i < n; ++i )
        {
            m_complexInput[ i ] = *src;
            src++;
        }

        // ZeroMemory last items.
        for ( size_t i = n; i < fftSize; ++i )
        {
            m_complexInput[ i ] = TComplex( T(0), T(0) );
        }
    }


    // Input is one channel of float audio data.
    void
    setInput( float const* __restrict__ src, uint32_t srcCount )
    {
        auto fftSize = uint32_t( m_complexInput.size() );
        auto n = std::min( srcCount, fftSize );

        for ( size_t i = 0; i < n; ++i )
        {
            m_complexInput[ i ] = TComplex( T( *src++ ), T(0) );
        }

        // ZeroMemory last items.
        for ( size_t i = n; i < fftSize; ++i )
        {
            m_complexInput[ i ] = TComplex( T(0), T(0) );
        }
    }

    void
    getOutput( TComplex* __restrict__ dst, uint32_t dstCount ) const
    {
        auto fftSize = uint32_t( m_complexOutput.size() );
        auto n = std::min( dstCount, fftSize );
        for ( size_t i = 0; i < n; i++ )
        {
            *dst = m_complexOutput[ i ];
            dst++;
        }
    }

    void
    getOutputAbs( float* __restrict__ dst, uint32_t dstCount ) const
    {
        auto fftSize = uint32_t( m_complexOutput.size() );
        auto n = std::min( dstCount, fftSize );
        for ( size_t i = 0; i < n; i++ )
        {
            TComplex const & z = m_complexOutput[ i ];
            T a = z.real();	// Lesezugriff auf komplexe Zahl
            T b = z.imag();	// Lesezugriff auf komplexe Zahl
            T z_abs = sqrt( (a*a) + (b*b) );	// Betrag der komplexen Zahl z
            *dst = z_abs;
            dst++;
        }
    }

    void
    getOutputInDecibel( float* __restrict__ dst, uint32_t dstCount )
    {
        auto n = std::min( dstCount, uint32_t( m_complexOutput.size() ) / 2 );

        for ( size_t i = 0; i < n; i++ )
        {
            TComplex const & z = m_complexOutput[ i ];
            T a = z.real();	// Lesezugriff auf komplexe Zahl
            T b = z.imag();	// Lesezugriff auf komplexe Zahl
            T z_abs = (a*a) + (b*b);	// Betrag der komplexen Zahl z
            T dB = T(-320);       // Umwandlung in Dezibels, min = -16 * 20 dB
            if ( z_abs > T(1e-16) )
            {
                //if ( sizeof( T ) < 8 )
                {
                    dB = log10f( z_abs ); // Umwandlung in Dezibels
                }
                //else
                {
                    //   dB = log10( z_abs ); // Umwandlung in Dezibels
                }
                dB *= T(10); // Umwandlung in Dezibels ( incl. 1/2 for the removed sqrt(a^2+b^2) )
            }

            //if ( dB < T(-20) ) dB = T(-320); // Benni filter
            //if ( dB > T(0) ) dB *= T(2); // Benni enhancer

            *dst++ = dB;
        }

        //      for ( size_t i = n/2; i < n; i++ )
        //      {
        //         TComplex const & z = m_complexOutput[ i ];
        //         T a = z.real();	// Lesezugriff auf komplexe Zahl
        //         T b = z.imag();	// Lesezugriff auf komplexe Zahl
        //         *dst++ = float(180.0/M_PI) * atan2f( b, a );
        //      }
    }


};

} // end namespace de
