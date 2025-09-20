// cmidiin.cpp
#include <iostream>
#include <cstdlib>
#include <RtMidi/RtMidi.h>

void
mycallback( double deltatime, std::vector< uint8_t >* message, void* userData )
{
   uint32_t nBytes = message->size();
   for ( uint32_t i=0; i<nBytes; i++ )
      std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
   if ( nBytes > 0 )
      std::cout << "stamp = " << deltatime << std::endl;
}

//========================================================================
int main(int argc, char** argv)
//========================================================================
{
   RtMidiIn* midiin = new RtMidiIn();

   // Check available ports.
   uint32_t nPorts = midiin->getPortCount();
   if ( nPorts == 0 )
   {
      std::cout << "No ports available!\n";
      goto cleanup;
   }

   midiin->openPort( 0 );

   // Set our callback function. This should be done immediately after
   // opening the port to avoid having incoming messages written to the
   // queue.
   midiin->setCallback( &mycallback );

   // Don't ignore sysex, timing, or active sensing messages.
   midiin->ignoreTypes( false, false, false );

   std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
   char input;
   std::cin.get(input);

   // Clean up
cleanup:
   delete midiin;
   return 0;
}



#if 0

// qmidiin.cpp
#include <iostream>
#include <cstdlib>
#include <RtMidi/RtMidi.h>

#include <signal.h>

#include <thread>
#include <chrono>

bool done;
static void finish(int ignore){ done = true; }

//========================================================================
int main(int argc, char** argv)
//========================================================================
{
   RtMidiIn *midiin = new RtMidiIn();
   std::vector< uint8_t > message;

   int nBytes, i;
   double stamp;

   // Check available ports.
   uint32_t nPorts = midiin->getPortCount();
   if ( nPorts == 0 )
   {
      std::cout << "No ports available!\n";
      goto cleanup;
   }

   midiin->openPort( 0 );
   // Don't ignore sysex, timing, or active sensing messages.
   midiin->ignoreTypes( false, false, false );
   // Install an interrupt handler function.
   done = false;

   (void) signal(SIGINT, finish);
   // Periodically check input queue.

   std::cout << "Reading MIDI from port ... quit with Ctrl-C.\n";
   while ( !done )
   {
      stamp = midiin->getMessage( &message );
      nBytes = message.size();
      for ( i=0; i<nBytes; i++ )
      {
         std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
      }

      if ( nBytes > 0 )
      {
         std::cout << "stamp = " << stamp << std::endl;
      }

      // Sleep for 10 milliseconds:
      std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
   }

   // Clean up
cleanup:
   delete midiin;
   return 0;
}

#endif
