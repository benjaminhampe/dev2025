#include <de/audio/buffer/SampleType.hpp>

namespace de {
namespace audio {

// MIDI stuff:
// get frequency from piano key ( C = 0, H = 11 for A4 use getFrequencyFromNote(9,4) = 440 Hz )
//inline float
//getFrequencyFromNote( int C_is_zero, int oktave )
//{
//   return 440.f * powf( 2.f, (float)( (C_is_zero%12) + (12*oktave) - 69) / 12.f );
//}

//Tones Oktave Frequenzen

// f( oktave = 0, semi = 0 ) = 16,35
// f( oktave = 4, semi = 9 ) = 440

// f( oktave, semi ) = 440Hz * 2 ^ [(oktave-4)+(semi-9)/12)]

// f( 0, 0 ) 440 * 2 ^ -4,75 = 16,35;
// f( 4, 9 ) 440 * 2 ^ [(4-4)+(9-9)/12)] = 440;
// f( 7, 11 ) 440 * 2 ^ [3+1/6] = 3951,07;

//        0       1       2       3       4       5       6       7
//0 - C   16,35   32,70   65,41   130,81  261,63  523,25  1046,50 2093,00
//1 - C’  17,32   34,65   69,30   138,59  277,18  554,37  1108,74 2217,46
//2 - D   18,35   36,71   73,42   146,83  293,66  587,33  1174,66 2349,32
//3 - D’  19,45   38,89   77,78   155,56  311,13  622,25  1244,51 2489,02
//4 - E   20,60   41,20   82,41   164,81  329,63  659,26  1328,51 2637,02
//5 - F   21,83   43,65   87,31   174,61  349,23  698,46  1396,91 2793,83
//6 - F’  23,12   46,25   92,50   185,00  369,99  739,99  1479,98 2959,96
//7 - G   24,50   49,00   98,00   196,00  392,00  783,99  1567,98 3135,96
//8 - G’  25,96   51,91   103,83  207,65  415,30  830,61  1661,22 3322,44
//9 - A   27,50   55,00   110,00  220,00  440,00  880,00  1760,00 3520,00
//10- A’  29,14   58,27   116,54  233,08  466,16  923,33  1864,66 3729,31
//11- H   30,87   61,74   123,47  246,94  493,88  987,77  1975,53 3951,07

//inline float
//getFrequencyFromNote( int oktave, int semitone_C_is_zero, float detune = 0.0f )
//{
//   return 440.f * powf( 2.f, float(oktave - 4) + ((float(semitone_C_is_zero - 9)+detune) / 12.f) );
//}

//inline float
//getFrequencyFromMidiNote( int midiNote, float detune = 0.0f )
//{
//   int oktave = midiNote / 12;
//   int semi = midiNote - 12 * oktave;
//   //DEM_DEBUG("midiNote(",midiNote,") = oktave(",oktave,"),semi(",semi,")")
//   return getFrequencyFromNote( oktave, semi, detune );
//}

float
getFrequencyFromNote( int oktave, float semitone_C_is_zero )
{
   // 440Hz = A4 = 12 * 5 + 9 = 69;
   return getFrequencyFromMidiNote( 12.0f*oktave + semitone_C_is_zero );
//   int semi = midiNote - 12 * oktave;
//   //DEM_DEBUG("midiNote(",midiNote,") = oktave(",oktave,"),semi(",semi,")")
//   return getFrequencyFromNote( oktave, semi, detune );
}


float
getFrequencyFromMidiNote( float midiNote )
{
   // 440Hz = A4 = 12 * 5 + 9 = 69  -> should be 440 * 2^0;
   return 440.0f * powf( 2.0f, (midiNote - 69.0f) * (1.0f/12.0f) );
//   int semi = midiNote - 12 * oktave;
//   //DEM_DEBUG("midiNote(",midiNote,") = oktave(",oktave,"),semi(",semi,")")
//   return getFrequencyFromNote( oktave, semi, detune );
}

float
volume_to_dB( float const volume )
{
   return 20.0f * ::log10f( volume );
}

float
dB_to_volume( float const dB )
{
   return ::powf( 10.0f, 0.05f * dB );
}

float
clampf( float value, float min_value, float max_value)
{
   return std::min( std::max( value, min_value), max_value );
}

double
clampd( double value, double min_value, double max_value)
{
   return std::min( std::max( value, min_value), max_value );
}








float const C64_32767_OVER_127 = float( 32767.0 / 127.0); // convert positive int8_t to int16_t
float const C64_32767_OVER_8388607 = float( 32767.0 / 8388607.0); // convert positive s24 to int16_t

double const C64_32767_OVER_2147483647 = 32767.0 / 2147483647.0; // convert positive s24 to int16_t

double const C64_INV_128 = 1.0 / 128.0;
double const C64_INV_127 = 1.0 / 127.0;
double const C64_INV_32768 = 1.0 / 32768.0;
double const C64_INV_32767 = 1.0 / 32767.0;

float const C32_INV_128 = float( C64_INV_128 );
float const C32_INV_127 = float( C64_INV_127 );
float const C32_INV_32768 = float( C64_INV_32768 );
float const C32_INV_32767 = float( C64_INV_32767 );

double const C64_INV_8388608 = 1.0 / 8388608.0;
double const C64_INV_8388607 = 1.0 / 8388607.0;
double const C64_INV_2147483648 = 1.0 / 2147483648.0;
double const C64_INV_2147483647 = 1.0 / 2147483647.0;

inline int32_t
s32_clamp( int32_t value, int32_t minValue, int32_t maxValue )
{
   if ( maxValue < minValue ) { std::swap( minValue, maxValue ); }
   return std::min( std::max( value, minValue ), maxValue );
   //return Math::clampT< int32_t >( value, minValue, maxValue );
}

inline int32_t
s32_round( float v )
{
   return std::lroundf( v );
}

inline int32_t
s32_round( double v )
{
   return std::lround( v );
}

//// ============================================================================
//bool
//SampleTypeConverter::needSampleTypeConverter( ESampleType src, ESampleType dst )
//// ============================================================================
//{
//    if (ST_contains(src,dst)) return false;
//    return true;
//}

// ============================================================================
bool
ST_Validate( ESampleType sampleType )
{
   if ( ST_isUnknown(sampleType) ) return false;

   if (  ST_is8Bit(sampleType) &&
         ST_is16Bit(sampleType) &&
         ST_is24Bit(sampleType) &&
         ST_is32Bit(sampleType) &&
         ST_is64Bit(sampleType) )
   {
      DE_ERROR("Invalid sampleType has no Bits set, 8,16,32,etc...")
      return false;
   }

   return true;
}


// ============================================================================
SampleTypeConverter::Converter_t
SampleTypeConverter::getConverter( ESampleType src, ESampleType dst )
// ============================================================================
{
   Converter_t converter = nullptr;

   if ( ST_contains( dst, ST_F32) )
   {
      if ( ST_contains( src, ST_S16 )) converter = converter_S16_to_F32;
      else if (ST_contains( src, ST_F32)) converter = converter_F32_to_F32;
      else if (ST_contains( src, ST_S8))  converter = converter_S8_to_F32;
      else if (ST_contains( src, ST_U8))  converter = converter_U8_to_F32;
      else if (ST_contains( src, ST_S24)) converter = converter_S24_to_F32;
      else if (ST_contains( src, ST_S32)) converter = converter_S32_to_F32;
      else if (ST_contains( src, ST_F64)) converter = converter_F64_to_F32;
      else if (ST_contains( src, ST_U16)) converter = converter_U16_to_F32;
      else if (ST_contains( src, ST_U24)) converter = converter_U24_to_F32;
      else if (ST_contains( src, ST_U32)) converter = converter_U32_to_F32;
      else { DE_ERROR("No converter from(",ST_toString(src),") to (ST_F32)") }
   }
   else if ( ST_contains( dst,ST_F64 ) )
   {
      if (ST_contains( src, ST_S16)) converter = converter_S16_to_F64;
      else if (ST_contains( src, ST_F32)) converter = converter_F32_to_F64;
      else if (ST_contains( src, ST_S8))  converter = converter_S8_to_F64;
      else if (ST_contains( src, ST_U8))  converter = converter_U8_to_F64;
      else if (ST_contains( src, ST_S24)) converter = converter_S24_to_F64;
      else if (ST_contains( src, ST_S32)) converter = converter_S32_to_F64;
      else if (ST_contains( src, ST_F64)) converter = converter_F64_to_F64;
      else if (ST_contains( src, ST_U16)) converter = converter_U16_to_F64;
      else if (ST_contains( src, ST_U24)) converter = converter_U24_to_F64;
      else if (ST_contains( src, ST_U32)) converter = converter_U32_to_F64;
      else { DE_ERROR("No converter from(", ST_toString(src),") to (ST_F64)") }
   }
   else if ( ST_contains( dst,ST_S16 ) )
   {
      if (ST_contains( src, ST_S16)) converter = converter_S16_to_S16;
      else if (ST_contains( src, ST_F32)) converter = converter_F32_to_S16;
      else if (ST_contains( src, ST_S8))  converter = converter_S8_to_S16;
      else if (ST_contains( src, ST_U8))  converter = converter_U8_to_S16;
      else if (ST_contains( src, ST_S24)) converter = converter_S24_to_S16;
      else if (ST_contains( src, ST_S32)) converter = converter_S32_to_S16;
      else if (ST_contains( src, ST_F64)) converter = converter_F64_to_S16;
      else if (ST_contains( src, ST_U16)) converter = converter_U16_to_S16;
      else if (ST_contains( src, ST_U24)) converter = converter_U24_to_S16;
      else if (ST_contains( src, ST_U32)) converter = converter_U32_to_S16;
      else { DE_ERROR("No valid src sample-type from(",ST_toString(src),") to (", ST_toString(dst),")") }
   }
   else if ( ST_contains( dst,ST_S32 ) )
   {
      if (ST_contains( src, ST_S16)) converter = converter_S16_to_S32;
      else if (ST_contains( src, ST_F32)) converter = converter_F32_to_S32;
      else if (ST_contains( src, ST_S8))  converter = converter_S8_to_S32;
      else if (ST_contains( src, ST_U8))  converter = converter_U8_to_S32;
      else if (ST_contains( src, ST_S24)) converter = converter_S24_to_S32;
      else if (ST_contains( src, ST_S32)) converter = converter_S32_to_S32;
      else if (ST_contains( src, ST_F64)) converter = converter_F64_to_S32;
      else if (ST_contains( src, ST_U16)) converter = converter_U16_to_S32;
      else if (ST_contains( src, ST_U24)) converter = converter_U24_to_S32;
      else if (ST_contains( src, ST_U32)) converter = converter_U32_to_S32;
      else { DE_ERROR("No valid src sample-type from(",ST_toString(src),") to (", ST_toString(dst),")") }
   }
   else if ( ST_contains( dst,ST_S24 ) ) // echt komplizierter Kram, aber hohe Qualitaet
   {
      if (ST_contains( src, ST_S16)) converter = converter_S16_to_S24;
      else if (ST_contains( src, ST_F32)) converter = converter_F32_to_S24;
      else if (ST_contains( src, ST_S8))  converter = converter_S8_to_S24;
      else if (ST_contains( src, ST_S24)) converter = converter_S24_to_S24;
      else { DE_ERROR("No valid src sample-type from(",ST_toString(src),") to (", ST_toString(dst),")") }
   }
   else
   {
      DE_ERROR("No valid dst sample-type from(",ST_toString(src),") to (", ST_toString(dst),")")
   }

   if ( !converter )
   {
      DE_ERROR("No sample-type converter from(",ST_toString(src),") to (", ST_toString(dst),")")
   }
   else
   {
      // DE_DEBUG("Found sample-type converter from(",ST_toString(src),") to (", ST_toString(dst),")")
   }

   return converter;
}

///
/// S16
///

// ============================================================================
void SampleTypeConverter::converter_S8_to_S16 ( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S8_to_S16(%x,%x,%d)\n", dst,src,sampleCount)

    int16_t* pDst = reinterpret_cast< int16_t* >( dst );
    int8_t const* pSrc = reinterpret_cast< int8_t const* >( src );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int16_t const value = *pSrc;

        if (value < 0)
        {
            *pDst = int16_t( value << 8 );
        }
        else
        {
            *pDst = int16_t( C64_32767_OVER_127 * value );
        }

        pDst++;
        pSrc++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_U8_to_S16 ( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U8_to_S16(%x,%x,%d)\n", dst,src,sampleCount)
    uint8_t const* pSrc = reinterpret_cast< uint8_t const* >( src );
    int16_t* pDst = reinterpret_cast< int16_t* >( dst );
    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int_fast16_t const value = int16_t( *pSrc ) - 128;

        if (value < 0)
        {
            *pDst = int16_t( value << 8 );
        }
        else
        {
            *pDst = int16_t(C64_32767_OVER_127 * value );
        }

        pDst++;
        pSrc++;
    }
}

// hier muss logischerweise nix konvertiert werden, nur kopiert.
// ============================================================================
void SampleTypeConverter::converter_S16_to_S16( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    ::memcpy( dst, src, sampleCount * sizeof(int16_t) );
//    int16_t* pDst = reinterpret_cast< int16_t* >( dst );
//    int16_t const* pSrc = reinterpret_cast< int16_t const* >( src );

//    for ( size_t i = 0; i < sampleCount; ++i )
//    {
//        *pDst = *pSrc;
//        pSrc++;
//        pDst++;
//    }
}

// ============================================================================
void SampleTypeConverter::converter_U16_to_S16( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U16_to_S16(%x,%x,%d)\n", dst,src,sampleCount)

   uint16_t const* pSrc = reinterpret_cast< uint16_t const* >( src );
   int16_t* pDst = reinterpret_cast< int16_t* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        *pDst = int16_t( static_cast< int_fast32_t >( *pSrc ) - 32768 );
        pDst++;
        pSrc++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_S24_to_S16( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S24_to_S16(%x,%x,%d)\n", dst,src,sampleCount)

    int16_t* pDst = reinterpret_cast< int16_t* >( dst );
    uint8_t const* pSrc = reinterpret_cast< uint8_t const* >( src );

    MyS24 value;

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        value.c[0] = (*pSrc); pSrc++;
        value.c[1] = (*pSrc); pSrc++;
        value.c[2] = (*pSrc); pSrc++;
        value.c[3] = 0;

        /// Repair the minus-sign ( overflow makes highest bit set )
        if (value.i & 0x800000)
        {
            value.i |= ~0xffffff; // Zweierkomplement
        }

        if (value.i < 0)
        {
            *pDst = int16_t(value.i >> 3);
        }
        else
        {
            *pDst = int16_t(C64_32767_OVER_8388607 * value.i);
        }

        pDst++;
    }

}

// ============================================================================
void SampleTypeConverter::converter_U24_to_S16( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U24_to_S16(%x,%x,%d)\n", dst,src,sampleCount)
    // throw std::runtime_error("audio::converter_U24_to_S16() - Not implemented!");
   static_cast< void >( dst );
   static_cast< void >( src );
   static_cast< void >( sampleCount );
   DE_ERROR("Not implemented")
}

// ============================================================================
void SampleTypeConverter::converter_S32_to_S16( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S32_to_S16(%x,%x,%d)\n", dst,src,sampleCount)
    int32_t const* pSrc = reinterpret_cast< int32_t const* >( src );
    int16_t* pDst = reinterpret_cast< int16_t* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int_fast32_t const value = *pSrc;

        if (value < 0)
        {
            *pDst = int16_t(value >> 16);
        }
        else
        {
            *pDst = int16_t(C64_32767_OVER_2147483647 * value);
        }

        pDst++;
        pSrc++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_U32_to_S16( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U32_to_S16(%x,%x,%d)\n", dst,src,sampleCount)
    uint32_t const* pSrc = reinterpret_cast< uint32_t const* >( src );
    int16_t* pDst = reinterpret_cast< int16_t* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int_fast32_t const value = int_fast32_t( int64_t(*pSrc) - 2147483648 );

        if (value < 0)
        {
            *pDst = int16_t(value >> 16);
        }
        else
        {
            *pDst = int16_t(C64_32767_OVER_2147483647 * value);
        }

        pSrc++;
        pDst++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_F32_to_S16( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_F32_to_S16(%x,%x,%d)\n", dst,src,sampleCount)
    float const* pSrc = reinterpret_cast< float const* >( src );
    int16_t* pDst = reinterpret_cast< int16_t* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        float const value = *pSrc;

        if (value < 0)
        {
            *pDst = int16_t( s32_clamp( s32_round( 32768.0f * value), -32768, -1) );
        }
        else
        {
            *pDst = int16_t( s32_clamp( s32_round( 32767.0f * value), 0, 32767) );
        }

        pSrc++;
        pDst++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_F64_to_S16( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_F64_to_S16(%x,%x,%d)\n", dst,src,sampleCount)
    float const* pSrc = reinterpret_cast< float const* >( src );
    int16_t* pDst = reinterpret_cast< int16_t* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        float const value = *pSrc;

        if (value < 0)
        {
            *pDst = int16_t( s32_clamp( s32_round( 32768.0f * value ), -32768, -1) );
        }
        else
        {
            *pDst = int16_t( s32_clamp( s32_round( 32767.0f * value ), 0, 32767) );
        }

        pSrc++;
        pDst++;
    }
}

///
/// F32
///

// ============================================================================
void SampleTypeConverter::converter_S8_to_F32 ( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S8_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    int8_t const* pSrc = reinterpret_cast< int8_t const* >( src );
    float* pDst = reinterpret_cast< float* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int8_t const value = *pSrc;
        if (value < 0)
        {
            *pDst = C32_INV_128 * value;
        }
        else
        {
            *pDst = C32_INV_127 * value;
        }

        pSrc++;
        pDst++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_U8_to_F32 ( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U8_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    uint8_t const* pSrc = reinterpret_cast< uint8_t const* >( src );
    float* pDst = reinterpret_cast< float* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int_fast16_t const value = int16_t( *pSrc ) - 128;
        if (value < 0)
        {
            *pDst = C32_INV_128 * value;
        }
        else
        {
            *pDst = C32_INV_127 * value;
        }
        pSrc++;
        pDst++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_S16_to_F32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
   // MMDebug("audio::converter_S16_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
   int16_t const* pSrc = reinterpret_cast< int16_t const* >( src );
   float* pDst = reinterpret_cast< float* >( dst );

   for ( size_t i = 0; i < sampleCount; ++i )
   {
      int16_t const value = *pSrc;
      if (value < 0)
      {
         *pDst = C32_INV_32768 * value;
      }
      else
      {
         *pDst = C32_INV_32768 * value;
      }
      pSrc++;
      pDst++;
   }
}

// ============================================================================
void SampleTypeConverter::converter_U16_to_F32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U16_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    uint16_t const* pSrc = reinterpret_cast< uint16_t const* >( src );
    float* pDst = reinterpret_cast< float* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int_fast32_t const value = int_fast32_t(*pSrc) - 32768;
        if (value < 0)
        {
            *pDst = C32_INV_32768 * value;
        }
        else
        {
            *pDst = C32_INV_32767 * value;
        }
        pSrc++;
        pDst++;
    }
}

//unsigned char* createS24( const int& i )
//{
//    unsigned char s24[3];
//    s24[0] = (i & 0x000000ff);
//    s24[1] = (i & 0x0000ff00) >> 8;
//    s24[2] = (i & 0x00ff0000) >> 16;
//    return s24;
//}

// ============================================================================
void SampleTypeConverter::converter_S24_to_F32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
   double const ONE_OVER_23BIT = 1.0 / 8388608.0;

   uint8_t const* pSrc = reinterpret_cast< uint8_t const* >( src );
   float* pDst = reinterpret_cast< float* >( dst );

   for ( size_t i = 0; i < sampleCount; i++ )
   {
      int32_t value = *((int32_t*)pSrc);
      value = value & 0x00ffffff;             // take 24 bits
      value |= (value & 0x00800000) ? 0xff000000 : 0;  // extend minus sign bits
      pSrc += 3;

      *pDst = float(value * ONE_OVER_23BIT);
      pDst++;
   }

#if 0
   // MMDebug("audio::converter_S24_to_F32(%x,%x,%d)\n", dst,src,sampleCount)

   uint8_t const* pSrc = reinterpret_cast< uint8_t const* >( src );
   float* pDst = reinterpret_cast< float* >( dst );

   MyS24 value;

   for ( size_t i = 0; i < sampleCount; ++i )
   {
      value.c[0] = (*pSrc); pSrc++;
      value.c[1] = (*pSrc); pSrc++;
      value.c[2] = (*pSrc); pSrc++;
      value.c[3] = 0;

      /// Repair the minus-sign ( overflow makes highest bit set )
      if (value.i & 0x800000)
      {
         value.i |= ~0xffffff; // Zweierkomplement
      }

      if (value.i < 0)
      {
         *pDst = float(C64_INV_8388608 * value.i);
      }
      else
      {
         *pDst = float(C64_INV_8388607 * value.i);
      }

      pDst++;
   }
#endif

#if 0
   uint8_t* temp2 = (uint8_t *)temp;
   double conv = 1.0 / 8388608.0;
   for (int32_t i = 0; i < numElems; i ++)
   {
   int32_t value = *((int32_t*)temp2);
   value = _byteswap32(value) & 0x00ffffff;             // take 24 bits
   value |= (value & 0x00800000) ? 0xff000000 : 0;  // extend minus sign bits
   buffer[i] = (float)(value * conv);
   temp2 += 3;
   }
#endif

}

// ============================================================================
void SampleTypeConverter::converter_U24_to_F32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U24_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    uint8_t const* pSrc = reinterpret_cast< uint8_t const* >( src );
    float* pDst = reinterpret_cast< float* >( dst );

