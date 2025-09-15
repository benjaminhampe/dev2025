#pragma once
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <vector>
#include <DarkImage.h>
#include "GeneralMidi.h"

namespace de {

// SMF Syntax - Standard Midi file syntax. ( Aufs Byte genau leider )
// <descriptor:length> means 'length' bytes, MSB first
// <descriptor:v> means variable length argument (special format)
// SMF := <header_chunk> + <track_chunk> [ + <track_chunk> ... ]
// header chunk := "MThd" + <header_length:4> + <format:2> + <num_tracks:2> + <time_division:2>
// track_chunk := "MTrk" + <length:4> + <track_event> [ + <track_event> ... ]
// track_event := <time:v> + [ <midi_event> | <meta_event> | <sysex_event> ]
// midi_event := any MIDI channel message, including running status
// meta_event := 0xFF + <meta_type:1> + <length:v> + <event_data_bytes>
// sysex_event := 0xF0 + <len:1> + <data_bytes> + 0xF7
// sysex_event := 0xF7 + <len:1> + <data_bytes> + 0xF7

// MidiNotes: 12x SemiTones per Oktave [C to H], 8x Oktaven [0 to 7] = 96 Toene
// =======================================================================
//     0       1       2       3       4       5       6       7
// =======================================================================
// C   16,35   32,70   65,41   130,81  261,63  523,25  1046,50 2093,00  Hz
// C’  17,32   34,65   69,30   138,59  277,18  554,37  1108,74 2217,46  Hz
// D   18,35   36,71   73,42   146,83  293,66  587,33  1174,66 2349,32  Hz
// D’  19,45   38,89   77,78   155,56  311,13  622,25  1244,51 2489,02  Hz
// E   20,60   41,20   82,41   164,81  329,63  659,26  1328,51 2637,02  Hz
// F   21,83   43,65   87,31   174,61  349,23  698,46  1396,91 2793,83  Hz
// F’  23,12   46,25   92,50   185,00  369,99  739,99  1479,98 2959,96  Hz
// G   24,50   49,00   98,00   196,00  392,00  783,99  1567,98 3135,96  Hz
// G’  25,96   51,91   103,83  207,65  415,30  830,61  1661,22 3322,44  Hz
// A   27,50   55,00   110,00  220,00  440,00  880,00  1760,00 3520,00  Hz
// A’  29,14   58,27   116,54  233,08  466,16  923,33  1864,66 3729,31  Hz
// H   30,87   61,74   123,47  246,94  493,88  987,77  1975,53 3951,07  Hz

// =======================================================================
struct MidiUtil
// =======================================================================
{
   DE_CREATE_LOGGER("de.MidiUtil")

   static void
   test()
   {
      // NUMBER   VARIABLE LENGTH QUANTITY
      // 00000000 00
      // 00000040 40
      // 0000007F 7F
      // 00000080 81 00
      // 00002000 C0 00
      // 00003FFF FF 7F
      // 00004000 81 80 00
      // 00100000 C0 80 00
      // 001FFFFF FF FF 7F
      // 00200000 81 80 80 00
      // 08000000 C0 80 80 00
      // 0FFFFFFF FF FF FF 7F

      testVLQ( 1, { 0x00 }, 1, 0 );
      testVLQ( 2, { 0x40 }, 1, 64 );
      testVLQ( 3, { 0x7F }, 1, 127 );

      testVLQ( 4, { 0x81, 0x00 }, 2, 0x80 ); // 128
      testVLQ( 5, { 0xC0, 0x00 }, 2, 0x2000 ); // 8*1024
      testVLQ( 6, { 0xFF, 0x7F }, 2, 0x3FFF );

      testVLQ( 7, { 0x81, 0x80, 0x00 }, 3, 0x00004000 );
      testVLQ( 8, { 0xC0, 0x80, 0x00 }, 3, 0x00100000 );
      testVLQ( 9, { 0xFF, 0xFF, 0x7F }, 3, 0x001FFFFF );

      testVLQ( 10, { 0x81, 0x80, 0x80, 0x00 }, 4, 0x00200000 );
      testVLQ( 11, { 0xC0, 0x80, 0x80, 0x00 }, 4, 0x08000000 );
      testVLQ( 12, { 0xFF, 0xFF, 0xFF, 0x7F }, 4, 0x0FFFFFFF );

      // Test with one more byte
      testVLQ( 13, { 0x00, 0x00 }, 1, 0 );
      testVLQ( 14, { 0x40, 0x00 }, 1, 64 );
      testVLQ( 15, { 0x7F, 0x00 }, 1, 127 );

      testVLQ( 16, { 0x81, 0x00, 0x00 }, 2, 0x80 ); // 128
      testVLQ( 17, { 0xC0, 0x00, 0x00 }, 2, 0x2000 ); // 8*1024
      testVLQ( 18, { 0xFF, 0x7F, 0x00 }, 2, 0x3FFF );

      testVLQ( 19, { 0x81, 0x80, 0x00, 0x00 }, 3, 0x00004000 );
      testVLQ( 20, { 0xC0, 0x80, 0x00, 0x00 }, 3, 0x00100000 );
      testVLQ( 21, { 0xFF, 0xFF, 0x7F, 0x00 }, 3, 0x001FFFFF );

      testVLQ( 22, { 0x81, 0x80, 0x80, 0x00, 0x00 }, 4, 0x00200000 );
      testVLQ( 23, { 0xC0, 0x80, 0x80, 0x00, 0x00 }, 4, 0x08000000 );
      testVLQ( 24, { 0xFF, 0xFF, 0xFF, 0x7F, 0x00 }, 4, 0x0FFFFFFF );
   }

   static void
   testVLQ( int const k, std::vector< uint8_t > const & bv, size_t const expectedBytes, uint32_t const expectedValue )
   {
      uint8_t const* beg = bv.data();
      uint8_t const* end = beg + bv.size();
      uint32_t returnedValue = 0;
      size_t const returnedBytes = parseVLQ( beg, end, returnedValue );

      std::ostringstream o;
      o << "testVLQ[" << k << "] :: Data(" << bv.size() << ")";
      o << "[";
      for ( size_t i = 0; i < bv.size(); ++i )
      {
         if ( i > 0 )
         {
            o << ", ";
         }
         o << "0x" << StringUtil::hex( bv[i] );

      }
      o << "] ";
      o << "expect( " << expectedBytes << "B, " << StringUtil::hex(expectedValue) << " = " << expectedValue << " ), ";
      o << "return( " << returnedBytes << "B, " << StringUtil::hex(returnedValue) << " = " << returnedValue << " ), ";

      bool ok = true;
      ok &= (expectedBytes == returnedBytes);
      ok &= (expectedValue == returnedValue);

      if ( ok )
      {
         o << " OK";
         DE_INFO( o.str() )
      }
      else
      {
         o << " FAIL";
         throw std::runtime_error( o.str() );
      }
   }

   //
   // VLQ - Variable-Length Quantity
   //

   static size_t
   parseVLQ( uint8_t const* const beg, uint8_t const* const end, uint32_t & value )
   {
      if ( !beg ) { return 0; }
      if ( !end ) { return 0; }
      if ( beg == end ) { return 0; }
      auto ptr = beg; // Copy pointer and work with it, keep original pointer for final delta.

      uint8_t c = *ptr++;     // Read 1st byte, hopefully the last.
      uint32_t r = c & 0x7F;  // Extract value part from 'c' into 'r'.
      while ( c >= 0x80 && ( ptr != end ) )
      {
         c = *ptr++;      // Read next byte, hopefully the last.
         r = ( r << 7 ) | ( 0x7F & c );   // Extract value part from 'c' into 'r'.
      }

      //DE_DEBUG("[",hexStr(uint64_t(ptr)),"] Parsed VLQ = ",retVal," after ",n," bytes")
      value = r;
      return static_cast<size_t>(ptr - beg);
   }

   // MIDI specific read function ( is it BIG ENDIAN ? )
   static size_t
   readU16_be( uint8_t const* const beg, uint8_t const* const end, uint16_t & value )
   {
      if ( !beg ) { return 0; }  // this check makes it noexcept/robust, but slower.
      if ( !end ) { return 0; }  // this check makes it noexcept/robust, but slower.
      if ( end - beg < 2 ) { return 0; }
      auto ptr = beg;
      uint_fast16_t r = *ptr++;
      uint_fast16_t g = *ptr++;
      value = uint_fast16_t( (r << 8) | g );
      return static_cast<size_t>(ptr - beg);
   }

   // MIDI specific read function ( is it BIG ENDIAN ? )
   static size_t
   readU32_be( uint8_t const* const beg, uint8_t const* const end, uint32_t & value )
   {
      if ( !beg ) { return 0; }  // this check makes it/robust, but slower.
      if ( !end ) { return 0; }  // this check makes it/robust, but slower.
      if ( end - beg < 4 ) { return 0; }
      auto ptr = beg;
      uint_fast32_t r = *ptr++;
      uint_fast32_t g = *ptr++;
      uint_fast32_t b = *ptr++;
      uint_fast32_t a = *ptr++;
      value = (r << 24) | (g << 8) | (b << 8) | a;
      return static_cast<size_t>(ptr - beg);
   }

/*
   static size_t
   readU16( uint8_t const* beg, uint8_t const* end, uint16_t & value )
   {
      if ( !beg ) { return 0; }
      if ( !end ) { return 0; }
      if ( end - beg < 2 ) { return 0; }
      auto ptr = beg;
      uint_fast16_t r = *ptr++;
      uint_fast16_t g = *ptr++;
      value = uint_fast16_t( (g << 8) | r );
      return static_cast<size_t>(ptr - beg);
   }

   static size_t
   readU32( uint8_t const* beg, uint8_t const* end, uint32_t & value )
   {
      if ( !beg ) { return 0; }
      if ( !end ) { return 0; }
      if ( end - beg < 4 ) { return 0; }
      auto ptr = beg;
      uint_fast32_t r = *ptr++;
      uint_fast32_t g = *ptr++;
      uint_fast32_t b = *ptr++;
      uint_fast32_t a = *ptr++;
      value = uint_fast32_t((a << 24) | (b << 8) | (g << 8) | r);
      return static_cast<size_t>(ptr - beg);
   }
*/

   static void
   decomposeNote( int midiNote, int & semitone, int & octave )
   {
      octave = midiNote / 12;
      semitone = midiNote - (octave * 12);
   }

   static bool
   isBlackPianoKey( int midiNote )
   {
      int semi = 0;
      int octave = 0;
      decomposeNote( midiNote, semi, octave );

      if ( semi == 1 // C = 0, C# = 1,
        || semi == 3 // D = 2, D# = 3, E = 4,
        || semi == 6 // F = 5, F# = 6,
        || semi == 8 // G = 7, G# = 8,
        || semi == 10)//A = 9, A# = 10, H = 11
      {
         return true; // black piano key
      }
      return false; // white piano key
   }

   static std::string
   getNoteStr( int midiNote, bool printOctave = true )
   {
      int semiTone = 0;
      int octave = 0;
      decomposeNote( midiNote, semiTone, octave );

      std::ostringstream o;

      switch ( semiTone )
      {
         case  0: o << "C";  break;
         case  1: o << "C#"; break;
         case  2: o << "D";  break;
         case  3: o << "D#"; break;
         case  4: o << "E";  break;
         case  5: o << "F";  break;
         case  6: o << "F#"; break;
         case  7: o << "G";  break;
         case  8: o << "G#"; break;
         case  9: o << "A";  break;
         case 10: o << "A#"; break;
         case 11: o << "B";  break;
      }

      if ( printOctave )
      {
         o << octave;
      }

      return o.str();
   }

   static uint32_t
   getNoteColor( int midiNote )
   {
      int semi = midiNote % 12;
      switch ( semi )
      {
         case  0: return 0xFF0000FF; // C
         case  1: return 0xFF00F07F; // C#
         case  2: return 0xFFF0F07F; // D
         case  3: return 0xFFF0707F; // D#
         case  4: return 0xFFF0007F; // E
         case  5: return 0xFFF07FF0; // F
         case  6: return 0xFF00A00F; // F#
         case  7: return 0xFFA0007F; // G
         case  8: return 0xFF20802F; // G#
         case  9: return 0xFF70B04F; // A
         case 10: return 0xFFC06010; // A#
         case 11: return 0xFFA0406F; // B;
         default: return 0xFF000000;
      }
   }

   //
   // f( midiNote ) = 220Hz * 2 ^ [(midiNote-69)/12)]
   // f( oktave, semi ) = 440Hz * 2 ^ [(oktave-4)+(semi-9)/12)]
   //
   // MIDI stuff:
   // get frequency from piano key ( C = 0, H = 11 for A4 use getFrequencyFromNote(9,4) = 440 Hz )
   //inline float
   //getFrequencyFromNote( int C_is_zero, int oktave )
   //{
   //   return 440.f * powf( 2.f, (float)( (C_is_zero%12) + (12*oktave) - 69) / 12.f );
   //}

   static int
   getMidiNoteFromFrequency( float frequency )
   {
      //f = 440.0f * powf( 2.0f, (midiNote - 69.0f) * (1.0f/12.0f) );
      //f / 440.0f = 2^((midiNote - 69.0f) / 12.0f));
      // log2(f / 440.0f) = log2(2^((midiNote - 69.0f) / 12.0f)));
      // log2(f / 440.0f) = (midiNote - 69.0f) / 12.0f;
      int midiNote = int(std::roundf( 12.f * log2f( frequency / 440.0f ) )) + 69;
      return midiNote;
   }

   static float
   getFrequencyFromMidiNote( float midiNote )
   {
      // 440Hz = A4 = 12 * 5 + 9 = 69  -> should be 440 * 2^0;
      return 440.0f * powf( 2.0f, (midiNote - 69.0f) * (1.0f/12.0f) );
   //   int semi = midiNote - 12 * oktave;
   //   //DEM_DEBUG("midiNote(",midiNote,") = oktave(",oktave,"),semi(",semi,")")
   //   return getFrequencyFromNote( oktave, semi, detune );
   }

   static float
   getFrequencyFromNote( int oktave, float semitone_C_is_zero )
   {
      // 440Hz = A4 = 12 * 5 + 9 = 69;
      return getFrequencyFromMidiNote( 12.0f*oktave + semitone_C_is_zero );
   //   int semi = midiNote - 12 * oktave;
   //   //DEM_DEBUG("midiNote(",midiNote,") = oktave(",oktave,"),semi(",semi,")")
   //   return getFrequencyFromNote( oktave, semi, detune );
   }


   static float
   volume_to_dB( float const volume )
   {
      return 20.0f * ::log10f( volume );
   }

   static float
   dB_to_volume( float const dB )
   {
      return ::powf( 10.0f, 0.05f * dB );
   }

   static float
   clampf( float value, float min_value, float max_value)
   {
      return std::min( std::max( value, min_value), max_value );
   }

   static double
   clampd( double value, double min_value, double max_value)
   {
      return std::min( std::max( value, min_value), max_value );
   }

   static uint32_t
   getChannelColor( int channel )
   {
      switch ( channel )
      {
         case 0: return 0xFF0000FF;    // red
         case 1: return 0xFF0080FF;    // orange
         case 2: return 0xFF00AF00;
         case 3: return 0xFFFFAF00;
         case 4: return 0xFFAF0000;
         case 5: return 0xFFFF0040;
         case 6: return 0xFF00FFFF;
         case 7: return 0xFFFFFFFF;
         case 8: return 0xFFFFC08F;
         case 9: return 0xFF000000;    // Drums = black
         case 10: return 0xFFFF00FF;
         case 11: return 0xFF00FF00;
         case 12: return 0xFF8F008F;
         case 13: return 0xFF3FF03F;
         case 14: return 0xFF7F40DF;
         case 15: return 0xFF3F808F;
         default: return 0xFFFFFFFF;
      }
   }

   //
   // Convert a MIDI keynote to a string
   //

   static int
   computePixelOffsetY( int midiNote, int lineHeight )
   {
      int const semiTone = midiNote % 12;
      switch ( semiTone )
      {
         case  0: return 0;   // C
         case  1: return 0;   // C#
         case  2: return int( 0.5f * lineHeight ); // D
         case  3: return int( 0.5f * lineHeight ); // D#
         case  4: return lineHeight; // E
         case  5: return int( 1.5f * lineHeight ); // F
         case  6: return int( 1.5f * lineHeight ); // F#
         case  7: return int( 2.0f * lineHeight ); // G
         case  8: return int( 2.0f * lineHeight ); // G#
         case  9: return int( 2.5f * lineHeight ); // A
         case 10: return int( 3.0f * lineHeight ); // b
         case 11: return int( 3.0f * lineHeight ); // H
         default: return 0;
      }
   }

   static int
   powi( int base, int exponent )
   {
      if ( exponent == 0 ) return 1;

      int k = base;
      for ( int i = 1; i < exponent; ++i )
      {
         k *= base;
      }
      return k;
   }

/*
   static int
   findActualEventLength( uint8_t const* data, int maxBytes )
   {
      auto byte = uint32_t( *data );

      if (byte == 0xf0 || byte == 0xf7)
      {
         int i = 1;

         while ( i < maxBytes )
         {
            if ( data[ i++ ] == 0xf7 )
            {
               break;
            }
         }

         return i;
      }

      if (byte == 0xff)
      {
         if ( maxBytes == 1 ) return 1;

         int value = readVLQ( data + 1, maxBytes - 1, nullptr );
         return std::min( maxBytes, value + 2 );
      }

      if (byte >= 0x80)
         return std::min( maxBytes, getMessageLengthFromFirstByte ((uint8) byte));

      return 0;
   }

*/

};

} // end namespace de
