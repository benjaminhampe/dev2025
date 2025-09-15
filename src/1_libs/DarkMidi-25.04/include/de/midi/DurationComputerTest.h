// (c) 2023 by <benjaminhampe@gmx.de>
#pragma once
#include <de/midi/DurationComputer.h>

namespace de {
namespace midi {

// =======================================================================
struct DurationComputerTest
// =======================================================================
{
   DE_CREATE_LOGGER("de.midi.DurationComputerTest")

   static void
   test()
   {
      testMidiFile( "../../media/midi/lost_3_34.mid" );
      testMidiFile( "../../media/midi/But not tonight - Depeche Mode_4_14.mid" ); // 4:13
      testMidiFile( "../../media/midi/gigidagostino_illflywithyou_joerock_5_03.mid" ); //
      testMidiFile( "../../media/midi/thank_you_50_12.mid" ); //
      testMidiFile( "../../media/midi/Bitter Sweet Symphony_4_16.mid" ); //
      testMidiFile( "../../media/midi/mmm_3_43.mid" ); //
      testMidiFile( "../../media/midi/porcelain_moby_davebulow_4_07.mid" ); //
      testMidiFile( "../../media/midi/hotelcal_6_37.mid" ); //
   }

   static void
   testMidiFile( std::string loadUri )
   {
      DE_TRACE("========================================================")
      DE_TRACE("Test: ", loadUri)

      Parser parser;
      DurationComputer listener;
      parser.addListener( &listener );
      parser.parse( loadUri );

      listener.computeDurationInSeconds();
   }
};

} // end namespace midi
} // end namespace de
