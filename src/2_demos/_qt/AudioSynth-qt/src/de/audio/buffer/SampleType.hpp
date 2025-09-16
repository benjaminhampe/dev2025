#pragma once
#include <DarkImage.h>

namespace de {
namespace audio {

// MIDI stuff:
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
//
// f( midiNote ) = 220Hz * 2 ^ [(midiNote-69)/12)]
// f( oktave, semi ) = 440Hz * 2 ^ [(oktave-4)+(semi-9)/12)]
//
float getFrequencyFromNote( int oktave, float semitone_C_is_zero );
float getFrequencyFromMidiNote( float midiNote );

float volume_to_dB( float const volume );
float dB_to_volume( float const dB );

float clampf( float value, float min_value, float max_value);
double clampd( double value, double min_value, double max_value);

// ===========================================================================
struct Api
// ===========================================================================
{
   enum EApi
   {
      AUTO_SELECT = 0, WIN_WASAPI, WIN_ASIO, WIN_DSOUND,
      LINUX_ALSA, LINUX_PULSE, LINUX_OSS, WAV_WRITER,
      EApiCount,
   };

   static std::string
   toString( EApi api )
   {
      switch (api)
      {
         case AUTO_SELECT: return "AUTO_SELECT";
         case WIN_WASAPI: return "WASAPI";
         case WIN_ASIO: return "ASIO";
         case WIN_DSOUND: return "DSOUND";
         case LINUX_ALSA: return "ALSA";
         case LINUX_PULSE: return "PULSE";
         case LINUX_OSS: return "OSS";
         case WAV_WRITER: return "WAV_WRITER";
         default: return "Unknown";
      }
   }
   static EApi
   parseString( std::string api )
   {
      dbStrLowerCase( api );
      if ( api == "wasapi" ) return WIN_WASAPI;
      else if ( api == "ds" ) return WIN_DSOUND;
      else if ( api == "dsound" ) return WIN_DSOUND;
      else if ( api == "asio" ) return WIN_ASIO;
      else return AUTO_SELECT;
   }
};

// ============================================================================
struct SampleRates
// ============================================================================
{
   static std::vector< int32_t > const &
   getDefaultSampleRates()
   {
      static std::vector< int32_t > const s_Rates = {
         4000,   5512,  8000,  9600, 11025,  12000,  16000,  22050, 24000,
         32000, 44100, 48000, 88200, 96000, 176400, 192000, 352800, 384000
      };
      return s_Rates;
   }

   static bool
   contains( int32_t rate )
   {
      for ( size_t i = 0; i < getDefaultSampleRates().size(); ++i )
      {
         if ( getDefaultSampleRates()[i] == rate )
         return true;
      }

      return false;
   }
};

// Oktaven und Frequenzen
//    0       1       2       3       4       5       6       7
//C   16,35   32,70   65,41   130,81  261,63  523,25  1046,50 2093,00
//C’  17,32   34,65   69,30   138,59  277,18  554,37  1108,74 2217,46
//D   18,35   36,71   73,42   146,83  293,66  587,33  1174,66 2349,32
//D’  19,45   38,89   77,78   155,56  311,13  622,25  1244,51 2489,02
//E   20,60   41,20   82,41   164,81  329,63  659,26  1328,51 2637,02
//F   21,83   43,65   87,31   174,61  349,23  698,46  1396,91 2793,83
//F’  23,12   46,25   92,50   185,00  369,99  739,99  1479,98 2959,96
//G   24,50   49,00   98,00   196,00  392,00  783,99  1567,98 3135,96
//G’  25,96   51,91   103,83  207,65  415,30  830,61  1661,22 3322,44
//A   27,50   55,00   110,00  220,00  440,00  880,00  1760,00 3520,00
//A’  29,14   58,27   116,54  233,08  466,16  923,33  1864,66 3729,31
//H   30,87   61,74   123,47  246,94  493,88  987,77  1975,53 3951,07

/*
/// causes memory BUG, dont use, but i really dont know why
/// i guess something already went wrong at allocating input and output
/// so its maybe not the function itself
// ===========================================================================
bool ApplyHammingWindow( f64* input, f64* output, u32 sample_count )
// ===========================================================================
{
   if (!input)
      return false;

   if (!output)
      return false;

   if (sample_count == 0)
      return false;

   for (u32 i=0; i<sample_count; i++)
   {
      f64 n = -0.5f*sample_count + i;
      f64 f = 0.54 + 0.46*cos( 2.0*core::PI64*n / sample_count );
      f64 tmp = input[i] * f;
      output[i] = tmp;
   }

   return true;
}
*/

// This Benni enum should cover all sample types u will need.
// Its the common denumerator of libs such as libAVCodec, libSndfile, libFMOD, mpg123 and others.
// ============================================================================
enum ESampleType : uint16_t // Yay, its composed of bitflags...
// ============================================================================
{
   ST_Unknown =  0,  // 0 - means no flag
   ST_Interleaved = 1, // 1-interleaved (ch0,ch1,ch0,ch1) or 0-planar (ch0,ch0,ch1,ch1) data
   ST_Signed =   1<<1, // for integer formats only
   ST_Int =      1<<2, // 0 = float, 1 = integer format.
   ST_8Bit =     1<<3, //
   ST_16Bit =    1<<4, // only one of these 1-6
   ST_24Bit =    1<<5, // flags may be set of course
   ST_32Bit =    1<<6, // setting more than one, then probably the lowest wins, so not good.
   ST_64Bit =    1<<7, //
   ST_128Bit =   1<<8, // TODO: Do we need 128 bits per sample?
   ST_256Bit =   1<<9, // TODO: Do we need 256 bits per sample?

   ST_S8 = ST_8Bit | ST_Int | ST_Signed,
   ST_S8P = ST_S8,
   ST_S8I = ST_S8 | ST_Interleaved,

   ST_U8 = ST_Int | ST_8Bit,
   ST_U8P = ST_U8,
   ST_U8I = ST_U8 | ST_Interleaved,

   // mostly used on consumer Audio-CDs and MP3s ( default )
   ST_S16 = ST_Int | ST_Signed | ST_16Bit,
   ST_S16P = ST_S16,
   ST_S16I = ST_S16 | ST_Interleaved,

   // mostly used for quality audio-studio-material .flac
   ST_S24 = ST_Int | ST_Signed | ST_24Bit,
   ST_S24P = ST_S24,
   ST_S24I = ST_S24 | ST_Interleaved,

   // mostly used for quality audio-studio-material .flac, libsndfile, ffmpeg
   ST_S32 = ST_32Bit | ST_Int | ST_Signed,
   ST_S32P = ST_S32,
   ST_S32I = ST_S32 | ST_Interleaved,

   // used for libavCodec
   ST_F16 = ST_16Bit,
   ST_F16P = ST_F16,
   ST_F16I = ST_F16 | ST_Interleaved,

   // used for internal calculations of an audio engine or as common intermmediate highp format.
   ST_F32 = ST_32Bit,
   ST_F32P = ST_F32,
   ST_F32I = ST_F32 | ST_Interleaved,

   // used in ffmpeg, unlikely to encounter
   ST_S64 = ST_Signed | ST_Int | ST_64Bit,
   ST_S64P = ST_S64,
   ST_S64I = ST_S64 | ST_Interleaved,

   // used for internal calculations of an audio engine or as common intermmediate highp format.
   ST_F64 = ST_64Bit,
   ST_F64P = ST_F64,
   ST_F64I = ST_F64 | ST_Interleaved,

   // libMpgHelper
   ST_U16 = ST_Int | ST_16Bit,
   ST_U16P = ST_U16,
   ST_U16I = ST_U16 | ST_Interleaved,

   ST_U24 = ST_Int | ST_24Bit,
   ST_U24P = ST_U24,
   ST_U24I = ST_U24 | ST_Interleaved,

   ST_U32 = ST_Int | ST_32Bit,
   ST_U32P = ST_U32,
   ST_U32I = ST_U32 | ST_Interleaved,

   // More for very exotic stuff found in libsndfile, speech encoding, very low bandwidth.
   ST_ULAW =     1<<10,
   ST_ALAW =     1<<11,
   ST_ADPCM =    1<<12,
   ST_MS_ADPCM = 1<<13,
   //ST_ULAW_8 = ST_Int | ST_Signed | ST_ULAW | ST_8Bit | ST_Interleaved,
   //ST_ALAW_8 = ST_Int | ST_Signed | ST_ALAW | ST_8Bit | ST_Interleaved,
   ESampleTypeCount = 14
   // ST_ForceAtleast16Bit = 0x7FFF
};

inline ESampleType
ST_makeInterleaved( ESampleType type )
{
   return ESampleType( uint32_t( type ) | ST_Interleaved );
}

// ============================================================================
inline ESampleType
ST_build( bool bIsInt, bool bIsSigned, uint16_t bitsPerSample, bool interleaved = true )
{
   uint_fast16_t st = 0;
   if ( interleaved ) st |= ST_Interleaved;
   if ( bIsInt ) { st |= ST_Int; }    // Datentyp
   if ( bIsSigned )  { st |= ST_Signed; }// Hat der Datentyp ein Vorzeichen
   if      (bitsPerSample == 8)    { st |= ST_8Bit; }
   else if (bitsPerSample == 16)   { st |= ST_16Bit; }
   else if (bitsPerSample == 24)   { st |= ST_24Bit; }
   else if (bitsPerSample == 32)   { st |= ST_32Bit; }
   else if (bitsPerSample == 64)   { st |= ST_64Bit; }
   else { return ST_Unknown; }
   return ESampleType( st & 0xFFFF );
}

// ============================================================================
inline uint32_t
ST_getBitsPerSample( ESampleType sampleType )
{
   uint32_t const st( sampleType );
   if      ( st & ST_16Bit ) return 16;
   else if ( st & ST_8Bit )  return 8;
   else if ( st & ST_24Bit ) return 24;
   else if ( st & ST_32Bit ) return 32;
   else if ( st & ST_64Bit ) return 64;
   else return 0;
}

// ============================================================================
inline uint32_t
ST_getBytesPerSample( ESampleType sampleType )
{
   uint32_t bps = ST_getBitsPerSample( sampleType );
   if ( bps >= 8 )   return ( bps >> 3 );
   else              return 0;
}

// ============================================================================
inline bool ST_contains( ESampleType sampleType, ESampleType searchFlags )
{
   return ( size_t( sampleType ) & size_t( searchFlags ) ) == size_t( searchFlags );
}

// ============================================================================
inline bool ST_isUnknown   ( ESampleType sampleType ) { return 0==sampleType; }
inline bool ST_isInterleaved(ESampleType sampleType ) { return ST_Interleaved == (sampleType & ST_Interleaved);   }
inline bool ST_isPlanar    ( ESampleType sampleType ) { return !ST_isInterleaved( sampleType );    }
inline bool ST_isSigned    ( ESampleType sampleType ) { return ST_Signed == (sampleType & ST_Signed); }
inline bool ST_isUnsigned  ( ESampleType sampleType ) { return !ST_isSigned( sampleType );         }
inline bool ST_isInteger   ( ESampleType sampleType ) { return ST_Int == (sampleType & ST_Int);    }
inline bool ST_isFloat     ( ESampleType sampleType ) { return !ST_isInteger( sampleType );        }
inline bool ST_is8Bit      ( ESampleType sampleType ) { return ST_8Bit ==(sampleType & ST_8Bit);   }
inline bool ST_is16Bit     ( ESampleType sampleType ) { return ST_16Bit ==(sampleType & ST_16Bit); }
inline bool ST_is24Bit     ( ESampleType sampleType ) { return ST_24Bit ==(sampleType & ST_24Bit); }
inline bool ST_is32Bit     ( ESampleType sampleType ) { return ST_32Bit ==(sampleType & ST_32Bit); }
inline bool ST_is64Bit     ( ESampleType sampleType ) { return ST_64Bit ==(sampleType & ST_64Bit); }

bool
ST_Validate( ESampleType sampleType );

// ============================================================================
inline std::string
ST_toString( ESampleType sampleType )
{
   ST_Validate( sampleType );

   if ( ST_isUnknown(sampleType) ) return "ST_Unknown";

   std::stringstream s;
   s << "ST_";
   if ( ST_isInteger(sampleType) )
   {
      if ( ST_isSigned(sampleType) ) { s << "S"; }
      else { s << "U"; }
   }
   else
   {
      s << "F";
   }
        if (ST_is8Bit(sampleType)) s << "8";
   else if (ST_is16Bit(sampleType)) s << "16";
   else if (ST_is24Bit(sampleType)) s << "24";
   else if (ST_is32Bit(sampleType)) s << "32";
   else if (ST_is64Bit(sampleType)) s << "64";

   if ( ST_isInterleaved(sampleType) ) s << "I";
   else s << "P";

   return s.str();
}



/**

   This module defines a new arithmetic type - s24.

   The s24 should be stay packed in memory or space req drops to full 32bit int.

   It is meant for 24bit audio samples.

   Probably not uptodate anymore regarding cheap float audio engines.
*/



constexpr int const S24_MAX = (1<<23)-1;
constexpr int const S24_MIN = -S24_MAX-1;

#pragma pack(push, 1)

// used in SampleType converter
struct MyS24
{
   union
   {
   uint8_t c[4];
   int32_t i;
   uint32_t u;
   };
};

// Class for signed 24-bit integer
struct s24
{
   uint8_t r;
   uint8_t g;
   uint8_t b; // b stores sign of number as unsigned, reconstructed in operator int()

