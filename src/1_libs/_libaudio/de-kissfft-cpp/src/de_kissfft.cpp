#include <de_kissfft.hpp>
#include <cassert>

namespace de {

KissFFT::KissFFT( uint32_t fftSize, bool bInverse )
  : m_fftSize( fftSize )
  , m_inverse( bInverse )
  , m_fft( fftSize, bInverse )
{
  m_fftInput.resize( m_fftSize );
  m_fftOutput.resize( m_fftSize );
}

// void
// setFFTSize( uint32_t fftSize )
// {
  // //if ( m_fftSize == fftSize ) return;
  // m_fftSize = fftSize;
  // m_fft.prepare( m_fftSize, m_inverse );

// }

uint32_t
KissFFT::size() const
{
  return m_fftSize;
}

void
KissFFT::fft()
{
  m_fft.transform( m_fftInput.data(), m_fftOutput.data() );
}

// Input is one channel of float audio data.
void
KissFFT::setInput( float const* src, uint32_t srcCount )
{
  auto fftSize = uint32_t( m_fftInput.size() );
  auto n = std::min( srcCount, fftSize );
  for ( size_t i = 0; i < n; ++i )
  {
     m_fftInput[ i ] = TComplex( T( *src ), T(0) );
     src++;
  }

  // ZeroMemory last items.
  for ( size_t i = n; i < fftSize; ++i )
  {
     m_fftInput[ i ] = TComplex( T(0), T(0) );
  }
}

void
KissFFT::setInput( TComplex const* src, uint32_t srcCount )
{
  auto fftSize = uint32_t( m_fftInput.size() );
  auto n = std::min( srcCount, fftSize );
  for ( size_t i = 0; i < n; ++i )
  {
     m_fftInput[ i ] = *src;
     src++;
  }

  // ZeroMemory last items.
  for ( size_t i = n; i < fftSize; ++i )
  {
     m_fftInput[ i ] = TComplex( T(0), T(0) );
  }
}

KissFFT::TComplex const*
KissFFT::getOutput() const { return m_fftOutput.data(); }

KissFFT::TComplex*
KissFFT::getOutput() { return m_fftOutput.data(); }

void
KissFFT::getOutput( TComplex* dst, uint32_t dstCount ) const
{
  auto fftSize = uint32_t( m_fftOutput.size() );
  auto n = std::min( dstCount, fftSize );
  for ( size_t i = 0; i < n; i++ )
  {
     *dst = m_fftOutput[ i ];
     dst++;
  }
}

void
KissFFT::getOutputAbs( float* dst, uint32_t dstCount ) const
{
  auto fftSize = uint32_t( m_fftOutput.size() );
  auto n = std::min( dstCount, fftSize );
  for ( size_t i = 0; i < n; i++ )
  {
     TComplex const & z = m_fftOutput[ i ];
     T a = z.real();	// Lesezugriff auf komplexe Zahl
     T b = z.imag();	// Lesezugriff auf komplexe Zahl
     T z_abs = sqrt( (a*a) + (b*b) );	// Betrag der komplexen Zahl z
     *dst = z_abs;
     dst++;
  }
}

void
KissFFT::getOutputInDecibel( float* dst, uint32_t dstCount )
{
  auto n = std::min( dstCount, uint32_t( m_fftOutput.size() ) / 2 );

  for ( size_t i = 0; i < n; i++ )
  {
     TComplex const & z = m_fftOutput[ i ];
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
//         TComplex const & z = m_fftOutput[ i ];
//         T a = z.real();	// Lesezugriff auf komplexe Zahl
//         T b = z.imag();	// Lesezugriff auf komplexe Zahl
//         *dst++ = float(180.0/M_PI) * atan2f( b, a );
//      }
}


} // end namespace de

