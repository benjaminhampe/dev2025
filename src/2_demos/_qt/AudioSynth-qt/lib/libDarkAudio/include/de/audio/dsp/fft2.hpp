#ifndef DE_AUDIO_FFT_HPP
#define DE_AUDIO_FFT_HPP

#include <de/audio/dsp/ShiftBuffer.hpp>

#ifdef _IRR_LINUX_PLATFORM_
#include <fftw3.h>
#else
#include <fftw3/fftw3.h>
#endif

#include <complex>

namespace de {
namespace audio {

// FFT real to complex
struct FFT_R2C
{
   typedef std::complex< double > TComplex;
   typedef double R;
   typedef fftw_complex C;

   DE_CREATE_LOGGER("de.audio.FFT_R2C")
   uint32_t m_fftSize;
   C* m_inBuf; // complex output-vector
   C* m_outBuf; // complex output-vector
   fftw_plan m_fftPlan;

   FFT_R2C()
      : m_fftSize( 8192 )
      , m_inBuf( nullptr )
      , m_outBuf( nullptr )
      , m_fftPlan( nullptr )
   {
      m_inBuf = (C*)fftw_malloc( m_fftSize * sizeof(C) );
      m_outBuf = (C*)fftw_malloc( m_fftSize * sizeof(C) );
   }

   ~FFT_R2C()
   {
      if (m_fftPlan) { fftw_destroy_plan(m_fftPlan); m_fftPlan = 0; }
      if (m_inBuf) { fftw_free(m_inBuf); m_inBuf = 0; }
      if (m_outBuf) { fftw_free(m_outBuf); m_outBuf = 0; }
      //fftw_export_wisdom_to_filename( m_fftWisdomFile.c_str() ); // Store Wisdom
      clear();
   }

   void clear()
   {
      fftw_cleanup();
      //m_fftSize = 0;
   }

   void setFFTSize( uint32_t fftSize )
   {
      m_fftSize = fftSize;
      if ( !m_fftPlan )
      {
         m_fftPlan = fftw_plan_dft_1d( m_fftSize, m_inBuf, m_outBuf, FFTW_FORWARD, FFTW_ESTIMATE );
      }
   }

   void fft( float const* src, TComplex* dst )
   {
      //ZeroMemory( m_inBuf, m_fftSize );
      //zeroMemory( m_outBuf );

      float const* pSrc = src;
      C* pDst = m_inBuf;
      for ( size_t i = 0; i < m_fftSize; i++ )
      {
         float sample = *pSrc;
         C & z = *pDst;
         z[ 0 ] = sample;
         z[ 1 ] = 0.0;
         pSrc++;
         pDst++;
      }

      fftw_execute( m_fftPlan );

      C const* qSrc = m_outBuf;
      TComplex* qDst = dst;
      for ( size_t i = 0; i < m_fftSize; i++ )
      {
         fftw_complex const & z = *qSrc;
         R re = z[ 0 ];
         R im = z[ 1 ];
         *qDst = TComplex( re, im );
         qSrc++;
         qDst++;
      }

   }

   void zeroMemory( fftw_complex* p )
   {
      for ( size_t i = 0; i < m_fftSize; i++ )
      {
         fftw_complex & z = *p;
         z[ 0 ] = 0.0;
         z[ 1 ] = 0.0;
         p++;
      }
   }

   uint32_t getFFTSize() const { return m_fftSize; }

//   void fft()
//   {
//      fftw_execute( m_fftPlan );
//   }

//   fftw_complex* getOutput()
//   {
//      size_t const i_max = std::min( _out.size(), size_t(m_fftSize) );
//      _out.clear();
//      _out.reserve( i_max );
//      for ( size_t i = 0; i < i_max; i++ )
//      {
//         fftw_complex const & z = m_OutBuf[ i ];	// Lesezugriff auf komplexe Zahl
//         double z_abs = sqrt( z[0]*z[0] + z[1]*z[1] );	// Betrag der komplexen Zahl z
//         z_abs = 20.0f*log10( z_abs );				// Umwandlung in Dezibels
//         _out.push_back( T( z_abs ) );
//      }
//   }

};

// FFT real to complex
struct IFFT_C2R
{
   typedef std::complex< double > TComplex;

