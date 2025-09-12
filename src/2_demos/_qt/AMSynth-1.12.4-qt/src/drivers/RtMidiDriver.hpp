/*
 *  RtMidiDriver.hpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */
#pragma once
#include "de_types.hpp"
#include "config.h"
#include "Configuration.h"
#include <RtMidi/RtMidi.h>
#include <sstream>
#include <thread>

struct RtMidiDriver
{
   std::string m_client_name;
   u32 			m_port_in;
   u32			m_port_out;
   RtMidiIn*   m_rtm_in;
   RtMidiOut*  m_rtm_out;

   std::vector< amsynth_short_midi_event_t > m_midiInBuffer;

   RtMidiDriver()
      : m_client_name("AMSynthMidiDriver")
      , m_port_in(0)
      , m_port_out(0)
      , m_rtm_in(nullptr)
      , m_rtm_out(nullptr)
   {
      std::vector< std::string > const midiInPorts = enumerateMidiIn();

      printf("enumerateMidiIn() :: Port.Count = %d\n", int(midiInPorts.size()) );
      for ( size_t i = 0; i < midiInPorts.size(); ++i )
      {
         printf("enumerateMidiIn() :: Port[%d] %s\n", int(i), midiInPorts[i].c_str() );

      }
      fflush( stdout );

      m_midiInBuffer.clear();
      m_midiInBuffer.reserve( 1024 );

      openInputPort();

   }

   ~RtMidiDriver()
   {
      closeInputPort();
   }

   static std::vector< std::string >
   enumerateMidiIn()
   {
      std::vector< std::string > portNames;

      RtMidiIn* rtm = nullptr;
      try
      {
         rtm = new RtMidiIn( RtMidi::WINDOWS_MM, "RtMidiDriverInputEnum" );

         u32 const nPorts = rtm->getPortCount();
         if ( nPorts )
         {
            portNames.reserve( nPorts );

            for ( u32 i = 0; i < nPorts; i++ )
            {
               std::string portName = rtm->getPortName( i );
               portNames.emplace_back( std::move( portName ) );
            }
         }

         if ( rtm )
         {
            delete rtm;
            rtm = nullptr;
         }

      }
      catch ( RtMidiError & e )
      {
         printf("enumerateMidiIn() :: RtMidiException: %s\n", e.getMessage().c_str() );
         fflush( stdout );

         if ( rtm )
         {
            delete rtm;
            rtm = nullptr;
         }
      }

      return portNames;
   }


   static void
   Callback_RtMidiInput( double deltaTime, std::vector< uint8_t >* message, void* userData )
   {
      RtMidiDriver* driver = reinterpret_cast<RtMidiDriver*>( userData );
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

            if ( command == 0x90 ) // NoteOn
            {
               amsynth_short_midi_event_t sme;
               sme.channel = channel;
               sme.command = command;
               sme.midiNote = data1;
               sme.velocity = data2;

               driver->m_midiInBuffer.emplace_back( std::move(sme) );
            }
            else if ( command == 0x80 ) // NoteOff
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
//               printf("RtMidiDriver.openInputPort() :: Port not open, abort\n" ); fflush(stdout);
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
         printf("RtMidiDriver.openInputPort() :: Got error: %s\n", e.getMessage().c_str() );
         fflush(stdout);
         return false;
      }
   }


   int read( u8* buffer, u32 maxBytes )
   {
   /*
      if (seq_handle == nullptr) {
         return 0;
      }
      unsigned char *ptr = buffer;
      while (1) {
         snd_seq_event_t *ev = nullptr;
         int res = snd_seq_event_input(seq_handle, &ev);
         if (res < 0)
            break;
         ptr += snd_midi_event_decode(seq_midi_parser, ptr, maxBytes - (ptr - buffer), ev);
         if (res < 1)
            break;
      }
      return (int)(ptr - buffer);
   */
      return 0;
   }
   int write_cc( u32 channel, u32 param, u32 value )
   {
   /*
      if (seq_handle == nullptr) {
         return 0;
      }
      int ret=0;
      snd_seq_event_t ev;


      snd_seq_ev_clear(&ev);
      snd_seq_ev_set_subs(&ev);
      snd_seq_ev_set_direct(&ev);
      snd_seq_ev_set_source(&ev, portid_out);
      ev.type = SND_SEQ_EVENT_CONTROLLER;
      ev.data.control.channel = channel;
      ev.data.control.param = param;
      ev.data.control.value = value;
      ret=snd_seq_event_output_direct(seq_handle, &ev);
   #if _DEBUG
      cout << "param = " << param << " value = " << value << " ret = " << ret << endl;
   #endif
      if (ret < 0 ) cout << snd_strerror(ret) << endl;
      return ret;
   */
      return 0;
   }



};