    MyS24 value;

    for ( size_t i = 0; i < sampleCount; ++i )
    {

        value.c[0] = (*pSrc); pSrc++;
        value.c[1] = (*pSrc); pSrc++;
        value.c[2] = (*pSrc); pSrc++;
        value.c[3] = 0;

        int_fast32_t const iValue = int_fast32_t( value.u ) - 8388608;

        if (iValue < 0)
        {
            *pDst = float(C64_INV_8388608 * double( iValue ) );
        }
        else
        {
            *pDst = float(C64_INV_8388607 * double( iValue ) );
        }

        pDst++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_S32_to_F32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S32_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    int32_t const* pSrc = reinterpret_cast< int32_t const* >( src );
    float* pDst = reinterpret_cast< float* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int_fast32_t const value = *pSrc;
        if (value < 0)
        {
            *pDst = float(C64_INV_2147483648 * double( value ) );
        }
        else
        {
            *pDst = float(C64_INV_2147483647 * double( value ) );
        }
        pSrc++;
        pDst++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_U32_to_F32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U32_to_F32(%x,%x,%d)\n", dst,src,sampleCount)

    float* pDst = reinterpret_cast< float* >( dst );
    uint32_t const* pSrc = reinterpret_cast< uint32_t const* >( src );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        uint32_t const value = *pSrc;
        int64_t const iValue = int64_t( value ) - 2147483648;

        if (iValue < 0)
        {
            *pDst = float( C64_INV_2147483648 * double(iValue) );
        }
        else
        {
            *pDst = float( C64_INV_2147483647 * double(iValue) );
        }

        pDst++;
        pSrc++;
    }
}

// hier muss logischerweise nix konvertiert werden, nur kopiert.
// ============================================================================
void SampleTypeConverter::converter_F32_to_F32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_F32_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    ::memcpy( dst, src, sampleCount * sizeof(float) );

//    float const* pSrc = reinterpret_cast< float const* >( src );
//    float* pDst = reinterpret_cast< float* >( dst );

//    for ( size_t i = 0; i < sampleCount; ++i )
//    {
//        *pDst = *pSrc;
//        pSrc++;
//        pDst++;
//    }
}

