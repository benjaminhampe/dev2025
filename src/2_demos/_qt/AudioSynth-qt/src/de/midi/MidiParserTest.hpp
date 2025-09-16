#pragma once
#include <de/midi/MidiParser.hpp>
//#include <de/midi/MidiHtmlWriter.hpp>

namespace de {
namespace midi {


// =======================================================================
struct MidiParserTest
// =======================================================================
{
   static std::vector< uint8_t >
   data1()
   {
      return std::vector< uint8_t >{
         //MIDI file theader
         0x4D, 0x54, 0x68, 0x64, // 4D 54 68 64 = 'M','T','h','d'
         0x00, 0x00, 0x00, 0x06, // 00 00 00 06 = header chunk length
         0x00, 0x00,             // 00 00       = format 0
         0x00, 0x01,             // 00 01       = 1 track
         0x00, 0x80,             // 00 80       = 128 ticks per quarter-note
         // Track 1 header
         0x4D, 0x54, 0x72, 0x6B, // 4D 54 72 6B = 'M','T','r','k'
         0x00, 0x00, 0x00, 0x2E, // 00 00 00 2E = data length (46)
         // Track 1 data
         0x00, 0xC1, 0x18,       // dt = 0, Ch '1' ProgramChange 'C' with 24 = GM Patch 25 = ?
         0x00, 0x91, 0x37, 0x60, // dt = 0, Ch '1' NoteOn 37=55 G3, vel(96) = mezzo forte
         0x00, 0x90, 0x3C, 0x60, // dt = 0, Ch '0' NoteOn 3C=60 C4, vel(96) = mezzo forte
         0x81, 0x00, 0x3C, 0x00, // dt=128, Ch '0' runstat NoteOff 3C=60 C4, vel(0) = off
         0x00, 0x3E, 0x60,       // dt = 0, Ch '0' runstat NoteOn 3E=62 D4, vel(96) = mezzo forte
         0x81, 0x00, 0x3E, 0x00, // dt=128, Ch '0' runstat NoteOff 3E=62 D4, vel(0) = off
         0x00, 0x40, 0x60,       // dt = 0, Ch '0' runstat NoteOn 40=64 E4, vel(96) = mezzo forte
         0x00, 0x91, 0x37, 0x00, // dt = 0, Ch '1' NoteOff 37=55 G3, vel(0) = off
         0x00, 0x91, 0x39, 0x60, // dt = 0, Ch '1' NoteOn 39=57 A3, vel(96) = mezzo forte
         0x81, 0x00, 0x90, 0x40, 0x00, // dt=128, Ch '0' NoteOff 40=64 E4, vel(0) = off
         0x00, 0x91, 0x39, 0x00, // dt = 0, Ch '1' NoteOff 39=57 A3, vel(0) = off
         // Track 1 footer
         0x00, 0xFF, 0x2F, 0x00
      };
   }

   static std::vector< uint8_t >
   data2()
   {
      return std::vector< uint8_t >{
         //MIDI file header
         0x4D, 0x54, 0x68, 0x64, // 4D 54 68 64 = 'M','T','h','d'
         0x00, 0x00, 0x00, 0x06, // 00 00 00 06 = header chunk length
         0x00, 0x01,             // 00 00       = format 1
         0x00, 0x02,             // 00 01       = 2 tracks
         0x00, 0x80,             // 00 80       = 128 ticks per quarter-note
         // Track 1 header
         0x4D, 0x54, 0x72, 0x6B, // 4D 54 72 6B = 'M','T','r','k'
         0x00, 0x00, 0x00, 0x1A, // 00 00 00 1A = data length (26)
         // Track 1 data
         0x00, 0x90, 0x3C, 0x60, // dt = 0, Ch '0' NoteOn C4 (3C=60), vel(96) = mezzo forte
         0x81, 0x00, 0x3C, 0x00, // dt=128, Ch '0' runstat NoteOff C4 (3C=60), vel(0) = off
         0x00, 0x3E, 0x60,       // dt = 0, Ch '0' runstat NoteOn D4 (3E=62), vel(96) = mezzo forte
         0x81, 0x00, 0x3E, 0x00, // dt=128, Ch '0' runstat NoteOff D4 (3E=62), vel(0) = off
         0x00, 0x40, 0x60,       // dt = 0, Ch '0' runstat NoteOn E4 (40=64), vel(96) = mezzo forte
         0x81, 0x00, 0x40, 0x00, // dt=128, Ch '0' runstat NoteOff E4 (40=64), vel(0) = off
         0x00, 0xFF, 0x2F, 0x00, // Track 1 footer
         // Track 2 header
         0x4D, 0x54, 0x72, 0x6B, // 4D 54 72 6B = 'M','T','r','k'
         0x00, 0x00, 0x00, 0x16, // 00 00 00 16 = data length (22)
         // Track 2 data
         0x00, 0xC1, 0x18,       // dt = 0, Ch '1' ProgramChange 'C' with 24 = GM Patch 25 = ?
         0x00, 0x91, 0x37, 0x60, // dt = 0, Ch '1' NoteOn G3 (37=55), vel(96) = mezzo forte
         0x82, 0x00, 0x37, 0x00, // dt=128, Ch '1' runstat NoteOff G3 (37=55), vel(0) = off
         0x00, 0x39, 0x60,       // dt = 0, Ch '1' NoteOn A3 (39=57), vel(96) = mezzo forte
         0x81, 0x00, 0x39, 0x00, // dt=128, Ch '1' runstat NoteOff A3 (39=57), vel(0) = off
         0x00, 0xFF, 0x2F, 0x00  // Track 2 footer
      };
   }

   static std::vector< uint8_t >
   data3()
   {
      return std::vector< uint8_t >{
         0x4D, 0x54, 0x68, 0x64,
         0x00, 0x00, 0x00, 0x06,
         0x00, 0x00, 0x00, 0x01, 0x01, 0xE0,
         0x4D, 0x54, 0x72, 0x6B,
         0x00, 0x00, 0x00, 0x86,
         0x00, 0xFF, 0x03, 0x35, 0x47, 0x65, 0x6E, 0x65, 0x72, 0x61,
         0x74, 0x65, 0x64, 0x20, 0x6D, 0x6F, 0x72, 0x73, 0x65, 0x20, 0x63, 0x6F, 0x64, 0x65, 0x20, 0x77,
         0x77, 0x77, 0x2E, 0x6D, 0x6F, 0x62, 0x69, 0x6C, 0x65, 0x66, 0x69, 0x73, 0x68, 0x2E, 0x63, 0x6F,
         0x6D, 0x2F, 0x67, 0x6F, 0x2F, 0x6D, 0x6F, 0x72, 0x73, 0x65, 0x5F, 0x63, 0x6F, 0x64, 0x65, 0x00,
         0xFF, 0x02, 0x17, 0x28, 0x43, 0x29, 0x20, 0x32, 0x30, 0x31, 0x30, 0x20, 0x4D, 0x6F, 0x62, 0x69,
         0x6C, 0x65, 0x66, 0x69, 0x73, 0x68, 0x2E, 0x63, 0x6F, 0x6D, 0x00, 0xFF, 0x58, 0x04, 0x04, 0x02,
         0x18, 0x08, 0x00, 0xFF, 0x51, 0x03, 0x08, 0x7A, 0x23, 0x00, 0xC0, 0x4F, 0x81, 0x70, 0x90, 0x51,
         0x7F, 0x3A, 0x80, 0x51, 0x00, 0x3A, 0x90, 0x51, 0x7F, 0x81, 0x2E, 0x80, 0x51, 0x00, 0x82, 0x2C,
         0x90, 0x51, 0x00, 0x82, 0x2C, 0x80, 0x51, 0x00, 0x00, 0xFF, 0x2F, 0x00 };
   }

   //Then the track chunk. Followed by the events (notice the running status is used in places):
   //4D 54 72 6B = MTrk
   //00 00 00 3B = chunk length (59)
   //Delta-Time [EventCodes]
   //00 [FF 58] 04 04 02 18 08= time signature 4bytes?, 4/4 time, (24 MIDI clocks = 1beat) / click?, 8 32nd notes / beat.
   //00 [FF 51] 03 07 A1 20   = tempo 3bytes? 500.000 usec / quarter note = 120bpm
   //00 [C0] 05               = Ch0 ProgramChange 5 = GM Patch 6 = Electric Piano 2
   //00 [C1] 2E               = Ch1 ProgramChange 46 = GM Patch 47 = Harp
   //00 [C2] 46               = Ch2 ProgramChange 70 = GM Patch 71 = Bassoon
   //00 [92] 30 60            = Ch2 NoteOn C3(48), forte(96)
   //00 [3C] 60               = running status
   //=00[3C] 60 60   > > > >  = Ch2 NoteOn C4(60), also forte(96)
   //60 [91] 43 64            = Ch1 NoteOn G4(67), mezzo-forte, delta-time(96)
   //96 [90] 76 32            = Ch0 NoteOn E5, piano
   //00 [92] 30 60            = Ch2 NoteOn C3, piano
   //00 [3C] 60               = running status
   //60 [91] 43 40            = Ch1 NoteOn C4, mezzo-forte
   //60 [90] 4C 20            = Ch2 NoteOn C3, piano
   //81 40 [82] 30 40 two-byte delta-time
   //00 [3C] 40 running status
   //00 [81] 43 40
   //00 [80] 4C 40
   //00 [FF 2F] 00 end of track

   //Then the track chunk. Followed by the events (notice the running status is used in places):
   //4D 54 72 6B MTrk
   //00 00 00 3B chunk length (59)
   //Delta-Time Event Comments
   //00 FF 58 04 04 02 18 08 time signature
   //00 FF 51 03 07 A1 20 tempo
   //00 C0 05
   //00 C1 2E
   //00 C2 46
   //00 92 30 60
   //00 3C 60 running status
   //60 91 43 40
   //60 90 4C 20
   //81 40 82 30 40 two-byte delta-time
   //00 3C 40 running status
   //00 81 43 40
   //00 80 4C 40
   //00 FF 2F 00 end of track

   static std::vector< uint8_t >
   data4()
   {
      return std::vector< uint8_t >{
         //4D 54 68 64 = MThd
         //00 00 00 06 = header chunk length
         //00 00       = format 0
         //00 01       = one track
         //00 60       = 96 per quarter-note
         0x4D, 0x54, 0x68, 0x64,
         0x00, 0x00, 0x00, 0x06,
         0x00, 0x00, 0x00, 0x01, 0x00, 0x60,

         //Then the track chunk. Followed by the events (notice the running status is used in places):
         //4D 54 72 6B = MTrk
         //00 00 00 3B = chunk length (59)
         0x4D, 0x54, 0x72, 0x6B,
         0x00, 0x00, 0x00, 0x3B,
         //Delta-Time [EventCodes]
         //00 [FF 58] 04 04 02 18 08= time signature 4bytes?, 4/4 time, (24 MIDI clocks = 1beat) / click?, 8 32nd notes / beat.
         //00 [FF 51] 03 07 A1 20   = tempo 3bytes? 500.000 usec / quarter note = 120bpm
         //00 [C0] 05               = Ch0 ProgramChange 5 = GM Patch 6 = Electric Piano 2
         //00 [C1] 2E               = Ch1 ProgramChange 46 = GM Patch 47 = Harp
         //00 [C2] 46               = Ch2 ProgramChange 70 = GM Patch 71 = Bassoon
         //00 [92] 30 60            = Ch2 NoteOn C3(48), forte(96)
         0x00, 0xFF, 0x58, 0x04, 0x04, 0x02, 0x18, 0x08,
         0x00, 0xFF, 0x51, 0x03, 0x07, 0xA1, 0x20, 0x00, 0xC0, 0x05,
         0x00, 0xC1, 0x2E, 0x00, 0xC2, 0x46, 0x00, 0x92, 0x30, 0x60, 0x00, 0x3C, 0x60,
         0x60, 0x91, 0x43, 0x40, 0x60, 0x90, 0x4C, 0x20, 0x81, 0x40, 0x82, 0x30, 0x40,
         0x00, 0x3C, 0x40, 0x00, 0x81, 0x43, 0x40, 0x00, 0x80, 0x4C, 0x40, 0x00, 0xFF, 0x2F, 0x00
      };
   }
/*
   static void
   testData( std::string loadUri, std::string saveUri, std::vector< uint8_t > const & data )
   {
      InputStream file;
      file.loadFromData( data.data(), data.size() );

      MidiHtmlWriter htmlWriter;

      MidiParser p;
      p.addListener( &htmlWriter );
      p.parse( file.beg(), file.end(), loadUri );

      htmlWriter.save( saveUri, loadUri, "" );
   }

   static void
   testFile( std::string loadUri, std::string saveUri )
   {
      InputStream file;
      file.loadFromFile( loadUri );

      MidiHtmlWriter htmlWriter;

      MidiParser p;
      p.addListener( &htmlWriter );
      p.parse( file.beg(), file.end(), loadUri );

      htmlWriter.save( saveUri, loadUri, "" );
   }

   static void
   test()
   {
//      {
//         auto d = data1();
//         MidiParser parser = MidiParser::createFromData( d.data(), d.size() );
//      }

//      {
//         auto d = data2();
//         MidiParser parser = MidiParser::createFromData( d.data(), d.size() );
//      }

//      {
//         auto d = data3();
//         MidiParser parser = MidiParser::createFromData( d.data(), d.size() );
//      }

////      {
////         auto d = data4();
////         MidiParser parser = MidiParser::createFromData( d.data(), d.size() );
////      }

//      MidiParser::createFromFile( "../../../_doc/midi/midi/Song of joy.mid");
//      MidiParser::createFromFile( "../../../_doc/midi/midi/Fantasia on Greensleeves.mid");

      //MidiParser::loadFromFile( "../../../_doc/midi/midi-sample/Moon_River_Noted_MusicBox.mid");

      testData( "MidiParser_testData1.mid", "MidiParser_testData1.mid.html", data1() );
      testData( "MidiParser_testData2.mid", "MidiParser_testData2.mid.html", data2() );
      testData( "MidiParser_testData3.mid", "MidiParser_testData3.mid.html", data3() );

      testFile( "../../media-midi/Song of joy.mid", "Song of joy.mid.html" );
      testFile( "../../media-midi/Super_Mario.mid", "Super_Mario.mid.html" );

      testFile( "../../media-midi/the moonlight sonata.mid", "the moonlight sonata.mid.html" );
      testFile( "../../media-midi/For Elise.mid", "For Elise.mid.html" );
      testFile( "../../media-midi/clair de lune - Debussy.mid", "clair de lune - Debussy.mid" );
      testFile( "../../media-midi/bach_tocatta_fugue_d_minor.mid", "bach_tocatta_fugue_d_minor.mid.html" );
      testFile( "../../media-midi/All The Small Things.mid", "../../media-midi/All The Small Things.mid.html" );
   }
*/
};

} // end namespace midi
} // end namespace de
