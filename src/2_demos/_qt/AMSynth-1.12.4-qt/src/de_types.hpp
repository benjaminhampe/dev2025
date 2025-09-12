/*
 *  de_types.hpp
 *
 *  Copyright (c) 2001-2012 Nick Dowell
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is part of amsynth.
 *
 *  amsynth is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  amsynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with amsynth.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <cstdint>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef long double f80;


//#include "gettext.h"
//#define _(string) gettext (string)

//#include <iostream>
#include <sstream>
#include <string>
#include <climits>
#include <csignal>
#include <cstring>
#include <algorithm>

#ifdef _DEBUG
#define DEBUGMSG( ... ) fprintf( stderr, __VA_ARGS__ )
#else
#define DEBUGMSG( ... )
#endif

struct amsynth_short_midi_event_t
{
   u8 command = 0;
   u8 channel = 0;
   u8 midiNote = 0;
   u8 velocity = 0;
};

struct amsynth_midi_event_t
{
   u32 offset_frames = 0;
   u32 length = 0;
   u8 *buffer = nullptr;
};

struct amsynth_midi_cc_t
{
   u8 channel = 0;
   u8 cc = 0;
   u8 value = 0;
};


enum {
    MIDI_STATUS_NOTE_OFF                = 0x80,
    MIDI_STATUS_NOTE_ON                 = 0x90,
    MIDI_STATUS_NOTE_PRESSURE           = 0xA0,
    MIDI_STATUS_CONTROLLER              = 0xB0,
    MIDI_STATUS_PROGRAM_CHANGE          = 0xC0,
    MIDI_STATUS_CHANNEL_PRESSURE        = 0xD0,
    MIDI_STATUS_PITCH_WHEEL             = 0xE0,
};

/* https://www.midi.org/specifications/item/table-3-control-change-messages-data-bytes-2
 */
enum {
    MIDI_CC_BANK_SELECT_MSB             = 0x00,
    MIDI_CC_MODULATION_WHEEL_MSB        = 0x01,
    MIDI_CC_DATA_ENTRY_MSB              = 0x06,
    MIDI_CC_PAN_MSB                     = 0x0A,

    MIDI_CC_BANK_SELECT_LSB             = 0x20,
    MIDI_CC_DATA_ENTRY_LSB              = 0x26,

    MIDI_CC_SUSTAIN_PEDAL               = 0x40,
    MIDI_CC_PORTAMENTO                  = 0x41,
    MIDI_CC_SOSTENUTO                   = 0x42,
    MIDI_CC_NRPN_LSB                    = 0x62,
    MIDI_CC_NRPN_MSB                    = 0x63,
    MIDI_CC_RPN_LSB                     = 0x64,
    MIDI_CC_RPN_MSB                     = 0x65,

    /* ------- Channel Mode Messages ------- */
    MIDI_CC_ALL_SOUND_OFF               = 0x78,
    MIDI_CC_RESET_ALL_CONTROLLERS       = 0x79,
    MIDI_CC_LOCAL_CONTROL               = 0x7A,
    MIDI_CC_ALL_NOTES_OFF               = 0x7B,
    MIDI_CC_OMNI_MODE_OFF               = 0x7C, /* + all notes off            */
    MIDI_CC_OMNI_MODE_ON                = 0x7D, /* + all notes off            */
    MIDI_CC_MONO_MODE_ON                = 0x7E, /* + poly off + all notes off */
    MIDI_CC_POLY_MODE_ON                = 0x7F, /* + mono off + all notes off */
};


struct StringUtil
{

   static char
   hexStrLowNibble( uint8_t byte )
   {
      uint_fast8_t const lowbyteNibble = byte & 0x0F;
      if ( lowbyteNibble < 10 )
      {
         return char('0' + lowbyteNibble);
      }
      else
      {
         return char('A' + (lowbyteNibble-10));
      }
   }

   static char
   hexStrHighNibble( uint8_t byte )
   {
      return hexStrLowNibble( byte >> 4 );
   }

   static std::string
   hexStr( uint8_t byte )
   {
      std::ostringstream o;
      o << hexStrHighNibble( byte ) << hexStrLowNibble( byte );
      return o.str();
   }

   static std::string
   hexStr( uint16_t const color )
   {
      uint8_t const r = color & 0xFF;
      uint8_t const g = ( color >> 8 ) & 0xFF;
      std::ostringstream o;
      o << hexStr( g ) << hexStr( r );
      return o.str();
   }

   static std::string
   hexStr( u32 const color )
   {
      uint8_t const r = color & 0xFF;
      uint8_t const g = ( color >> 8 ) & 0xFF;
      uint8_t const b = ( color >> 16 ) & 0xFF;
      uint8_t const a = ( color >> 24 ) & 0xFF;
      std::ostringstream o;
      o << hexStr( a ) << hexStr( b ) << hexStr( g ) << hexStr( r );
      return o.str();
   }

   static std::string
   hexStr( u64 color )
   {
      uint8_t const r = color & 0xFF;
      uint8_t const g = ( color >> 8 ) & 0xFF;
      uint8_t const b = ( color >> 16 ) & 0xFF;
      uint8_t const a = ( color >> 24 ) & 0xFF;
      uint8_t const x = ( color >> 32 ) & 0xFF;
      uint8_t const y = ( color >> 40 ) & 0xFF;
      uint8_t const z = ( color >> 48 ) & 0xFF;
      uint8_t const w = ( color >> 56 ) & 0xFF;
      std::ostringstream o;
      o << hexStr( w ) << hexStr( z ) << hexStr( y ) << hexStr( x );
      o << hexStr( a ) << hexStr( b ) << hexStr( g ) << hexStr( r );
      return o.str();
   }

/*
   static std::string
   hexStr( uint8_t const* beg, uint8_t const* end )
   {
      std::ostringstream o;
      auto ptr = beg;
      while ( ptr < end )
      {
         if ( ptr > beg ) o << ", ";
         o << "0x" << hexStr( *ptr );
         ptr++;
      }
      return o.str();
   }

   static std::string
   hexStr( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow )
   {
      size_t i = 0;
      std::ostringstream o;
      auto ptr = beg;
      while ( ptr < end )
      {
         i++;
         o << "0x" << hexStr( *ptr++ );
         if ( ptr < end ) o << ", ";
         if ( i >= nBytesPerRow )
         {
            i -= nBytesPerRow;
            o << "\n";
         }
      }
      return o.str();
   }
*/
};