// ============================================================================
void SampleTypeConverter::converter_F64_to_F32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_F64_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    double const* pSrc = reinterpret_cast< double const* >( src );
    float* pDst = reinterpret_cast< float* >( dst );
    for ( size_t i = 0; i < sampleCount; ++i )
    {
        *pDst = static_cast< float >( *pSrc ); // read, convert and store value.
        pSrc++;
        pDst++;
    }
}

//// ============================================================================
//// ============================================================================
///
///     Converter to S32:
///
//// ============================================================================
//// ============================================================================
//void SampleTypeConverter::converter_S8_to_S32 ( void const* src, void* dst, size_t sampleCount );
//void SampleTypeConverter::converter_U8_to_S32 ( void const* src, void* dst, size_t sampleCount );
//void SampleTypeConverter::converter_S16_to_S32( void const* src, void* dst, size_t sampleCount );
//void SampleTypeConverter::converter_U16_to_S32( void const* src, void* dst, size_t sampleCount );
//void SampleTypeConverter::converter_S24_to_S32( void const* src, void* dst, size_t sampleCount );
//void SampleTypeConverter::converter_U24_to_S32( void const* src, void* dst, size_t sampleCount );
//void SampleTypeConverter::converter_S32_to_S32( void const* src, void* dst, size_t sampleCount );
//void SampleTypeConverter::converter_U32_to_S32( void const* src, void* dst, size_t sampleCount );
//void SampleTypeConverter::converter_F32_to_S32( void const* src, void* dst, size_t sampleCount ); // hier muss logischerweise nix konvertiert werden, nur kopiert.
//void SampleTypeConverter::converter_F64_to_S32( void const* src, void* dst, size_t sampleCount );