   s24() : r(0), g(0), b(0)
   {}

   s24( int32_t val )
   {
      operator=( val );
   }

   s24& operator=( int32_t val )
   {
      r = (uint8_t)val;       // is that correct ?
      g = (uint8_t)(val>>8);  // is that correct ?
      b = (uint8_t)(val>>16); // is that correct ?
      return *this;
   }

   s24& operator=( s24 const & other )
   {
      if ( &other==this ) { return *this; }
      r = other.r; g = other.g; b = other.b;
      return *this;
   }

   // overloading this operator is the magic mostly.
   // Now class can compute with it and then convert back to s24.
   operator int32_t() const
   {
       return (r) | (g<<8) | (b<<16) | (b&0x80 ? (1<<24) : 0); // looks correct

        // return (r) | (g<<8) | (b<<16) | (b&0x80 ? (-1<<24) : 0); // looks correct
   }

   s24& operator++ ()            { operator=(*this+1); return *this; }
   s24 operator++ (int32_t)      { s24 t = *this; operator=(*this+1); return t; }
   s24& operator-- ()            { operator=(*this+1); return *this;}
   s24 operator-- (int32_t)      { s24 t = *this; operator=(*this+1); return t; }
   s24& operator+= (int32_t rhs) { operator=(*this + rhs); return *this; }
   s24& operator-= (int32_t rhs) { operator=(*this - rhs); return *this; }
   s24& operator*= (int32_t rhs) { operator=(*this * rhs); return *this; }
   s24& operator/= (int32_t rhs) { operator=(*this / rhs); return *this; }
   s24& operator%= (int32_t rhs) { operator=(*this % rhs); return *this; }
   s24& operator>>= (int32_t rhs){ operator=(*this >> rhs); return *this; }
   s24& operator<<= (int32_t rhs){ operator=(*this << rhs); return *this; }
   s24& operator|= (int32_t rhs) { operator=(*this | rhs); return *this; }
   s24& operator&= (int32_t rhs) { operator=(*this & rhs); return *this; }
   s24& operator^= (int32_t rhs) { operator=(*this ^ rhs); return *this; }
   s24& operator|=( s24 rhs )    { r |= rhs.r; g |= rhs.g; b |= rhs.b; return *this; }
   s24& operator&=( s24 rhs )    { r &= rhs.r; g &= rhs.g; b &= rhs.b; return *this; }
   s24& operator^=( s24 rhs )    { r ^= rhs.r; g ^= rhs.g; b ^= rhs.b; return *this; }
};
#pragma pack(pop)


struct SampleTypeConverter
{
   DE_CREATE_LOGGER("de.audio.SampleTypeConverter")
   ///
   /// Convert between arbitrary sample-types.
   ///
   typedef void (*Converter_t) (void const*, void*, uint64_t);

