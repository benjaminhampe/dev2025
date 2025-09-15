#pragma once
#include <RtMidi/RtMidi.h>
#include <sstream>
#include <thread>

struct RtMidiUtil
{
   static RtMidiOut*
   openMidiOut( uint32_t port )
   {
      RtMidiOut* rtm = nullptr;
      try
      {
         rtm = new RtMidiOut( RtMidi::WINDOWS_MM, "RtMidiOutputEnum" );

         uint32_t const nPorts = rtm->getPortCount();
         if ( nPorts > port )
         {
            rtm->openPort( port );
            return rtm;
         }
         else
         {
            delete rtm;
            rtm = nullptr;
            return nullptr;
         }
      }
      catch ( RtMidiError & e )
      {
         printf("openMidiOut() :: RtMidiException: %s\n", e.getMessage().c_str() );
         fflush( stdout );

         if ( rtm )
         {
            delete rtm;
            rtm = nullptr;
         }
         return nullptr;
      }
   }

//   static void printMidiIn()
//   {
//      std::vector< std::string > const ports = enumerateMidiIn();

//      printf("enumerateMidiIn() :: Port.Count = %d\n", int(ports.size()) );
//      for ( size_t i = 0; i < ports.size(); ++i )
//      {
//         printf("enumerateMidiIn() :: Port[%d] %s\n", int(i), ports[i].c_str() );

//      }
//      fflush( stdout );
//   }



   static void printMidiOut()
   {
      std::vector< std::string > const ports = enumerateMidiOut();

      printf("enumerateMidiOut() :: Port.Count = %d\n", int(ports.size()) );
      for ( size_t i = 0; i < ports.size(); ++i )
      {
         printf("enumerateMidiOut() :: Port[%d] %s\n", int(i), ports[i].c_str() );

      }
      fflush( stdout );
   }


   static std::vector< std::string >
   enumerateMidiOut()
   {
      std::vector< std::string > portNames;

      RtMidiOut* rtm = nullptr;
      try
      {
         rtm = new RtMidiOut( RtMidi::WINDOWS_MM, "RtMidiOutputEnum" );

         uint32_t const nPorts = rtm->getPortCount();
         if ( nPorts )
         {
            portNames.reserve( nPorts );

            for ( uint32_t i = 0; i < nPorts; i++ )
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
         printf("enumerateMidiOut() :: RtMidiException: %s\n", e.getMessage().c_str() );
         fflush( stdout );

         if ( rtm )
         {
            delete rtm;
            rtm = nullptr;
         }
      }

      return portNames;
   }

   static void printMidiIn()
   {
      std::vector< std::string > const ports = enumerateMidiIn();

      printf("enumerateMidiIn() :: Port.Count = %d\n", int(ports.size()) );
      for ( size_t i = 0; i < ports.size(); ++i )
      {
         printf("enumerateMidiIn() :: Port[%d] %s\n", int(i), ports[i].c_str() );

      }
      fflush( stdout );
   }

   static std::vector< std::string >
   enumerateMidiIn()
   {
      std::vector< std::string > portNames;

      RtMidiIn* rtm = nullptr;
      try
      {
         rtm = new RtMidiIn( RtMidi::WINDOWS_MM, "RtMidiInputEnum" );

         uint32_t const nPorts = rtm->getPortCount();
         if ( nPorts )
         {
            portNames.reserve( nPorts );

            for ( uint32_t i = 0; i < nPorts; i++ )
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
};