// ============================================================================
void SampleTypeConverter::converter_S8_to_S32 ( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S8_to_S32(%x,%x,%d)\n", dst,src,sampleCount)
   static_cast< void >( dst );
   static_cast< void >( src );
   static_cast< void >( sampleCount );
   DE_ERROR("Not implemented")
//    int32_t* pDst = reinterpret_cast< int32_t* >( dst );
//    int8_t const* pSrc = reinterpret_cast< int8_t const* >( src );

//    double const f1 = (double)std::numeric_limits<int32_t>::min() / (double)std::numeric_limits<char>::min();
//    double const f2 = (double)std::numeric_limits<int32_t>::max() / (double)std::numeric_limits<char>::max();

//    int64_t const min_int32_t = (int64_t)std::numeric_limits<int32_t>::min();
//    int64_t const max_int32_t = (int64_t)std::numeric_limits<int32_t>::max();

//    double fSample;
//    int64_t iSample;
//    for ( size_t i = 0; i < sampleCount; ++i )
//    {
//        int8_t const value = *pSrc;
//        if (value < 0)
//        {
//            fSample = f1*(double)value;
//            iSample = (int64_t)(fSample-0.5);
//            if (iSample < min_int32_t )
//            {
//                SampleConverterUnderflow++;
//                iSample = min_int32_t;
//            }
//        }
//        else
//        {
//            fSample = f2*(double)value;
//            iSample = (int64_t)(fSample+0.5);
//            if ( iSample > max_int32_t )
//            {
//                // SampleConverterOverflow++;
//                iSample = max_int32_t;
//            }
//        }
//        *pDst = (int32_t)iSample;
//        pDst++;
//        pSrc++;
//    }
}

