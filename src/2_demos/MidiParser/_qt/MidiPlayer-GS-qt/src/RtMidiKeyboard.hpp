/*
 *  RtMidiKeyboard.hpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */
#pragma once
#include <RtMidiUtil.hpp>
#include <cstdint>
#include <algorithm>

struct RtMidiKeyboard
{
   std::string m_client_name;
   uint32_t 	m_port_out;
   RtMidiOut*  m_rtm_out;

   std::vector< uint8_t > m_midiOutMessageBuffer;

   RtMidiKeyboard()
      : m_client_name("RtMidiKeyboard")
      , m_port_out(0)
      , m_rtm_out(nullptr)
   {
      RtMidiUtil::printMidiIn();
      RtMidiUtil::printMidiOut();
      //m_midiInBuffer.clear();
      //m_midiInBuffer.reserve( 1024 );

      m_midiOutMessageBuffer.reserve( 32 );

      //openInputPort();
      openOutput();
   }

   ~RtMidiKeyboard()
   {
      closeOutput();
   }

   void openOutput()
   {
      if ( m_rtm_out ) { return; } // Already open
      m_rtm_out = RtMidiUtil::openMidiOut( 0 );
   }

   void closeOutput()
   {
      if ( !m_rtm_out ) { return; } // Already closed
      delete m_rtm_out;
      m_rtm_out = nullptr;
   }


   void sendMessage( std::vector< uint8_t > const & message )
   {
      if ( !m_rtm_out ) return;
      m_midiOutMessageBuffer.clear();
      m_midiOutMessageBuffer.assign( message.begin(), message.end() );
      m_rtm_out->sendMessage( &m_midiOutMessageBuffer );
   }

   void sendShortMessage( int command, int channel, int midiNote, int velocity )
   {
      if ( !m_rtm_out ) return;
      uint8_t status = (uint8_t(command) & 0xF0) | (uint8_t(channel) & 0x0F);
      uint8_t data1 = uint8_t(midiNote);
      uint8_t data2 = uint8_t(velocity);
      uint32_t packet = (uint32_t(data2) << 16) | (uint32_t(data1) << 8) | uint32_t(status);
      m_rtm_out->sendShortMessage( packet );
   }

   // Bn Control Change | Bn cc xx, cc = controller (0-121) xx = value (0-127)
   // Bn Channel Mode Local Ctl  Bn 7A xx xx = 0 (off), 127 (on)
   // Bn All Notes Off           Bn 7B 00
   // Bn Omni Mode Off           Bn 7C 00
   // Bn Omni Mode On            Bn 7D 00
   // Bn Mono Mode On            Bn 7E cc cc = number of channels
   // Bn Poly Mode On            Bn 7F 00

   void sendAllSoundOff()
   {
      for ( int i = 0; i < 16; ++i )
      {
         sendShortMessage( 0xB0, i, 0x78, 0 );
      }
   }

   void sendAllNotesOff()
   {
      for ( int i = 0; i < 16; ++i )
      {
         sendShortMessage( 0xB0, i, 0x7B, 0 );
      }
   }

   void sendAllSoundOff( int channel )
   {
      sendShortMessage( 0xB0, channel, 0x78, 0 );
   }

   void sendAllNotesOff( int channel )
   {
      sendShortMessage( 0xB0, channel, 0x7B, 0 );
   }

   void sendNoteOn( int channel = 0, int midiNote = 69, int velocity = 127 )
   {
      sendShortMessage( 0x90, channel, midiNote, velocity );
   }

   void sendNoteOff( int channel = 0, int midiNote = 69, int velocity = 127 )
   {
      sendShortMessage( 0x80, channel, midiNote, velocity );
   }

   /*
   static void
   Callback_RtMidiInput( double deltaTime, std::vector< uint8_t >* message, void* userData )
   {
      RtMidiKeyboard* driver = reinterpret_cast<RtMidiKeyboard*>( userData );
      if ( driver && message )
      {
         auto const byteCount = message->size();
         auto & buffer = *message;

      #if defined(BENNI_DEBUG_MIDI)
         std::ostringstream o;
         o << "deltaTime(" << deltaTime << "), byteCount(" << byteCount << ")";

         for ( size_t i = 0; i < byteCount; i++ )
         {
            o << " 0x" << StringUtil::hexStr( buffer[ i ] );
         }

         printf("%s\n", o.str().c_str() ); fflush(stdout);
      #endif

         if ( byteCount == 3 )
         {
            u8 status = buffer[0];
            u8 command = status & 0xF0;
            u8 channel = status & 0x0F;
            u8 data1 = buffer[1];   // midiNote, cc
            u8 data2 = buffer[2];   // velocity, value

            if ( command == 0x80 ) // NoteOff
            {
               amsynth_short_midi_event_t sme;
               sme.channel = channel;
               sme.command = command;
               sme.midiNote = data1;
               sme.velocity = data2;

               driver->m_midiInBuffer.emplace_back( std::move(sme) );
            }
            else if ( command == 0x90 ) // NoteOn
            {
               amsynth_short_midi_event_t sme;
               sme.channel = channel;
               sme.command = command;
               sme.midiNote = data1;
               sme.velocity = data2;

               driver->m_midiInBuffer.emplace_back( std::move(sme) );
            }
            else if ( command == 0xB0 ) // ControlChange
            {
               amsynth_short_midi_event_t sme;
               sme.channel = channel;
               sme.command = command;
               sme.midiNote = data1;
               sme.velocity = data2;

               driver->m_midiInBuffer.emplace_back( std::move(sme) );
            }
         }

         auto beg = message->begin();
         auto end = message->end();
         while ( beg != end )
         {
            beg++;
         }
      }
   }

   void closeInputPort()
   {
      if ( m_rtm_in )
      {
         delete m_rtm_in;
         m_rtm_in = nullptr;
      }
   }

   bool openInputPort()
   {
      closeInputPort();

      Configuration & config = Configuration::get();

      try
      {
         m_rtm_in = new RtMidiIn();

         uint32_t nPorts = m_rtm_in->getPortCount();
         if ( nPorts > m_port_in )
         {
            m_rtm_in->openPort( m_port_in );

            m_rtm_in->setCallback( &Callback_RtMidiInput, this );

            // Don't ignore sysex, timing, or active sensing messages.
            m_rtm_in->ignoreTypes( false, false, false );

//            std::this_thread::sleep_for( std::chrono::milliseconds(100) );

//            bool ok = m_rtm_in->isPortOpen();

//            if ( !ok )
//            {
//               printf("RtMidiKeyboard.openInputPort() :: Port not open, abort\n" ); fflush(stdout);
//               closeInputPort();
//            }

            return true;
         }
         else
         {
            closeInputPort();
            return false;
         }
      }
      catch ( RtMidiError & e )
      {
         closeInputPort();
         printf("RtMidiKeyboard.openInputPort() :: Got error: %s\n", e.getMessage().c_str() );
         fflush(stdout);
         return false;
      }
   }

   */
};