   typedef fftw_complex C;
   typedef double R;

   DE_CREATE_LOGGER("de.audio.IFFT")
   uint32_t m_fftSize;
   C* m_inBuf;  // complex input-vector
   C* m_outBuf; // real output-vector
   fftw_plan m_fftPlan;

   IFFT_C2R()
      : m_fftSize( 8192 )
      , m_inBuf( nullptr )
      , m_outBuf( nullptr )
      , m_fftPlan( nullptr )
   {
      m_inBuf = (C*)fftw_malloc( m_fftSize * sizeof(C) );
      m_outBuf = (C*)fftw_malloc( m_fftSize * sizeof(C) );
   }

   ~IFFT_C2R()
   {
      //fftw_export_wisdom_to_filename( m_fftWisdomFile.c_str() ); // Store Wisdom
      if (m_fftPlan) { fftw_destroy_plan(m_fftPlan); m_fftPlan = 0; }
      if (m_inBuf) { fftw_free(m_inBuf); m_inBuf = 0; }
      if (m_outBuf) { fftw_free(m_outBuf); m_outBuf = 0; }
      fftw_cleanup();
   }

   void clear()
   {

      //m_fftSize = 0;
   }

   void setFFTSize( uint32_t fftSize )
   {
      m_fftSize = fftSize;
      if ( !m_fftPlan )
      {
         m_fftPlan = fftw_plan_dft_1d( m_fftSize, m_inBuf, m_outBuf, FFTW_BACKWARD, FFTW_ESTIMATE );
      }
   }

   void ifft( TComplex const* src, float* dst )
   {
      //zeroMemory( m_inBuf );
      //ZeroMemory( m_outBuf, m_fftSize );

      // Copy Spektral filtered input (double) to iFFT input
      TComplex const* pSrc = src;
      C* pDst = m_inBuf;
      for ( size_t i = 0; i < m_fftSize; i++ )
      {
         TComplex const & zSrc = *pSrc;
         C & z = *pDst;
         z[ 0 ] = zSrc.real();
         z[ 1 ] = zSrc.imag();
         pSrc++;
         pDst++;
      }

      fftw_execute( m_fftPlan );

      // Copy iFFT output (double) to user output (float)
      C const* qSrc = m_outBuf;
      float* qDst = dst;
      for ( size_t i = 0; i < m_fftSize; i++ )
      {
         C const & z = *qSrc;
         double a = z[ 0 ];
         double b = z[ 1 ];
         *qDst = sqrt( a*a + b*b );
         qSrc++;
         qDst++;
      }
   }

   void zeroMemory( fftw_complex* p )
   {
      for ( size_t i = 0; i < m_fftSize; i++ )
      {
         fftw_complex & z = *p;
         z[ 0 ] = 0.0;
         z[ 1 ] = 0.0;
         p++;
      }
   }

   uint32_t getFFTSize() const { return m_fftSize; }

//   void fft()
//   {
//      fftw_execute( m_fftPlan );
//   }

//   fftw_complex* getOutput()
//   {
//      size_t const i_max = std::min( _out.size(), size_t(m_fftSize) );
//      _out.clear();
//      _out.reserve( i_max );
//      for ( size_t i = 0; i < i_max; i++ )
//      {
//         fftw_complex const & z = m_OutBuf[ i ];	// Lesezugriff auf komplexe Zahl
//         double z_abs = sqrt( z[0]*z[0] + z[1]*z[1] );	// Betrag der komplexen Zahl z
//         z_abs = 20.0f*log10( z_abs );				// Umwandlung in Dezibels
//         _out.push_back( T( z_abs ) );
//      }
//   }

};

} // end namespace audio
} // end namespace de

#endif