// ============================================================================
void SampleTypeConverter::converter_U8_to_S32 ( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U8_to_S32(%x,%x,%d)\n", dst,src,sampleCount)
   static_cast< void >( dst );
   static_cast< void >( src );
   static_cast< void >( sampleCount );
   DE_ERROR("Not implemented")
//    int32_t* pDst = reinterpret_cast< int32_t* >( dst );
//    uint8_t const* pSrc = reinterpret_cast< uint8_t const* >( src );

//    double const f1 = (double)std::numeric_limits<int32_t>::min() / (double)std::numeric_limits<char>::min();
//    double const f2 = (double)std::numeric_limits<int32_t>::max() / (double)std::numeric_limits<char>::max();

//    int64_t const min_int32_t = (int64_t)std::numeric_limits<int32_t>::min();
//    int64_t const max_int32_t = (int64_t)std::numeric_limits<int32_t>::max();

//    double fSample;
//    int64_t iSample;
//    for ( size_t i = 0; i < sampleCount; ++i )
//    {
//        // const uint8_t original = *pSrc;
//        int32_t const value = ((int32_t)*pSrc) - 128;
//        if (value < 0)
//        {
//            fSample = f1*(double)value;
//            iSample = (int64_t)(fSample-0.5);
//            if (iSample < min_int32_t )
//            {
//                SampleConverterUnderflow++;
//                iSample = min_int32_t;
//            }
//        }
//        else
//        {
//            fSample = f2*(double)value;
//            iSample = (int64_t)(fSample+0.5);
//            if ( iSample > max_int32_t )
//            {
//                //SampleConverterOverflow++;
//                iSample = max_int32_t;
//            }
//        }
//        *pDst = (int32_t)iSample;
//        pDst++;
//        pSrc++;
//    }
}

// ============================================================================
void SampleTypeConverter::converter_S16_to_S32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S16_to_S32(%x,%x,%d)\n", dst,src,sampleCount)
   static_cast< void >( dst );
   static_cast< void >( src );
   static_cast< void >( sampleCount );
   DE_ERROR("Not implemented")