   ///
   /// Use this function to determine the correct converter
   ///
   static Converter_t
   getConverter( ESampleType src, ESampleType dst );

   ///
   /// Converts to S16
   ///
   static void converter_S8_to_S16 ( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U8_to_S16 ( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S16_to_S16( void const* src, void* dst, uint64_t sampleCount ); // hier muss logischerweise nix konvertiert werden, nur kopiert.
   static void converter_U16_to_S16( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S24_to_S16( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U24_to_S16( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S32_to_S16( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U32_to_S16( void const* src, void* dst, uint64_t sampleCount );
   static void converter_F32_to_S16( void const* src, void* dst, uint64_t sampleCount );
   static void converter_F64_to_S16( void const* src, void* dst, uint64_t sampleCount );
   ///
   /// Converts to S24
   ///
   static void converter_S8_to_S24( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S16_to_S24 ( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S24_to_S24( void const* src, void* dst, uint64_t sampleCount );
   static void converter_F32_to_S24 ( void const* src, void* dst, uint64_t sampleCount );
   ///
   /// Converts to F32
   ///
   static void converter_S8_to_F32 ( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U8_to_F32 ( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S16_to_F32( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U16_to_F32( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S24_to_F32( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U24_to_F32( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S32_to_F32( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U32_to_F32( void const* src, void* dst, uint64_t sampleCount );
   static void converter_F32_to_F32( void const* src, void* dst, uint64_t sampleCount ); // hier muss logischerweise nix konvertiert werden, nur kopiert.
   static void converter_F64_to_F32( void const* src, void* dst, uint64_t sampleCount );

   ///
   /// Converts to F64 - New: 19.03.2016 For statistical analysis in audio.Channel
   ///
   static void converter_S8_to_F64 ( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U8_to_F64 ( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S16_to_F64( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U16_to_F64( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S24_to_F64( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U24_to_F64( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S32_to_F64( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U32_to_F64( void const* src, void* dst, uint64_t sampleCount );
   static void converter_F32_to_F64( void const* src, void* dst, uint64_t sampleCount ); // hier muss logischerweise nix konvertiert werden, nur kopiert.
   static void converter_F64_to_F64( void const* src, void* dst, uint64_t sampleCount );

   ///
   /// Converts to S32
   ///
   static void converter_S8_to_S32 ( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U8_to_S32 ( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S16_to_S32( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U16_to_S32( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S24_to_S32( void const* src, void* dst, uint64_t sampleCount );
   static void converter_U24_to_S32( void const* src, void* dst, uint64_t sampleCount );
   static void converter_S32_to_S32( void const* src, void* dst, uint64_t sampleCount ); // hier muss logischerweise nix konvertiert werden, nur kopiert.
   static void converter_U32_to_S32( void const* src, void* dst, uint64_t sampleCount );
   static void converter_F32_to_S32( void const* src, void* dst, uint64_t sampleCount );
   static void converter_F64_to_S32( void const* src, void* dst, uint64_t sampleCount );

   ///
   /// Converts to F32_sse
   ///
   //static void converter_S8_to_F32_sse ( void const* src, void* dst, uint64_t sampleCount );
   //static void converter_U8_to_F32_sse ( void const* src, void* dst, uint64_t sampleCount );
   //static void converter_S16_to_F32_sse( void const* src, void* dst, uint64_t sampleCount );
   //static void converter_U16_to_F32_sse( void const* src, void* dst, uint64_t sampleCount );
   //static void converter_S24_to_F32_sse( void const* src, void* dst, uint64_t sampleCount );
   //static void converter_U24_to_F32_sse( void const* src, void* dst, uint64_t sampleCount );
   //static void converter_S32_to_F32_sse( void const* src, void* dst, uint64_t sampleCount );
   //static void converter_U32_to_F32_sse( void const* src, void* dst, uint64_t sampleCount );
   //static void converter_F32_to_F32_sse( void const* src, void* dst, uint64_t sampleCount );
   //static void converter_F64_to_F32_sse( void const* src, void* dst, uint64_t sampleCount );

};




///// src in range [-2^15,2^15-1]
///// dst in range [-1,1]
//static void convertSampleS16tofloat( const s16& src, float& out )
//{
//    if (src<0)
//        out = INV_MIN_S16 * src;
//    else
//        out = INV_MAX_S16 * src;
//}

///// src in range [-2^23,2^23-1]
///// dst in range [-1,1]
//static void convertSampleS24tofloat( const uint32_t& src, float& out )
//{
//    if (src<0)
//    {
//        out = INV_MIN_S24 * (src & 0x00FFFFFF);
//    }
//    else
//    {
//        out = INV_MAX_S24 * (src & 0x00FFFFFF);
//    }
//}

///// src in range [-2^31,2^31-1]
///// dst in range [-1,1]
//static void convertSampleuint32_ttofloat( const uint32_t& src, float& out )
//{
//    if (src<0)
//    {
//        out = INV_MIN_uint32_t * src;
//    }
//    else
//    {
//        out = INV_MAX_uint32_t * src;
//    }
//}

///// src in range [-1,1]
///// dst in range [-2^15,2^15-1]
//static void convertSamplefloattoS16( float src, s16& out, bool& overflow, bool& underflow )
//{
//    if (src > 1.0f)
//    {
//        src = 1.0f;
//        overflow = true;
//    }
//    else if (src < -1.0f)
//    {
//        src = -1.0f;
//        underflow = true;
//    }

//    if (src<0.0f)
//        out = (s16)dbClampInt( dbRound32(src * 32768.0f), -32768, 0);
//    else
//        out = (s16)dbClampInt( dbRound32(src * 32767.0f), 0, 32767);
//}


// ============================================================================
struct WaveFormat64
// ============================================================================
{
   DE_CREATE_LOGGER("de.audio.WaveFormat64")
   ESampleType m_SampleType;  // 16-bit
   uint16_t m_ChannelCount;   // 16-bit
   float m_SampleRate;        // 32-bit
   //uint64_t m_FrameCount;     // 64-bit
                              // = 128-bit per WaveFormat64
public:
   static WaveFormat64
   createStereo16_44_1kHz() { return WaveFormat64( ST_S16, 2, 44100.0 ); }

   static WaveFormat64
   createStereo16_48kHz() { return WaveFormat64( ST_S16, 2, 48000.0 ); }

   static WaveFormat64
   createStereo32_48kHz() { return WaveFormat64( ST_F32, 2, 48000.0 ); }

   WaveFormat64()
      : m_SampleType( ST_Unknown )
      , m_ChannelCount( 0 )
      , m_SampleRate( 0.0f )
      //, m_FrameCount( 0L )
   {}

   WaveFormat64( ESampleType sampleType, int channels, float sampleRate )
      : m_SampleType( sampleType )
      , m_ChannelCount( uint16_t( channels ) )
      , m_SampleRate( sampleRate )
      //, m_FrameCount( frames )
   {}

   std::string
   toString() const
   {
      std::stringstream s;
      s << ST_toString( m_SampleType )
        << "*" << m_ChannelCount
        << "*" << int(0.1f * int(0.01f*m_SampleRate)) << "kHz";
        //<< "*" << m_FrameCount;
        //<< "|Duration:" << dbSecondsToString( getDuration() );
      return s.str();
   }

   bool isChannel( uint32_t channel ) const
   {
      if ( channel >= m_ChannelCount )
      {
         DE_ERROR("Invalid channel ", channel, " of ", m_ChannelCount )
         return false;
      }
      return true;
   }

   bool isValid() const
   {
      return ST_getBitsPerSample( m_SampleType ) > 0
             && m_ChannelCount > 0
             && m_SampleRate > 0.f;
   }

   //bool isEmpty() const { return !isValidFormat() || getFrameCount() < 1; }
   void setFormat( ESampleType sampleType, int channels, float sampleRate )
   {
      setSampleType( sampleType );
      setChannelCount( channels );
      setSampleRate( sampleRate );
   }
   void setSampleType( ESampleType sampleType ) { m_SampleType = sampleType; }
   void setChannelCount( int channels )         { m_ChannelCount = uint16_t( channels ); }
   void setSampleRate( float sampleRate )       { m_SampleRate = sampleRate; }


   ESampleType getSampleType() const { return m_SampleType; }
   uint32_t getChannelCount() const { return m_ChannelCount; }
   float getSampleRate() const { return m_SampleRate; }
   //uint64_t getFrameCount() const { return m_FrameCount; }
   uint32_t getBitsPerSample() const { return ST_getBitsPerSample( m_SampleType ); }
   uint32_t getBytesPerSample() const { return ST_getBytesPerSample( m_SampleType ); }

   //uint64_t getSampleCount() const { return getFrameCount() * getChannelCount(); }
   //uint64_t getByteCount() const { return getSampleCount() * getBytesPerSample(); }
   bool isInterleaved() const { return ST_isInterleaved( m_SampleType ); }
   double getBitRate() const
   {
      return double( m_SampleRate ) * getBytesPerSample() / 1024.0;
   }

   uint32_t getByteSize() const { return getBytesPerSample() * getChannelCount(); }

   uint32_t getFrameSize() const // in [bytes]
   {
      uint32_t bps = getBytesPerSample();
      return isInterleaved() ? (bps * getChannelCount()) : bps;
   }

   // Equality-Operator
   bool operator== ( WaveFormat64 const & other ) const
   {
      if ( m_SampleType != other.m_SampleType ) return false;
      if ( m_ChannelCount != other.m_ChannelCount ) return false;
      if ( m_SampleRate != other.m_SampleRate ) return false;
      //if ( m_FrameCount != other.m_FrameCount ) return false;
      return true;
   }
   // Inequality-Operator
   bool operator!= ( WaveFormat64 const & other ) const
   {
      return !(*this == other); /// calls operator== and negates the result
   }
   // Copy-Operator
   WaveFormat64& operator= ( WaveFormat64 const & other )
   {
      if ( this == &other ) { return *this; }
      m_SampleType = other.m_SampleType;
      m_ChannelCount = other.m_ChannelCount;
      m_SampleRate = other.m_SampleRate;
      //m_FrameCount = other.m_FrameCount;
      return *this;
   }

   // f64 getDuration() const
   // {
      // if (m_SampleRate <= 0)
      // {
         // return 0.0;
      // }
      // else
      // {
         // return (double)m_UsedFrames / (double)m_SampleRate;
      // }
   // }
//   double getDuration() const
//   {
//      if ( std::abs( m_SampleRate ) < 0.0001f ) { return 0.0; }
//      else { return double( m_FrameCount ) / double( m_SampleRate ); }
//   }
//   uint64_t getFrameIndex( double t_in_ms ) const
//   {
//      if ( m_ChannelCount < 1 || m_FrameCount < 1 ) return 0;
//      uint64_t frameIndex = uint64_t( ( t_in_ms * double( m_SampleRate ) ) + 0.5 );
//      frameIndex -= ( frameIndex % m_ChannelCount );
//      return frameIndex;
//   }
//   uint64_t getSampleIndex( double t_in_ms, uint32_t channelIndex ) const
//   {
//      return getFrameIndex( t_in_ms ) + channelIndex;
//   }
};


} // end namespace audio
} // end namespace de

