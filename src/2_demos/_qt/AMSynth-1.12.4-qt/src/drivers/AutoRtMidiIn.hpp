/*
 *  RtMidiDriver.hpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */
#pragma once

/*
#include "de_types.hpp"
#include "config.h"
#include "Configuration.h"
#include <RtMidi/RtMidi.h>

struct AutoRtMidiIn
{
   RtMidiIn* m_rtm;
   int m_port;
   std::string m_portName;

   AutoRtMidiIn( int port = 0 )
      : m_rtm( nullptr )
      , m_port( port )
   {

   }

   ~AutoRtMidiIn()
   {
      close();
   }


   void close()
   {
      if ( m_rtm )
      {
         delete m_rtm;
         m_rtm = nullptr;
      }
   }

   void open()
   {
      close();

      try
      {
         m_rtm = new RtMidiIn();

         // Check available ports.
         int nPorts = int( m_rtm->getPortCount() );
         if ( nPorts > 0 && nPorts > m_port )
         {
            m_portName = m_rtm->getPortName( m_port );

            m_rtm->openPort( m_port );

            // Set our callback function. This should be done immediately after
            // opening the port to avoid having incoming messages written to the
            // queue.
            m_rtm->setCallback( &mycallback );

            // Don't ignore sysex, timing, or active sensing messages.
            m_rtm->ignoreTypes( false, false, false );
         }
         else
         {
            std::cout << "No ports available!\n";
         }


      }
      catch ( RtMidiError &error )
      {
         error.printMessage();
         goto cleanup;
      }
//      std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
//      char input;
//      std::cin.get(input);
*/