//    int32_t* pDst = reinterpret_cast< int32_t* >( dst );
//    int16_t const* pSrc = reinterpret_cast< int16_t const* >( src );

//    double const f1 = (double)std::numeric_limits<int32_t>::min() / (double)std::numeric_limits<int16_t>::min();
//    double const f2 = (double)std::numeric_limits<int32_t>::max() / (double)std::numeric_limits<int16_t>::max();

//    int64_t const min_int32_t = (int64_t)std::numeric_limits<int32_t>::min();
//    int64_t const max_int32_t = (int64_t)std::numeric_limits<int32_t>::max();

//    double fSample;
//    int64_t iSample;
//    for ( size_t i = 0; i < sampleCount; ++i )
//    {
//        int16_t const value = *pSrc;
//        if (value < 0)
//        {
//            fSample = f1*(double)value;
//            iSample = (int64_t)(fSample-0.5);
//            if (iSample < min_int32_t )
//            {
//                SampleConverterUnderflow++;
//                iSample = min_int32_t;
//            }
//        }
//        else
//        {
//            fSample = f2*(double)value;
//            iSample = (int64_t)(fSample+0.5);
//            if ( iSample > max_int32_t )
//            {
//                //SampleConverterOverflow++;
//                iSample = max_int32_t;
//            }
//        }
//        *pDst = (int32_t)iSample;
//        pDst++;
//        pSrc++;
//    }
}

// ============================================================================
void SampleTypeConverter::converter_U16_to_S32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U16_to_S32(%x,%x,%d)\n", dst,src,sampleCount)
   static_cast< void >( dst );
   static_cast< void >( src );
   static_cast< void >( sampleCount );
   DE_ERROR("Not implemented")
//    int32_t* pDst = reinterpret_cast< int32_t* >( dst );
//    uint16_t const* pSrc = reinterpret_cast< uint16_t const* >( src );

//    double const f1 = (double)std::numeric_limits<int32_t>::min() / (double)std::numeric_limits<short>::min();
//    double const f2 = (double)std::numeric_limits<int32_t>::max() / (double)std::numeric_limits<short>::max();

//    int64_t const min_int32_t = (int64_t)std::numeric_limits<int32_t>::min();
//    int64_t const max_int32_t = (int64_t)std::numeric_limits<int32_t>::max();

//    double fSample;
//    int64_t iSample;
//    for ( size_t i = 0; i < sampleCount; ++i )
//    {
//        // const uint8_t original = *pSrc;
//        int32_t const value = ((int32_t)*pSrc) + (int32_t)std::numeric_limits<short>::min();
//        if (value < 0)
//        {
//            fSample = f1*(double)value;
//            iSample = (int64_t)(fSample-0.5);
//            if (iSample < min_int32_t )
//            {
//                SampleConverterUnderflow++;
//                iSample = min_int32_t;
//            }
//        }
//        else
//        {
//            fSample = f2*(double)value;
//            iSample = (int64_t)(fSample+0.5);
//            if ( iSample > max_int32_t )
//            {
//                //SampleConverterOverflow++;
//                iSample = max_int32_t;
//            }
//        }
//        *pDst = (int32_t)iSample;
//        pDst++;
//        pSrc++;
//    }
}

// ============================================================================
void SampleTypeConverter::converter_S24_to_S32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S24_to_S32(%x,%x,%d)\n", dst,src,sampleCount)
   static_cast< void >( dst );
   static_cast< void >( src );
   static_cast< void >( sampleCount );
   DE_ERROR("Not implemented")
   //throw std::runtime_error("audio::converter_S24_to_S32() - Not implemented!");
//    //SampleConverterOverflow;
//    //SampleConverterUnderflow;
//    if (!dst || !src || sampleCount==0)
//    {
//        std::cout << "S8toS32: invalid use" << std::endl;
//        return;
//    }

//    int32_t* pDst = reinterpret_cast< int32_t* >( dst );
//    s24* pSrc = (s24*)src;

//    double const f1 = (double)std::numeric_limits<int32_t>::min() / (double)std::numeric_limits<int16_t>::min();
//    double const f2 = (double)std::numeric_limits<int32_t>::max() / (double)std::numeric_limits<int16_t>::max();

//    int64_t const min_int32_t = (int64_t)std::numeric_limits<int32_t>::min();
//    int64_t const max_int32_t = (int64_t)std::numeric_limits<int32_t>::max();

//    double fSample;
//    int64_t iSample;
//    for ( size_t i = 0; i < sampleCount; ++i )
//    {
//        int16_t const value = *pSrc;
//        if (value < 0)
//        {
//            fSample = f1*(double)value;
//            iSample = (int64_t)(fSample-0.5);
//            if (iSample < min_int32_t )
//            {
//                SampleConverterUnderflow++;
//                iSample = min_int32_t;
//            }
//        }
//        else
//        {
//            fSample = f2*(double)value;
//            iSample = (int64_t)(fSample+0.5);
//            if ( iSample > max_int32_t )
//            {
//                SampleConverterOverflow++;
//                iSample = max_int32_t;
//            }
//        }
//        pDst = (int32_t)iSample;
//        pDst++;
//        pSrc++;
//    }
}

// ============================================================================
void SampleTypeConverter::converter_U24_to_S32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U24_to_S32(%x,%x,%d)\n", dst,src,sampleCount)
   static_cast< void >( dst );
   static_cast< void >( src );
   static_cast< void >( sampleCount );
   DE_ERROR("Not implemented")
   //throw std::runtime_error("audio::converter_U24_to_S32() - Not implemented!");
//    //SampleConverterOverflow;
//    //SampleConverterUnderflow;
//    if (!dst || !src || sampleCount==0)
//    {
//        std::cout << "S8toS32: invalid use" << std::endl;
//        return;
//    }

//    int32_t* pDst = reinterpret_cast< int32_t* >( dst );
//    s24* pSrc = (s24*)src;

