#include <iostream>
#include <cstdlib>
#include <RtMidi/RtMidi.h>

// midiout.cpp

#include <thread>
#include <chrono>

//========================================================================
int main(int argc, char** argv)
//========================================================================
{
   RtMidiOut* midiout = new RtMidiOut();
   std::vector< uint8_t > message;
   // Check available ports.
   uint32_t nPorts = midiout->getPortCount();
   if ( nPorts == 0 )
   {
      std::cout << "No ports available!\n";
      goto cleanup;
   }

   // Open first available port.
   midiout->openPort( 0 );

   // Send out a series of MIDI messages.
   // Program change: 192, 5
   message.push_back( 192 );
   message.push_back( 5 );
   midiout->sendMessage( &message );

   // Control Change: 176, 7, 100 (volume)
   message[0] = 176;
   message[1] = 7;
   message.push_back( 100 );
   midiout->sendMessage( &message );

for ( int k = 0; k < 10; ++k )
{
   // NoteOn: 144, 64, 90
   message[0] = 144;
   message[1] = 64;
   message[2] = 90;
   midiout->sendMessage( &message );

   // Sleep:
   std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

   // NoteOff: 128, 64, 40
   message[0] = 128;
   message[1] = 64;
   message[2] = 40;
   midiout->sendMessage( &message );


   // Sleep:
   std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );

}

   // Clean up
cleanup:
   delete midiout;

   return 0;
}

