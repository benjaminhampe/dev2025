#include <de/audio/dsp/ADSR.hpp>

namespace de {
namespace audio {

// =========================================================
ADSR::ADSR()
// =========================================================
   : m_isBypassed( false )
   , m_a(33.f)
   , m_d(200.f)
   , m_s(0.65f)
   , m_r(500.f)
{
   computeDuration();
}

std::string
ADSR::toString() const
{
   std::stringstream s;
   s << "a:" << m_a << ", "
        "d:" << m_d << ", "
        "s:" << m_s << ", "
        "r:" << m_r << ", "
      "sum:" << m_sum;
   return s.str();
}

void
ADSR::computeDuration() { m_sum = m_a + m_d + m_r; }

float
ADSR::getDuration() const { return 0.001 * m_sum; } // ms to sec

float
ADSR::computeSample( double td ) const
{
   if ( m_isBypassed )
   {
      return 1.0f; // neutral element
   }

   float t = float( 1000.0 * td ); // seconds to milliseconds
   //t = fmodf( t, m_sum ); // seconds to milliseconds

   if ( t <= 0.0f || t >= m_sum )
   {
      return 0.0f;
   }

   if ( t <= m_a )
   {
      float m = 1.0f / m_a;
      float v = t*m;
      return v*v;
   }

   t -= m_a;

   if ( t > 0.0f && t <= m_d )
   {
      float m = (1.0f-m_s)/m_d;
      float v = 1.0f - t*m;
      return v*v;
   }

   t -= m_d;

   if ( t > 0.0f && t < m_r )
   {
      float m = m_s/m_r;
      float v = m_s - t*m;
      return v*v;
   }

   return 0.0f;
}

bool
ADSR::isBypassed() const { return m_isBypassed; }
void
ADSR::setBypassed( bool bypassed ) { m_isBypassed = bypassed; }
void
ADSR::set( float a, float d, float s, float r )
{
   m_a = a;
   m_d = d;
   m_s = s;
   m_r = r;
   m_sum = m_a + m_d + m_r;
   DE_DEBUG( toString() )
}
void
ADSR::set( ADSR const & other ) { set( other.m_a, other.m_d, other.m_s, other.m_r ); }
void
ADSR::setA( float ms ) { m_a = ms; computeDuration(); }
void
ADSR::setD( float ms ) { m_d = ms; computeDuration(); }
void
ADSR::setS( float pc ) { m_s = pc; }
void
ADSR::setR( float ms ) { m_r = ms; computeDuration(); }

/*
//                   1
//                 --+--
//               --  |  ---
//             --    |      ---
//           --      |         ---+---------------+ sustain
//         -- m = 1/a| m = (s-1)/d|  m = 0        |
//       --   n = 0  | n = +1.0f  |  n = s        |
// 0___--____________|____________|_______________|______________ 0
// ####| Attack time | Decay time | sustain level |#####################
//
float noteOn( double timeSinceNoteOnStart ) const
{
   //float sum = std::abs(a + d + r);
   float t = float( 1000.0 * timeSinceNoteOnStart ); // seconds to milliseconds
   //t = fmodf( t, sum ); // seconds to milliseconds

   if ( t < 0.0f )
   {
      return 0.0f;
   }
   else if ( t <= a )
   {
      float m = 1.0f / a;
      return t*m;
   }
   else if ( t > a && t <= a + d )
   {
      float m = (s - 1.0f) / d;
      return t*m + 1.0f;
   }
   else
   {
      return s; //
   }
}

// 1
// +--
//    --
//      --
//        --------------+  sustain
//                      |----
//                      |    ----
//                      |        ----
// __________________0,0|____________----_____________ t,0
//        ADS           |  ReleaseTime   |

float noteOff( double timeSinceNoteOffStart ) const
{
   //float sum = std::abs(a + d + r);
   float t = float( 1000.0 * timeSinceNoteOffStart ); // seconds to milliseconds
   //t = fmodf( t, sum ); // seconds to milliseconds

   if ( t < 0.0f )
   {
      return 0.0f;
   }
   else if ( t <= r )
   {
      float m = s/r;
      return s - t*m;
   }
   else // if ( t > sum )
   {
      return 0.0f;
   }
}

//        1
//       /|\
//      / | \
//     /  |  ---  sustain
// ___/   |  |   \_________________ 0
//    | a | d| r |
//
float computeValue( double td ) const
{
   //float sum = std::abs(a + d + r);
   float t = float( 1000.0 * td ); // seconds to milliseconds
   //t = fmodf( t, sum ); // seconds to milliseconds

   if ( t < 0.0f )
   {
      return 0.0f;
   }
   else if ( t <= a )
   {
      float m = 1.0f / a;
      return t*m;
   }
   else if ( t > a && t <= a + d )
   {
      float m = -(1.0f-s)/d;
      return (t-a)*m + 1.0f;
   }
   else if ( t > a + d && t <= a + d + r )
   {
      float m = -s/r;
      return (t-(a+d))*m + s;
   }
   else // if ( t > sum )
   {
      return 0.0f;
   }
}

// [DebugBegin]
uint32_t
computeColor( float td ) const
{
   float sum = std::abs(a + d + r);
   float t = float( 1000.0 * td ); // seconds to milliseconds
   t = fmodf( t, sum ); // seconds to milliseconds

   if ( t < 0.0f )
   {
      return 0xFF000000;
   }
   else if ( t >= 0.0f && t < a )
   {
      return 0xFF00FFFF;
   }
   else if ( t >= a && t < a + d )
   {
      return 0xFF0000FF;
   }
   else if ( t >= a + d && t <= a + d + r )
   {
      return 0xFFFF0000;
   }
   else // if ( t > sum )
   {
      return 0xFFFFFFFF;
   }
}
// [DebugEnd]

static float
fancyWave( float t, float freq = 440.0f, float phase = 0.2f, float phase2 = -0.2f )
{
   float a = sinf( float(M_PI)*freq*t );
   float b = sinf( float(M_PI*2.0)*freq*t );
   float c = sinf( float(M_PI*2.0)*2.0f*freq*t + phase );
   float d = sinf( float(M_PI*2.0)*3.0f*freq*t + phase2 );
   return (a + b + c + d) / 4.0f;
}

static void test()
{
   ADSR adsr;

   Image img( 2*1024,2*512 );
   img.fill( 0xFF802020 );
   int w = img.getWidth();
   int h = img.getHeight();

   float t_min = -0.2f; // s
   float t_max = 2.0f;
   float t_dif = t_max - t_min;
   {
      int x1 = float(w)*(0.0f - t_min) / t_dif;
      int x2 = x1;
      int y1 = 0;
      int y2 = h-1;
      ImagePainter::drawLine( img, x1,y1, x2,y2, 0xFFFFFFFF );
   }
   {
      int x1 = 0;
      int x2 = w-1;
      int y1 = float(h) * 0.5f;
      int y2 = y1;
      ImagePainter::drawLine( img, x1,y1, x2,y2, 0xFFFFFFFF );
   }

   int n = w;
   float dt = t_dif / float(n);
   float dx = float(w) / float( n );
   float dy = float(h) / float( 2 );

   // Sin
   for ( int i = 1; i < n; i++ )
   {
      float t1 = dt * (i-1) + t_min;
      float t2 = dt * i + t_min;
      float h1 = adsr.computeValue( t1 ) * fancyWave( t1 );
      float h2 = adsr.computeValue( t2 ) * fancyWave( t2 );
      int x1 = int(dx * (i-1));
      int x2 = int(dx * i);
      int y1 = float(h/2) - h1 * dy;
      int y2 = float(h/2) - h2 * dy;

      ImagePainter::drawLine( img, x1,y1, x2,y2, 0xFFCCCCCC );
   }

   for ( int i = 1; i < n; i++ )
   {
      float t1 = dt * (i-1) + t_min;
      float t2 = dt * i + t_min;
      float h1 = adsr.computeValue( t1 );
      float h2 = adsr.computeValue( t2 );
      auto c1 = adsr.computeColor( t1 );
      auto c2 = adsr.computeColor( t2 );
      int x1 = int(dx * (i-1));
      int x2 = int(dx * i);
      int y1 = float(h/2) - h1 * dy;
      int y2 = float(h/2) - h2 * dy;

      ImagePainter::drawLine( img, x1,y1, x2,y2, c2 );
   }


   dbSaveImage( img, "ADSR.test1.png" );

   SampleBuffer sbf( ST_F32I, 2, 44100 );
   sbf.resize( 10*44100 );

   for ( size_t i = 0; i < 10*44100; ++i )
   {
      double t = double( i ) / double( sbf.getSampleRate() );

      float sample = 0.0f;
      sample += adsr.computeValue( t ) * fancyWave( t, 440, 0,0 );

      if ( t > 1 && t < 4 )
         sample += adsr.computeValue( t ) * fancyWave( t, 387, 0,0 );
      if ( t > 3 && t < 7 )
         sample += adsr.computeValue( t ) * fancyWave( t, 554, 0,0 );
      if ( t > 6 && t < 7 )
         sample += adsr.computeValue( t ) * fancyWave( t, 665, 0,0 );
      if ( t > 7 && t < 8 )
         sample += adsr.computeValue( t ) * fancyWave( t, 705, 0,0 );
      if ( t > 1 && t < 9 )
         sample += adsr.computeValue( t ) * fancyWave( t, 645, 0,0 );

      sample = std::clamp( sample, -1.0f, 1.0f );

      sbf.setSampleF32( i,0,sample );
      sbf.setSampleF32( i,1,sample );
   }

   SampleBufferIO::save( sbf, "ADSR.test1.wav" );
   SampleBufferIO::save( sbf, "ADSR.test1.mp3" );
}

SampleBuffer
createSin( uint32_t n, int sampleRate, float amp, float freq, float phase, ADSR hulle )
{
   SampleBuffer dst;
   dst.setFormat( ST_F32I, 2, sampleRate );
   dst.resize( n );

   for ( size_t i = 0; i < n; i++ )
   {
      float t =
      float arg = (2.0 * M_PI) * freq * t + phase;
      float sample = amp * sinf( arg );
      hulle
      dst.setSample( i, 0, )
   }
}
*/


} // end namespace audio.
} // end namespace de.