//    double const f1 = (double)std::numeric_limits<int32_t>::min() / (double)std::numeric_limits<int16_t>::min();
//    double const f2 = (double)std::numeric_limits<int32_t>::max() / (double)std::numeric_limits<int16_t>::max();

//    int64_t const min_int32_t = (int64_t)std::numeric_limits<int32_t>::min();
//    int64_t const max_int32_t = (int64_t)std::numeric_limits<int32_t>::max();

//    double fSample;
//    int64_t iSample;
//    for ( size_t i = 0; i < sampleCount; ++i )
//    {
//        int16_t const value = *pSrc;
//        if (value < 0)
//        {
//            fSample = f1*(double)value;
//            iSample = (int64_t)(fSample-0.5);
//            if (iSample < min_int32_t )
//            {
//                SampleConverterUnderflow++;
//                iSample = min_int32_t;
//            }
//        }
//        else
//        {
//            fSample = f2*(double)value;
//            iSample = (int64_t)(fSample+0.5);
//            if ( iSample > max_int32_t )
//            {
//                SampleConverterOverflow++;
//                iSample = max_int32_t;
//            }
//        }
//        pDst = (int32_t)iSample;
//        pDst++;
//        pSrc++;
//    }
}

// ============================================================================
void SampleTypeConverter::converter_S32_to_S32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S32_to_S32(%x,%x,%d)\n", dst,src,sampleCount)
   ::memcpy( dst, src, sampleCount * sizeof(int32_t) );
}

// ============================================================================
void SampleTypeConverter::converter_U32_to_S32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
   DE_ERROR("Not implemented")
   static_cast< void >( dst );
   static_cast< void >( src );
   static_cast< void >( sampleCount );
    // MMDebug("audio::converter_U32_to_S32(%x,%x,%d)\n", dst,src,sampleCount)

    //throw std::runtime_error("audio::converter_U32_to_S32() - Not implemented!");
//    //SampleConverterOverflow;
//    //SampleConverterUnderflow;
//    if (!dst || !src || sampleCount==0)
//    {
//        std::cout << "S8toS32: invalid use" << std::endl;
//        return;
//    }
//   memcpy( dst, src, sampleCount*sizeof(int32_t) );

}

// ============================================================================
void SampleTypeConverter::converter_F32_to_S32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_F32_to_S32(%x,%x,%d)\n", dst,src,sampleCount)
    float const* pSrc = reinterpret_cast< float const* >( src );
    int32_t* pDst = reinterpret_cast< int32_t* >( dst );

    double const f1 = double( std::abs(std::numeric_limits<int32_t>::min()) );
    double const f2 = double( std::numeric_limits<int32_t>::max() );

    int64_t const min_int32_t = int64_t( std::numeric_limits<int32_t>::min() );
    int64_t const max_int32_t = int64_t( std::numeric_limits<int32_t>::max() );

    double fSample;
    int64_t iSample;
    for ( size_t i = 0; i < sampleCount; ++i )
    {
        float const value = *pSrc;
        if (value < 0.0f)
        {
            fSample = f1 * double( value );
            iSample = int64_t( fSample - 0.5 );
            if (iSample < min_int32_t )
            {
                // SampleConverterUnderflow++;
                iSample = min_int32_t;
            }
        }
        else
        {
            fSample = f2 * double( value );
            iSample = int64_t( fSample + 0.5 );
            if ( iSample > max_int32_t )
            {
                //SampleConverterOverflow++;
                iSample = max_int32_t;
            }
        }
        *pDst = int32_t( iSample );
        pDst++;
        pSrc++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_F64_to_S32( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_F64_to_S32(%x,%x,%d)\n", dst,src,sampleCount)
    double const* pSrc = reinterpret_cast< double const* >( src );
    int32_t* pDst = reinterpret_cast< int32_t* >( dst );

    double const f1 = double( std::abs(std::numeric_limits<int32_t>::min()) );
    double const f2 = double( std::numeric_limits<int32_t>::max() );

    int64_t const min_int32_t = int64_t( std::numeric_limits<int32_t>::min() );
    int64_t const max_int32_t = int64_t( std::numeric_limits<int32_t>::max() );

    double fSample;
    int64_t iSample;
    for ( size_t i = 0; i < sampleCount; ++i )
    {
        double const value = *pSrc;
        if (value < 0.0)
        {
            fSample = f1 * double( value );
            iSample = int64_t( fSample - 0.5 );
            if (iSample < min_int32_t )
            {
                // SampleConverterUnderflow++;
                iSample = min_int32_t;
            }
        }
        else
        {
            fSample = f2 * double( value );
            iSample = int64_t( fSample + 0.5 );
            if ( iSample > max_int32_t )
            {
                //SampleConverterOverflow++;
                iSample = max_int32_t;
            }
        }
        *pDst = int32_t( iSample );
        pDst++;
        pSrc++;
    }
}

///
/// S24
///

// ============================================================================
void SampleTypeConverter::converter_S8_to_S24( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
   static_cast< void >( dst );
   static_cast< void >( src );
   static_cast< void >( sampleCount );
   DE_ERROR("Not implemented")
   // throw std::runtime_error( "converter_S8_to_S24() used but !!!");
}

// ============================================================================
void SampleTypeConverter::converter_S16_to_S24 ( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S16_to_S24(%x,%x,%d)\n", dst,src,sampleCount)
    int16_t const* pSrc = reinterpret_cast< int16_t const* >( src );
    uint8_t* pDst = reinterpret_cast< uint8_t* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int32_t k = *pSrc++;
        k = s32_clamp( k << 8, -8388608, 8388607 );
        s24 s = k;
        *pDst++ = s.r;
        *pDst++ = s.g;
        *pDst++ = s.b;
    }
}

// ============================================================================
void SampleTypeConverter::converter_S24_to_S24( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    ::memcpy( dst, src, sampleCount * 3 ); // 3 Bytes per Sample ( signed s24 )
}

// ============================================================================
void SampleTypeConverter::converter_F32_to_S24 ( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    float const* pSrc = reinterpret_cast< float const* >( src );
    uint8_t* pDst = reinterpret_cast< uint8_t* >( dst );

    // s24 value;
    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int32_t k;
        float f = *pSrc++;
        if ( f < 0.0f )
            k = s32_clamp( int32_t( 8388608.0f*f + 0.5f ), -8388608, -1 );
        else
            k = s32_clamp( int32_t( 8388607.0f*f + 0.5f ), 0, 8388607 );

        s24 s = k;
        *pDst++ = s.r;
        *pDst++ = s.g;
        *pDst++ = s.b;
    }
}

///
/// F64
///

// ============================================================================
void SampleTypeConverter::converter_S8_to_F64 ( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S8_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    int8_t const* pSrc = reinterpret_cast< int8_t const* >( src );
    double* pDst = reinterpret_cast< double* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int8_t const value = *pSrc;
        if (value < 0)
        {
            *pDst = C64_INV_128 * value;
        }
        else
        {
            *pDst = C64_INV_127 * value;
        }

        pDst++;
        pSrc++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_U8_to_F64 ( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U8_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    double* pDst = reinterpret_cast< double* >( dst );
    uint8_t const* pSrc = reinterpret_cast< uint8_t const* >( src );
    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int16_t const value = int16_t( *pSrc ) - 128;
        if (value < 0)
        {
            *pDst = C64_INV_128 * value;
        }
        else
        {
            *pDst = C64_INV_127 * value;
        }
        pDst++;
        pSrc++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_S16_to_F64( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S16_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    int16_t const* pSrc = reinterpret_cast< int16_t const* >( src );
    double* pDst = reinterpret_cast< double* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int16_t const value = *pSrc;
        if (value < 0)
        {
            *pDst = C64_INV_32768 * value;
        }
        else
        {
            *pDst = C64_INV_32768 * value;
        }
        pDst++;
        pSrc++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_U16_to_F64( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U16_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    uint16_t const* pSrc = reinterpret_cast< uint16_t const* >( src );
    double* pDst = reinterpret_cast< double* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int_fast32_t const value = int_fast32_t(*pSrc) - 32768;
        if (value < 0)
        {
            *pDst = C64_INV_32768 * value;
        }
        else
        {
            *pDst = C64_INV_32767 * value;
        }

        pDst++;
        pSrc++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_S24_to_F64( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S24_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    uint8_t const* pSrc = reinterpret_cast< uint8_t const* >( src );
    double* pDst = reinterpret_cast< double* >( dst );

    MyS24 value;
    for ( size_t i = 0; i < sampleCount; ++i )
    {
        value.c[0] = (*pSrc); pSrc++;
        value.c[1] = (*pSrc); pSrc++;
        value.c[2] = (*pSrc); pSrc++;
        value.c[3] = 0;

        /// Repair the minus-sign ( overflow makes highest bit set )
        if (value.i & 0x800000)
        {
            value.i |= ~0xffffff; // Zweierkomplement
        }

        if (value.i < 0)
        {
            *pDst = C64_INV_8388608 * double( value.i );
        }
        else
        {
            *pDst = C64_INV_8388607 * double( value.i );
        }

        pDst++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_U24_to_F64( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U24_to_F32(%x,%x,%d)\n", dst,src,sampleCount)

    double* pDst = reinterpret_cast< double* >( dst );
    uint8_t const* pSrc = reinterpret_cast< uint8_t const* >( src );

    MyS24 value;

    for ( size_t i = 0; i < sampleCount; ++i )
    {

        value.c[0] = (*pSrc); pSrc++;
        value.c[1] = (*pSrc); pSrc++;
        value.c[2] = (*pSrc); pSrc++;
        value.c[3] = 0;

        int32_t const iValue = int32_t( value.u ) - 8388608;

        if (iValue < 0)
        {
            *pDst = C64_INV_8388608 * double( iValue );
        }
        else
        {
            *pDst = C64_INV_8388607 * double( iValue );
        }

        pDst++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_S32_to_F64( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_S32_to_F32(%x,%x,%d)\n", dst,src,sampleCount)

    double* pDst = reinterpret_cast< double* >( dst );
    int32_t const* pSrc = reinterpret_cast< int32_t const* >( src );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        int32_t const value = *pSrc;
        if (value < 0)
        {
            *pDst = C64_INV_2147483648 * double( value );
        }
        else
        {
            *pDst = C64_INV_2147483647 * double( value );
        }

        pDst++;
        pSrc++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_U32_to_F64( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_U32_to_F32(%x,%x,%d)\n", dst,src,sampleCount)

    double* pDst = reinterpret_cast< double* >( dst );
    uint32_t const* pSrc = reinterpret_cast< uint32_t const* >( src );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        uint32_t const value = *pSrc;
        int64_t const iValue = int64_t(value) - 2147483648;

        if (iValue < 0)
        {
            *pDst = C64_INV_2147483648 * double( iValue );
        }
        else
        {
            *pDst = C64_INV_2147483647 * double( iValue );
        }

        pDst++;
        pSrc++;
    }
}

// hier muss logischerweise nix konvertiert werden, nur kopiert.
// ============================================================================
void SampleTypeConverter::converter_F32_to_F64( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_F32_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    float const* pSrc = reinterpret_cast< float const* >( src );
    double* pDst = reinterpret_cast< double* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        *pDst = double( *pSrc );
        pSrc++;
        pDst++;
    }
}

// ============================================================================
void SampleTypeConverter::converter_F64_to_F64( void const* src, void* dst, size_t sampleCount )
// ============================================================================
{
    // MMDebug("audio::converter_F64_to_F32(%x,%x,%d)\n", dst,src,sampleCount)
    double const* pSrc = reinterpret_cast< double const* >( src );
    double* pDst = reinterpret_cast< double* >( dst );

    for ( size_t i = 0; i < sampleCount; ++i )
    {
        *pDst = *pSrc;
        pDst++;
        pSrc++;
    }
}



} // end namespace audio
} // end namespace de
