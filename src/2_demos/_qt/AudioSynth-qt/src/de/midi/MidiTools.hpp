#pragma once
#include <de/midi/InputStream.hpp>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <sstream>

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
// Opens a Midifile and display partitur like, 1st milestone

// MIDI section <-------------MIDI Header--------------->
// Byte number  0  1  2  3  4  5  6  7  8  9  10 11 12 13
// Byte data    4D 54 68 64 00 00 00 06 00 01 00 01 00 80

// <-----Track Header----> <-Track data-> {Track out>
// 14 15 16 17 18 19 20 21 [22 to x]      [x+1 to x+4]
// 4D 54 72 6B 00 00 00 0A blah blah..... 00 FF 2F 00

// Toene Oktave Frequenzen
// ================================================================
//     0       1       2       3       4       5       6       7
// C   16,35   32,70   65,41   130,81  261,63  523,25  1046,50 2093,00
// C’  17,32   34,65   69,30   138,59  277,18  554,37  1108,74 2217,46
// D   18,35   36,71   73,42   146,83  293,66  587,33  1174,66 2349,32
// D’  19,45   38,89   77,78   155,56  311,13  622,25  1244,51 2489,02
// E   20,60   41,20   82,41   164,81  329,63  659,26  1328,51 2637,02
// F   21,83   43,65   87,31   174,61  349,23  698,46  1396,91 2793,83
// F’  23,12   46,25   92,50   185,00  369,99  739,99  1479,98 2959,96
// G   24,50   49,00   98,00   196,00  392,00  783,99  1567,98 3135,96
// G’  25,96   51,91   103,83  207,65  415,30  830,61  1661,22 3322,44
// A   27,50   55,00   110,00  220,00  440,00  880,00  1760,00 3520,00
// A’  29,14   58,27   116,54  233,08  466,16  923,33  1864,66 3729,31
// H   30,87   61,74   123,47  246,94  493,88  987,77  1975,53 3951,07

// MIDI Channel Messages:
// ================================================================
// 8n Note Off                8n kk vv, kk = key number (0-127) vv = velocity (0-127)
// 9n Note On                 9n kk vv, kk = key number (0-127) vv = velocity (1-127, 0 = note off)
// An Polyphonic After Touch  An kk tt, kk = key number (0-127) tt = after touch (0-127)
// Bn Control Change          Bn cc xx, cc = controller (0-121) xx = value (0-127)
// Bn Channel Mode Local Ctl  Bn 7A xx xx = 0 (off), 127 (on)
// Bn All Notes Off           Bn 7B 00
// Bn Omni Mode Off           Bn 7C 00
// Bn Omni Mode On            Bn 7D 00
// Bn Mono Mode On            Bn 7E cc cc = number of channels
// Bn Poly Mode On            Bn 7F 00
// Cn Program Change          Cn pp pp = program (0-127)
// Dn Channel After Touch     Dn tt tt = after touch (0-127)
// En Pitch Wheel Change      En ll hh ll = low 7 bits (0-127) hh = high 7 bits (0-127)

// microseconds per tick = microseconds per quarter note / ticks per quarter note

// ticks = resolution * (1 / tempo) * 1000 * elapsed_time

// - resolution in ticks/beat (or equivalently ticks/Quarter note). This fixes the smallest time interval to be generated.
// - tempo in microseconds per beat, which determines how many ticks are generated in a set time interval.
// - elapsed_time which provides the fixed timebase for playing the midi events.

// ticks   ticks   beat   1000 us    ms
// ----- = ----- * ---- * ------- * ----
// time    beat     us       ms     time

// - time is the elapsed time between calls to the tick generator. This must be calculated by the tick generator based on the history of the previous call to the tick generator.
// - tempo is the tempo determined by the Set Tempo MIDI event. Note this event only deals in Quarter Notes.
// - resolution is held as TicksPerQuarterNote.

//>>> mido.parse([0x92, 0x10, 0x20])
//Message('note_on', channel=2, note=16, velocity=32, time=0)
//>>> mido.parse_all([0x92, 0x10, 0x20, 0x82, 0x10, 0x20])
//[Message('note_on', channel=2, note=16, velocity=32, time=0),
//Message('note_off', channel=2, note=16, velocity=32, time=0)]

//////////////////////////////
//
// MidiFile::setMillisecondTicks -- set the ticks per quarter note
//   value to milliseconds.  The format for this specification is
//   highest 8-bits: SMPTE Frame rate (as a negative 2's compliment value).
//   lowest 8-bits: divisions per frame (as a positive number).
//   for millisecond resolution, the SMPTE value is -25, and the
//   frame rate is 40 frame per division.  In hexadecimal, these
//   values are: -25 = 1110,0111 = 0xE7 and 40 = 0010,1000 = 0x28
//   So setting the ticks per quarter note value to 0xE728 will cause
//   delta times in the MIDI file to represent milliseconds.  Calling
//   this function will not change any exiting timestamps, it will
//   only change the meaning of the timestamps.
//

// void MidiFile::setMillisecondTicks(void) {
   // m_ticksPerQuarterNote = 0xE728;
// }

//////////////////////////////
//
// MidiFile::addTimeSignature -- Add a time signature meta message
//      (meta #0x58).  The "bottom" parameter must be a power of two;
//      otherwise, it will be set to the next highest power of two.
//
// Default values:
//     clocksPerClick     == 24 (quarter note)
//     num32ndsPerQuarter ==  8 (8 32nds per quarter note)
//
// Time signature of 4/4 would be:
//    top    = 4
//    bottom = 4 (converted to 2 in the MIDI file for 2nd power of 2).
//    clocksPerClick = 24 (2 eighth notes based on num32ndsPerQuarter)
//    num32ndsPerQuarter = 8
//
// Time signature of 6/8 would be:
//    top    = 6
//    bottom = 8 (converted to 3 in the MIDI file for 3rd power of 2).
//    clocksPerClick = 36 (3 eighth notes based on num32ndsPerQuarter)
//    num32ndsPerQuarter = 8
//
//////////////////////////////
//
// MidiFile::addCompoundTimeSignature -- Add a time signature meta message
//      (meta #0x58), where the clocksPerClick parameter is set to three
//      eighth notes for compount meters such as 6/8 which represents
//      two beats per measure.
//
// Default values:
//     clocksPerClick     == 36 (quarter note)
//     num32ndsPerQuarter ==  8 (8 32nds per quarter note)
//
//////////////////////////////
//
// MidiFile::makeVLV --  This function is used to create
//   size byte(s) for meta-messages.  If the size of the data
//   in the meta-message is greater than 127, then the size
//   should (?) be specified as a VLV.
//

namespace de {
namespace midi {


enum EMidiStatus
{
   kNoteOff = 0x80,
   kNoteOn = 0x90,
   kPolyAfterTouch = 0xA0, // PolyPressure max = 0xAF
   kController = 0xB0,  // CC
   kProgramChange = 0xC0,
   kAfterTouch = 0xD0,  // Channel AfterTouch
   kPitchBend = 0xE0,
   kSysexStart = 0xF0,
   kQuarterFrame = 0xF1,
   kSongPointer = 0xF2,
   kSongSelect = 0xF3,
   kCableSelect = 0xF5,
   kSysexEnd = 0xF7,
   kMidiClock = 0xF8,
   kMidiClockStart = 0xFA,
   kMidiClockContinue = 0xFB,
   kMidiClockStop = 0xFC,
   kActiveSensing = 0xFE,
   kMidiReset = 0xFF, // meta events in *.mid, *.smf ( standard midi file )
};

#define MIDI_CHANNEL_BITS     0x0F
#define MIDI_STATUS_BITS      0xF0

// ================================================================

//==========================================================================
#define MUSIC_NUM_KEYS 18
#define MUSIC_NUM_SCALES 8

const int musicScales[MUSIC_NUM_SCALES][MUSIC_NUM_KEYS] =
{
    //chromatic
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17},
    //major
    {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28, 29},
    //major pentatonic
    {0, 2, 4, 7, 9, 12, 14, 16, 19, 21, 24, 26, 28, 31, 33, 36, 38, 40},
    //minor
    {0, 2, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 20, 22, 24, 26, 27, 29},
    //minor pentatonic
    {0, 3, 5, 7, 10, 12, 15, 17, 19, 22, 24, 27, 29, 31, 34, 36, 39, 41},
    //melodic minor
    {0, 2, 3, 5, 7, 9, 11, 12, 14, 15, 17, 19, 21, 23, 24, 26, 27, 29},
    //harmonic minor
    {0, 2, 3, 5, 7, 8, 11, 12, 14, 15, 17, 19, 20, 23, 24, 26, 27, 29},
    //blues
    {0, 3, 5, 6, 7, 10, 12, 15, 17, 18, 19, 22, 24, 27, 29, 30, 31, 34}
};

/*
class IMasterTrackInfo: public FUnknown
{
public:
//------------------------------------------------------------------------
   virtual int	getPpqBase () = 0;
   virtual int	seconds2ppq (int seconds) = 0;
   virtual int	ppq2seconds (int ppq) = 0;

   virtual int	countTempoEvents () = 0;
   virtual int	getTempo (int index, int* ppq, int* bpm) = 0; // bpm * 100
   virtual int	getTempoIndex (int ppq) = 0;

   virtual int	getBar (int ppq, int* barCount, int* beatCount, int* remainder) = 0;
   virtual int	getBarPosition (int barCount, int beatCount, int remainder) = 0;

   virtual int	countSignatureEvents () = 0;
   virtual int	getSignature (int index, int* numerator, int* denominator) = 0;
   virtual int	getSignatureIndex (int ppq) = 0;
//------------------------------------------------------------------------
   static const FUID iid;
};

bool writeMidi(const MidiEvent& midiEvent)
{
   if (midiEvent.size > 4) return true;

   VstEvents vstEvents; memset(&vstEvents, 0, sizeof(VstEvents));

   VstMidiEvent vstMidiEvent; memset(&vstMidiEvent, 0, sizeof(VstMidiEvent));

   vstEvents.numEvents = 1;
   vstEvents.events[0] = (VstEvent*)&vstMidiEvent;

   vstMidiEvent.type = kVstMidiType;
   vstMidiEvent.byteSize    = kVstMidiEventSize;
   vstMidiEvent.deltaFrames = midiEvent.frame;

   for ( uint8_t i=0; i<midiEvent.size; ++i )
      vstMidiEvent.midiData[i] = midiEvent.data[i];

   return hostCallback(audioMasterProcessEvents, 0, 0, &vstEvents) == 1;
}

static bool writeMidiCallback(void* ptr, const MidiEvent& midiEvent)
{
   return ((PluginVst*)ptr)->writeMidi(midiEvent);
}

*/

// MIDI Defines :: General Midi Instrument categories
enum EMidiGM_Category
{
   GMCat_Piano = 0,
   GMCat_ChromaticPercussion = 8,
   GMCat_Organ = 16,
   GMCat_Guitar = 24,
   GMCat_Bass = 32,
   GMCat_Strings = 40,
   GMCat_Ensemble = 48,
   GMCat_Brass = 56,
   GMCat_Reed = 64,
   GMCat_Pipe = 72,
   GMCat_SynthLead = 80,
   GMCat_SynthPad = 88,
   GMCat_SynthEffects = 96,
   GMCat_Ethnic = 104,
   GMCat_Percussive = 112,
   GMCat_SoundEffects = 120,
   GMCat_Unknown = 128
};

// MIDI Defines General Midi Instruments
enum EMidiGM
{
   GM_0_AcousticGrandPiano = 0,
   GM_1_BrightAcousticPiano,
   GM_2_ElectricGrandPiano,
   GM_3_HonkyTonkPiano,
   GM_4_RhodesPiano,
   GM_5_ChorusedPiano,
   GM_6_Harpsichord,
   GM_7_Clavinet,
   GM_8_Celesta,
   GM_9_Glockenspiel,
   GM_10_MusicBox,
   GM_11_Vibraphone,
   GM_12_Marimba,
   GM_13_Xylophone,
   GM_14_TubularBells,
   GM_15_Dulcimer,
   GM_16_HammondOrgan,
   GM_17_PercussiveOrgan,
   GM_18_RockOrgan,
   GM_19_ChurchOrgan,
   GM_20_ReedOrgan,
   GM_21_Accordian,
   GM_22_Harmonica,
   GM_23_TangoAccordian,
   GM_24_AcousticGuitarNylon,
   GM_25_AcousticGuitarSteel,
   GM_26_ElectricGuitarJazz,
   GM_27_ElectricGuitarClean,
   GM_28_ElectricGuitarMuted,
   GM_29_OverdrivenGuitar,
   GM_30_DistortionGuitar,
   GM_31_GuitarHarmonics,
   GM_32_AcousticBass,
   GM_33_ElectricBassFinger,
   GM_34_ElectricBassPick,
   GM_35_FretlessBass,
   GM_36_SlapBass1,
   GM_37_SlapBass2,
   GM_38_SynthBass1,
   GM_39_SynthBass2,
   GM_40_Violin,
   GM_41_Viola,
   GM_42_Cello,
   GM_43_Contrabass,
   GM_44_TremoloStrings,
   GM_45_PizzicatoStrings,
   GM_46_OrchestralHarp,
   GM_47_Timpani,
   GM_48_StringEnsemble1,
   GM_49_StringEnsemble2,
   GM_50_SynthStrings1,
   GM_51_SynthStrings2,
   GM_52_ChoirAahs,
   GM_53_VoiceOohs,
   GM_54_SynthVoice,
   GM_55_OrchestraHit,
   GM_56_Trumpet,
   GM_57_Trombone,
   GM_58_Tuba,
   GM_59_MutedTrumpet,
   GM_60_FrenchHorn,
   GM_61_BrassSection,
   GM_62_SynthBrass1,
   GM_63_SynthBrass2,
   GM_64_SopranoSax,
   GM_65_AltoSax,
   GM_66_TenorSax,
   GM_67_BaritoneSax,
   GM_68_Oboe,
   GM_69_EnglishHorn,
   GM_70_Bassoon,
   GM_71_Clarinet,
   GM_72_Piccolo,
   GM_73_Flute,
   GM_74_Recorder,
   GM_75_PanFlute,
   GM_76_BottleBlow,
   GM_77_Shakuhachi,
   GM_78_Whistle,
   GM_79_Ocarina,
   GM_80_Lead1Square,
   GM_81_Lead2Sawtooth,
   GM_82_Lead3CaliopeLead,
   GM_83_Lead4ChiffLead,
   GM_84_Lead5Charang,
   GM_85_Lead6Voice,
   GM_86_Lead7Fifths,
   GM_87_Lead8BrassAndLead,
   GM_88_Pad1NewAge,
   GM_89_Pad2Warm,
   GM_90_Pad3Polysynth,
   GM_91_Pad4Choir,
   GM_92_Pad5Bowed,
   GM_93_Pad6Metallic,
   GM_94_Pad7Halo,
   GM_95_Pad8Sweep,
   GM_96_FX1Rain,
   GM_97_FX2Soundtrack,
   GM_98_FX3Crystal,
   GM_99_FX4Atmosphere,
   GM_100_FX5Brightness,
   GM_101_FX6Goblins,
   GM_102_FX7Echoes,
   GM_103_FX8SciFi,
   GM_104_Sitar,
   GM_105_Banjo,
   GM_106_Shamisen,
   GM_107_Koto,
   GM_108_Kalimba,
   GM_109_Bagpipe,
   GM_110_Fiddle,
   GM_111_Shanai,
   GM_112_TinkleBell,
   GM_113_Agogo,
   GM_114_SteelDrums,
   GM_115_Woodblock,
   GM_116_TaikoDrum,
   GM_117_MelodicTom,
   GM_118_SynthDrum,
   GM_119_ReverseCymbal,
   GM_120_GuitarFretNoise,
   GM_121_BreathNoise,
   GM_122_Seashore,
   GM_123_BirdTweet,
   GM_124_TelephoneRing,
   GM_125_Helicopter,
   GM_126_Applause,
   GM_127_Gunshot,
   GM_UnknownInstrument
};

// MIDI Defines
inline std::string
EMidiGM_toString( int instrument )
{
   switch ( instrument )
   {
      case GM_0_AcousticGrandPiano: return "GM_0_AcousticGrandPiano";
      case GM_1_BrightAcousticPiano: return "GM_1_BrightAcousticPiano";
      case GM_2_ElectricGrandPiano: return "GM_2_ElectricGrandPiano";
      case GM_3_HonkyTonkPiano: return "GM_3_HonkyTonkPiano";
      case GM_4_RhodesPiano: return "GM_4_RhodesPiano";
      case GM_5_ChorusedPiano: return "GM_5_ChorusedPiano";
      case GM_6_Harpsichord: return "GM_6_Harpsichord";
      case GM_7_Clavinet: return "GM_7_Clavinet";
      case GM_8_Celesta: return "GM_8_Celesta";
      case GM_9_Glockenspiel: return "GM_9_Glockenspiel";
      case GM_10_MusicBox: return "GM_10_MusicBox";
      case GM_11_Vibraphone: return "GM_11_Vibraphone";
      case GM_12_Marimba: return "GM_12_Marimba";
      case GM_13_Xylophone: return "GM_13_Xylophone";
      case GM_14_TubularBells: return "GM_14_TubularBells";
      case GM_15_Dulcimer: return "GM_15_Dulcimer";
      case GM_16_HammondOrgan: return "GM_16_HammondOrgan";
      case GM_17_PercussiveOrgan: return "GM_17_PercussiveOrgan";
      case GM_18_RockOrgan: return "GM_18_RockOrgan";
      case GM_19_ChurchOrgan: return "GM_19_ChurchOrgan";
      case GM_20_ReedOrgan: return "GM_20_ReedOrgan";
      case GM_21_Accordian: return "GM_21_Accordian";
      case GM_22_Harmonica: return "GM_22_Harmonica";
      case GM_23_TangoAccordian: return "GM_23_TangoAccordian";
      case GM_24_AcousticGuitarNylon: return "GM_24_AcousticGuitarNylon";
      case GM_25_AcousticGuitarSteel: return "GM_25_AcousticGuitarSteel";
      case GM_26_ElectricGuitarJazz: return "GM_26_ElectricGuitarJazz";
      case GM_27_ElectricGuitarClean: return "GM_27_ElectricGuitarClean";
      case GM_28_ElectricGuitarMuted: return "GM_28_ElectricGuitarMuted";
      case GM_29_OverdrivenGuitar: return "GM_29_OverdrivenGuitar";
      case GM_30_DistortionGuitar: return "GM_30_DistortionGuitar";
      case GM_31_GuitarHarmonics: return "GM_31_GuitarHarmonics";
      case GM_32_AcousticBass: return "GM_32_AcousticBass";
      case GM_33_ElectricBassFinger: return "GM_33_ElectricBassFinger";
      case GM_34_ElectricBassPick: return "GM_34_ElectricBassPick";
      case GM_35_FretlessBass: return "GM_35_FretlessBass";
      case GM_36_SlapBass1: return "GM_36_SlapBass1";
      case GM_37_SlapBass2: return "GM_37_SlapBass2";
      case GM_38_SynthBass1: return "GM_38_SynthBass1";
      case GM_39_SynthBass2: return "GM_39_SynthBass2";
      case GM_40_Violin: return "GM_40_Violin";
      case GM_41_Viola: return "GM_41_Viola";
      case GM_42_Cello: return "GM_42_Cello";
      case GM_43_Contrabass: return "GM_43_Contrabass";
      case GM_44_TremoloStrings: return "GM_44_TremoloStrings";
      case GM_45_PizzicatoStrings: return "GM_45_PizzicatoStrings";
      case GM_46_OrchestralHarp: return "GM_46_OrchestralHarp";
      case GM_47_Timpani: return "GM_47_Timpani";
      case GM_48_StringEnsemble1: return "GM_48_StringEnsemble1";
      case GM_49_StringEnsemble2: return "GM_49_StringEnsemble2";
      case GM_50_SynthStrings1: return "GM_50_SynthStrings1";
      case GM_51_SynthStrings2: return "GM_51_SynthStrings2";
      case GM_52_ChoirAahs: return "GM_52_ChoirAahs";
      case GM_53_VoiceOohs: return "GM_53_VoiceOohs";
      case GM_54_SynthVoice: return "GM_54_SynthVoice";
      case GM_55_OrchestraHit: return "GM_55_OrchestraHit";
      case GM_56_Trumpet: return "GM_56_Trumpet";
      case GM_57_Trombone: return "GM_57_Trombone";
      case GM_58_Tuba: return "GM_58_Tuba";
      case GM_59_MutedTrumpet: return "GM_59_MutedTrumpet";
      case GM_60_FrenchHorn: return "GM_60_FrenchHorn";
      case GM_61_BrassSection: return "GM_61_BrassSection";
      case GM_62_SynthBrass1: return "GM_62_SynthBrass1";
      case GM_63_SynthBrass2: return "GM_63_SynthBrass2";
      case GM_64_SopranoSax: return "GM_64_SopranoSax";
      case GM_65_AltoSax: return "GM_65_AltoSax";
      case GM_66_TenorSax: return "GM_66_TenorSax";
      case GM_67_BaritoneSax: return "GM_67_BaritoneSax";
      case GM_68_Oboe: return "GM_68_Oboe";
      case GM_69_EnglishHorn: return "GM_69_EnglishHorn";
      case GM_70_Bassoon: return "GM_70_Bassoon";
      case GM_71_Clarinet: return "GM_71_Clarinet";
      case GM_72_Piccolo: return "GM_72_Piccolo";
      case GM_73_Flute: return "GM_73_Flute";
      case GM_74_Recorder: return "GM_74_Recorder";
      case GM_75_PanFlute: return "GM_75_PanFlute";
      case GM_76_BottleBlow: return "GM_76_BottleBlow";
      case GM_77_Shakuhachi: return "GM_77_Shakuhachi";
      case GM_78_Whistle: return "GM_78_Whistle";
      case GM_79_Ocarina: return "GM_79_Ocarina";
      case GM_80_Lead1Square: return "GM_80_Lead1Square";
      case GM_81_Lead2Sawtooth: return "GM_81_Lead2Sawtooth";
      case GM_82_Lead3CaliopeLead: return "GM_82_Lead3CaliopeLead";
      case GM_83_Lead4ChiffLead: return "GM_83_Lead4ChiffLead";
      case GM_84_Lead5Charang: return "GM_84_Lead5Charang";
      case GM_85_Lead6Voice: return "GM_85_Lead6Voice";
      case GM_86_Lead7Fifths: return "GM_86_Lead7Fifths";
      case GM_87_Lead8BrassAndLead: return "GM_87_Lead8BrassAndLead";
      case GM_88_Pad1NewAge: return "GM_88_Pad1NewAge";
      case GM_89_Pad2Warm: return "GM_89_Pad2Warm";
      case GM_90_Pad3Polysynth: return "GM_90_Pad3Polysynth";
      case GM_91_Pad4Choir: return "GM_91_Pad4Choir";
      case GM_92_Pad5Bowed: return "GM_92_Pad5Bowed";
      case GM_93_Pad6Metallic: return "GM_93_Pad6Metallic";
      case GM_94_Pad7Halo: return "GM_94_Pad7Halo";
      case GM_95_Pad8Sweep: return "GM_95_Pad8Sweep";
      case GM_96_FX1Rain: return "GM_96_FX1Rain";
      case GM_97_FX2Soundtrack: return "GM_97_FX2Soundtrack";
      case GM_98_FX3Crystal: return "GM_98_FX3Crystal";
      case GM_99_FX4Atmosphere: return "GM_99_FX4Atmosphere";
      case GM_100_FX5Brightness: return "GM_100_FX5Brightness";
      case GM_101_FX6Goblins: return "GM_101_FX6Goblins";
      case GM_102_FX7Echoes: return "GM_102_FX7Echoes";
      case GM_103_FX8SciFi: return "GM_103_FX8SciFi";
      case GM_104_Sitar: return "GM_104_Sitar";
      case GM_105_Banjo: return "GM_105_Banjo";
      case GM_106_Shamisen: return "GM_106_Shamisen";
      case GM_107_Koto: return "GM_107_Koto";
      case GM_108_Kalimba: return "GM_108_Kalimba";
      case GM_109_Bagpipe: return "GM_109_Bagpipe";
      case GM_110_Fiddle: return "GM_110_Fiddle";
      case GM_111_Shanai: return "GM_111_Shanai";
      case GM_112_TinkleBell: return "GM_112_TinkleBell";
      case GM_113_Agogo: return "GM_113_Agogo";
      case GM_114_SteelDrums: return "GM_114_SteelDrums";
      case GM_115_Woodblock: return "GM_115_Woodblock";
      case GM_116_TaikoDrum: return "GM_116_TaikoDrum";
      case GM_117_MelodicTom: return "GM_117_MelodicTom";
      case GM_118_SynthDrum: return "GM_118_SynthDrum";
      case GM_119_ReverseCymbal: return "GM_119_ReverseCymbal";
      case GM_120_GuitarFretNoise: return "GM_120_GuitarFretNoise";
      case GM_121_BreathNoise: return "GM_121_BreathNoise";
      case GM_122_Seashore: return "GM_122_Seashore";
      case GM_123_BirdTweet: return "GM_123_BirdTweet";
      case GM_124_TelephoneRing: return "GM_124_TelephoneRing";
      case GM_125_Helicopter: return "GM_125_Helicopter";
      case GM_126_Applause: return "GM_126_Applause";
      case GM_127_Gunshot: return "GM_127_Gunshot";
      default: return "GM_UnknownInstrument";
   }
}

//
// Percussion instruments on 10th MIDI channel ( channel[9] for programmers ).

// MIDI Defines
enum EMidiDrum
{
   GM_Drum_35_B0_Acoustic_Bassdrum = 35,
   GM_Drum_36_C1_Bass_Drum,
   GM_Drum_37_Db1_RimShot,
   GM_Drum_38_D1_SnareDrum1,
   GM_Drum_39_Eb1_HandClap,
   GM_Drum_40_E1_SnareDrum2,
   GM_Drum_41_F1_Low_Floor_Tom,
   GM_Drum_42_Gb1_Closed_HiHat,
   GM_Drum_43_G1_High_Floor_Tom,
   GM_Drum_44_Ab1_Pedal_HiHat,
   GM_Drum_45_A1_Low_Tom,
   GM_Drum_46_Bb1_Open_HiHat,
   GM_Drum_47_B1_Low_Mid_Tom,
   GM_Drum_48_C2_Hi_Mid_Tom,
   GM_Drum_49_Db_Crash_Becken1,
   GM_Drum_50_D2_High_Tom,
   GM_Drum_51_Eb2_Ride_Becken1,
   GM_Drum_52_E2_China_Crash_Becken,
   GM_Drum_53_F2_Ride2_Glocke,
   GM_Drum_54_Gb2_Tamburin,
   GM_Drum_55_G2_Splash_Becken,
   GM_Drum_56_Ab2_Kuhglocke,
   GM_Drum_57_A2_Crash_Becken2,
   GM_Drum_58_B2_Vibraslap,
   GM_Drum_59_H2_Ride_Becken2,
   GM_Drum_60_C3_Hi_Bongo,
   GM_Drum_61_Db3_Low_Bongo,
   GM_Drum_62_D3_gedaempfte_Hi_Conga,
   GM_Drum_63_Eb_offene_Hi_Conga,
   GM_Drum_64_E3_tiefe_Conga,
   GM_Drum_65_F3_hohe_Timbale,
   GM_Drum_66_Gb3_tiefe_Timbale,
   GM_Drum_67_G3_hohe_Agogo,
   GM_Drum_68_Ab3_tiefe_Agogo,
   GM_Drum_69_A3_Cabasa,
   GM_Drum_70_B3_Maracas,
   GM_Drum_71_H3_Trillerpfeife_kurz,
   GM_Drum_72_C4_Trillerpfeife_lang,
   GM_Drum_73_Db4_Guiro_kurz,
   GM_Drum_74_D4_Guiro_lang,
   GM_Drum_75_Eb4_Clave,
   GM_Drum_76_E4_hoher_Holzblock,
   GM_Drum_77_F4_tiefer_Holzblock,
   GM_Drum_78_Gb4_gedaempfte_Cuica,
   GM_Drum_79_G4_offene_Cuica,
   GM_Drum_80_Ab4_gedaempfte_Triangel,
   GM_Drum_81_A4_offene_Triangel,
   GM_Drum_82_B4_Shaker,
   GM_Drum_83_H4_Jingle_Bells,
   GM_Drum_84_C5_Belltree,
   GM_Drum_Unknown
};

// MIDI Defines
inline std::string
EMidiGM_Drum_toString( int drum )
{
   switch ( drum )
   {
      case GM_Drum_84_C5_Belltree: return "GM_Drum_84_C5_Belltree";
      default: return "GM_Drum_Unknown";
   }
}


// MIDI CC Defines
/*
   00000000 00 0 Bank Select 0-127 MSB
   00000001 01 1 * Modulation wheel 0-127 MSB
   00000010 02 2 Breath control 0-127 MSB
   00000011 03 3 Undefined 0-127 MSB
   00000100 04 4 Foot controller 0-127 MSB
   00000101 05 5 Portamento time 0-127 MSB
   00000110 06 6 Data Entry 0-127 MSB
   00000111 07 7 * Channel Volume (formerly Main Volume) 0-127 MSB
   00001000 08 8 Balance 0-127 MSB
   Standard MIDI file format, updated http://www.music.mcgill.ca/~ich/classes/mumt306/StandardMIDIfilefor...
   10 von 20 11.09.2021, 19:11
   00001001 09 9 Undefined 0-127 MSB
   00001010 0A 10 * Pan 0-127 MSB
   00001011 0B 11 * Expression Controller 0-127 MSB
   00001100 0C 12 Effect control 1 0-127 MSB
   00001101 0D 13 Effect control 2 0-127 MSB
   00001110 0E 14 Undefined 0-127 MSB
   00001111 0F 15 Undefined 0-127 MSB
   00010000 10 16 General Purpose Controller #1 0-127 MSB
   00010001 11 17 General Purpose Controller #2 0-127 MSB
   00010010 12 18 General Purpose Controller #3 0-127 MSB
   00010011 13 19 General Purpose Controller #4 0-127 MSB
   00010100 14 20 Undefined 0-127 MSB
   00010101 15 21 Undefined 0-127 MSB
   00010110 16 22 Undefined 0-127 MSB
   00010111 17 23 Undefined 0-127 MSB
   00011000 18 24 Undefined 0-127 MSB
   00011001 19 25 Undefined 0-127 MSB
   00011010 1A 26 Undefined 0-127 MSB
   00011011 1B 27 Undefined 0-127 MSB
   00011100 1C 28 Undefined 0-127 MSB
   00011101 1D 29 Undefined 0-127 MSB
   00011110 1E 30 Undefined 0-127 MSB
   00011111 1F 31 Undefined 0-127 MSB
   00100000 20 32 Bank Select 0-127 LSB
   00100001 21 33 Modulation wheel 0-127 LSB
   00100010 22 34 Breath control 0-127 LSB
   00100011 23 35 Undefined 0-127 LSB
   00100100 24 36 Foot controller 0-127 LSB
   00100101 25 37 Portamento time 0-127 LSB
   00100110 26 38 Data entry 0-127 LSB
   00100111 27 39 Channel Volume (formerly Main Volume) 0-127 LSB
   00101000 28 40 Balance 0-127 LSB
   00101001 29 41 Undefined 0-127 LSB
   00101010 2A 42 Pan 0-127 LSB
   00101011 2B 43 Expression Controller 0-127 LSB
   00101100 2C 44 Effect control 1 0-127 LSB
   00101101 2D 45 Effect control 2 0-127 LSB
   00101110 2E 46 Undefined 0-127 LSB
   00101111 2F 47 Undefined 0-127 LSB
   00110000 30 48 General Purpose Controller #1 0-127 LSB
   00110001 31 49 General Purpose Controller #2 0-127 LSB
   00110010 32 50 General Purpose Controller #3 0-127 LSB
   00110011 33 51 General Purpose Controller #4 0-127 LSB
   00110100 34 52 Undefined 0-127 LSB
   00110101 35 53 Undefined 0-127 LSB
   00110110 36 54 Undefined 0-127 LSB
   00110111 37 55 Undefined 0-127 LSB
   00111000 38 56 Undefined 0-127 LSB
   00111001 39 57 Undefined 0-127 LSB
   00111010 3A 58 Undefined 0-127 LSB
   00111011 3B 59 Undefined 0-127 LSB
   00111100 3C 60 Undefined 0-127 LSB
   00111101 3D 61 Undefined 0-127 LSB
   00111110 3E 62 Undefined 0-127 LSB
   00111111 3F 63 Undefined 0-127 LSB
   01000000 40 64 * Damper pedal on/off (Sustain) <63=off >64=on
   01000001 41 65 Portamento on/off <63=off >64=on
   01000010 42 66 Sustenuto on/off <63=off >64=on
   01000011 43 67 Soft pedal on/off <63=off >64=on
   01000100 44 68 Legato Footswitch <63=off >64=on
   01000101 45 69 Hold 2 <63=off >64=on
   01000110 46 70 Sound Controller 1 (Sound Variation) 0-127 LSB
   01000111 47 71 Sound Controller 2 (Timbre) 0-127 LSB
   01001000 48 72 Sound Controller 3 (Release Time) 0-127 LSB
   01001001 49 73 Sound Controller 4 (Attack Time) 0-127 LSB
   01001010 4A 74 Sound Controller 5 (Brightness) 0-127 LSB
   01001011 4B 75 Sound Controller 6 0-127 LSB
   01001100 4C 76 Sound Controller 7 0-127 LSB
   01001101 4D 77 Sound Controller 8 0-127 LSB
   01001110 4E 78 Sound Controller 9 0-127 LSB
   01001111 4F 79 Sound Controller 10 0-127 LSB
   01010000 50 80 General Purpose Controller #5 0-127 LSB
   01010001 51 81 General Purpose Controller #6 0-127 LSB
   01010010 52 82 General Purpose Controller #7 0-127 LSB
   01010011 53 83 General Purpose Controller #8 0-127 LSB
   01010100 54 84 Portamento Control 0-127 Source Note
   01010101 55 85 Undefined 0-127 LSB
   01010110 56 86 Undefined 0-127 LSB
   01010111 57 87 Undefined 0-127 LSB
   01011000 58 88 Undefined 0-127 LSB
   01011001 59 89 Undefined 0-127 LSB
   01011010 5A 90 Undefined 0-127 LSB
   01011011 5B 91 Effects 1 Depth 0-127 LSB
   01011100 5C 92 Effects 2 Depth 0-127 LSB
   01011101 5D 93 Effects 3 Depth 0-127 LSB
   01011110 5E 94 Effects 4 Depth 0-127 LSB
   01011111 5F 95 Effects 5 Depth 0-127 LSB
   01100000 60 96 Data entry +1 N/A
   01100001 61 97 Data entry -1 N/A
   01100010 62 98 Non-Registered Parameter Number LSB 0-127 LSB
   01100011 63 99 Non-Registered Parameter Number MSB 0-127 MSB
   01100100 64 100 * Registered Parameter Number LSB 0-127 LSB
   01100101 65 101 * Registered Parameter Number MSB 0-127 MSB
   01100110 66 102 Undefined ?
   01100111 67 103 Undefined ?
   01101000 68 104 Undefined ?
   01101001 69 105 Undefined ?
   01101010 6A 106 Undefined ?
   01101011 6B 107 Undefined ?
   01101100 6C 108 Undefined ?
   01101101 6D 109 Undefined ?
   01101110 6E 110 Undefined ?
   01101111 6F 111 Undefined ?
   01110000 70 112 Undefined ?
   01110001 71 113 Undefined ?
   01110010 72 114 Undefined ?
   01110011 73 115 Undefined ?
   01110100 74 116 Undefined ?
   01110101 75 117 Undefined ?
   01110110 76 118 Undefined ?
   01110111 77 119 Undefined ?
   01111000 78 120 All Sound Off 0
   01111001 79 121 * Reset All Controllers 0
   01111010 7A 122 Local control on/off 0=off 127=on
   01111011 7B 123 * All notes off 0
   01111100 7C 124 Omni mode off (+ all notes off) 0
   01111101 7D 125 Omni mode on (+ all notes off) 0
   01111110 7E 126 Poly mode on/off (+ all notes off) **
   01111111 7F 127 Poly mode on (incl mono=off +all notes off) 0
*/
enum EMidiCC
{
   CC_0_BankSelect = 0, // 0-127 MSB
   CC_1_ModulationWheel,// 0-127 MSB
   CC_2_BreathControl,  // 0-127 MSB
   CC_3_Undefined,      // 0-127 MSB
   CC_4_FootController, // 0-127 MSB
   CC_5_PortamentoTime, // 0-127 MSB
   CC_6_DataEntry,      // 0-127 MSB
   CC_7_Volume,         // 0-127 MSB (ChannelVolume, formerly MainVolume)
   CC_8_Balance,        // 0-127 MSB
   CC_9_Undefined,      // 0-127 MSB
   CC_10_Pan,           // 0-127 MSB
   CC_11_ExpressionController, // 0-127 MSB
   CC_12_EffectControl1, // 0-127 MSB
   CC_13_EffectControl2, // 0-127 MSB
   CC_14_Undefined, // 0-127 MSB
   CC_15_Undefined, // 0-127 MSB
   CC_16_GeneralPurposeController1, // 0-127 MSB
   CC_17_GeneralPurposeController2, // 0-127 MSB
   CC_18_GeneralPurposeController3, // 0-127 MSB
   CC_19_GeneralPurposeController4, // 0-127 MSB
   CC_20_Undefined, // 0-127 MSB
   CC_21_Undefined, // 0-127 MSB
   CC_22_Undefined, // 0-127 MSB
   CC_23_Undefined, // 0-127 MSB
   CC_24_Undefined, // 0-127 MSB
   CC_25_Undefined, // 0-127 MSB
   CC_26_Undefined, // 0-127 MSB
   CC_27_Undefined, // 0-127 MSB
   CC_28_Undefined, // 0-127 MSB
   CC_29_Undefined, // 0-127 MSB
   CC_30_Undefined, // 0-127 MSB
   CC_31_Undefined, // 0-127 MSB
   CC_32_BankSelect, // 0-127 LSB
   CC_33_ModulationWheel, // 0-127 LSB
   CC_34_BreathControl, // 0-127 LSB
   CC_35_Undefined, // 0-127 LSB
   CC_36_FootController, // 0-127 LSB
   CC_37_PortamentoTime, // 0-127 LSB
   CC_38_DataEntry, // 0-127 LSB
   CC_39_Volume, // (Channel Volume, formerly Main Volume) 0-127 LSB
   CC_40_Balance, // 0-127 LSB
   CC_41_Undefined, // 0-127 LSB
   CC_42_Pan, // 0-127 LSB
   CC_43_ExpressionController, // 0-127 LSB
   CC_44_EffectControl1, // 0-127 LSB
   CC_45_EffectControl2, // 0-127 LSB
   CC_46_Undefined, // 0-127 LSB
   CC_47_Undefined, // 0-127 LSB
   CC_48_GeneralPurposeController1, // 0-127 LSB
   CC_49_GeneralPurposeController2, // 0-127 LSB
   CC_50_GeneralPurposeController3, // 0-127 LSB
   CC_51_GeneralPurposeController4, // 0-127 LSB
   CC_52_Undefined, // 0-127 LSB
   CC_53_Undefined, // 0-127 LSB
   CC_54_Undefined, // 0-127 LSB
   CC_55_Undefined, // 0-127 LSB
   CC_56_Undefined, // 0-127 LSB
   CC_57_Undefined, // 0-127 LSB
   CC_58_Undefined, // 0-127 LSB
   CC_59_Undefined, // 0-127 LSB
   CC_60_Undefined, // 0-127 LSB
   CC_61_Undefined, // 0-127 LSB
   CC_62_Undefined, // 0-127 LSB
   CC_63_Undefined, // 0-127 LSB
   CC_64_SustainPedal, // on/off (damper pedal) sustain off <= 63, sustain on >=64 ( right pedal on piano )
   CC_65_Portamento, // on/off <63=off >64=on
   CC_66_Sustenuto, // on/off <63=off >64=on
   CC_67_SoftPedal, // on/off <63=off >64=on ( left pedal on piano )
   CC_68_LegatoFootswitch, // <63=off >64=on
   CC_69_Hold2, // <63=off >64=on
   CC_70_SoundController1, // (Sound Variation) 0-127 LSB
   CC_71_SoundController2, // (Timbre) 0-127 LSB
   CC_72_SoundController3, // (Release Time) 0-127 LSB
   CC_73_SoundController4, // (Attack Time) 0-127 LSB
   CC_74_SoundController5, // (Brightness) 0-127 LSB
   CC_75_SoundController6, // 0-127 LSB
   CC_76_SoundController7, // 0-127 LSB
   CC_77_SoundController8, // 0-127 LSB
   CC_78_SoundController9, // 0-127 LSB
   CC_79_SoundController10, // 0-127 LSB
   CC_80_GeneralPurposeController5, // 0-127 LSB
   CC_81_GeneralPurposeController6, // 0-127 LSB
   CC_82_GeneralPurposeController7, // 0-127 LSB
   CC_83_GeneralPurposeController8, // 0-127 LSB
   CC_84_PortamentoControl, // 0-127 Source Note
   CC_85_Undefined, // 0-127 LSB
   CC_86_Undefined, // 0-127 LSB
   CC_87_Undefined, // 0-127 LSB
   CC_88_Undefined, // 0-127 LSB
   CC_89_Undefined, // 0-127 LSB
   CC_90_Undefined, // 0-127 LSB
   CC_91_Effects1Depth, // 0-127 LSB
   CC_92_Effects2Depth, // 0-127 LSB
   CC_93_Effects3Depth, // 0-127 LSB
   CC_94_Effects4Depth, // 0-127 LSB
   CC_95_Effects5Depth, // 0-127 LSB
   CC_96_DataEntry, // +1 N/A
   CC_97_DataEntry, // -1 N/A
   CC_98_NonRegisteredParameterNumber, // LSB 0-127 LSB
   CC_99_NonRegisteredParameterNumber, // MSB 0-127 MSB
   CC_100_RegisteredParameterNumber, // LSB 0-127 LSB
   CC_101_RegisteredParameterNumber, // MSB 0-127 MSB
   CC_102_Undefined, // ?
   CC_103_Undefined, // ?
   CC_104_Undefined, // ?
   CC_105_Undefined, // ?
   CC_106_Undefined, // ?
   CC_107_Undefined, // ?
   CC_108_Undefined, // ?
   CC_109_Undefined, // ?
   CC_110_Undefined, // ?
   CC_111_Undefined, // ?
   CC_112_Undefined, // ?
   CC_113_Undefined, // ?
   CC_114_Undefined, // ?
   CC_115_Undefined, // ?
   CC_116_Undefined, // ?
   CC_117_Undefined, // ?
   CC_118_Undefined, // ?
   CC_119_Undefined, // ?
   CC_120_AllSoundOff, // 0
   CC_121_ResetAllControllers, // 0
   CC_122_LocalControl, // on/off 0=off 127=on
   CC_123_AllNotesOff, // 0
   CC_124_Omni_mode_off, // (+ all notes off) 0
   CC_125_Omni_mode_on, // (+ all notes off) 0
   CC_126_Poly_mode_on_off, // (+ all notes off)
   CC_127_Poly_mode_on, // (+ mono off + all notes off)
   CC_Unknown
};

// MIDI Defines
inline std::string
EMidiCC_toString( int cc )
{
   switch ( cc )
   {
      case CC_0_BankSelect: return "CC_0_BankSelect";
      case CC_1_ModulationWheel: return "CC_1_ModulationWheel";
      case CC_2_BreathControl: return "CC_2_BreathControl";
      case CC_3_Undefined: return "CC_3_Undefined";
      case CC_4_FootController: return "CC_4_FootController";
      case CC_5_PortamentoTime: return "CC_5_PortamentoTime";
      case CC_6_DataEntry: return "CC_6_DataEntry";
      case CC_7_Volume: return "CC_7_Volume";
      case CC_8_Balance: return "CC_8_Balance";
      case CC_9_Undefined: return "CC_9_Undefined";
      case CC_10_Pan: return "CC_10_Pan";
      case CC_11_ExpressionController: return "CC_11_ExpressionController";
      case CC_12_EffectControl1: return "CC_12_EffectControl1";
      case CC_13_EffectControl2: return "CC_13_EffectControl2";
      case CC_14_Undefined: return "CC_14_Undefined";
      case CC_15_Undefined: return "CC_15_Undefined";
      case CC_16_GeneralPurposeController1: return "CC_16_GeneralPurposeController1";
      case CC_17_GeneralPurposeController2: return "CC_16_GeneralPurposeController2";
      case CC_18_GeneralPurposeController3: return "CC_16_GeneralPurposeController3";
      case CC_19_GeneralPurposeController4: return "CC_16_GeneralPurposeController4";
      case CC_20_Undefined: return "CC_20_Undefined";
      case CC_21_Undefined: return "CC_21_Undefined";
      case CC_22_Undefined: return "CC_22_Undefined";
      case CC_23_Undefined: return "CC_23_Undefined";
      case CC_24_Undefined: return "CC_24_Undefined";
      case CC_25_Undefined: return "CC_25_Undefined";
      case CC_26_Undefined: return "CC_26_Undefined";
      case CC_27_Undefined: return "CC_27_Undefined";
      case CC_28_Undefined: return "CC_28_Undefined";
      case CC_29_Undefined: return "CC_29_Undefined";
      case CC_30_Undefined: return "CC_30_Undefined";
      case CC_31_Undefined: return "CC_31_Undefined";
      case CC_32_BankSelect: return "CC_32_BankSelect";
      case CC_33_ModulationWheel: return "CC_33_ModulationWheel";
      case CC_34_BreathControl: return "CC_34_BreathControl";
      case CC_35_Undefined: return "CC_35_Undefined";
      case CC_36_FootController: return "CC_36_FootController";
      case CC_37_PortamentoTime: return "CC_37_PortamentoTime";
      case CC_38_DataEntry: return "CC_38_DataEntry";
      case CC_39_Volume: return "CC_39_Volume";
      case CC_40_Balance: return "CC_40_Balance";
      case CC_41_Undefined: return "CC_41_Undefined";
      case CC_42_Pan: return "CC_42_Pan";
      case CC_43_ExpressionController: return "CC_43_ExpressionController";
      case CC_44_EffectControl1: return "CC_44_EffectControl1";
      case CC_45_EffectControl2: return "CC_45_EffectControl2";
      case CC_46_Undefined: return "CC_46_Undefined";
      case CC_47_Undefined: return "CC_47_Undefined";
      case CC_48_GeneralPurposeController1: return "CC_48_GeneralPurposeController1";
      case CC_49_GeneralPurposeController2: return "CC_49_GeneralPurposeController2";
      case CC_50_GeneralPurposeController3: return "CC_50_GeneralPurposeController3";
      case CC_51_GeneralPurposeController4: return "CC_51_GeneralPurposeController4";
      case CC_52_Undefined: return "CC_52_Undefined";
      case CC_53_Undefined: return "CC_53_Undefined";
      case CC_54_Undefined: return "CC_54_Undefined";
      case CC_55_Undefined: return "CC_55_Undefined";
      case CC_56_Undefined: return "CC_56_Undefined";
      case CC_57_Undefined: return "CC_57_Undefined";
      case CC_58_Undefined: return "CC_58_Undefined";
      case CC_59_Undefined: return "CC_59_Undefined";
      case CC_60_Undefined: return "CC_60_Undefined";
      case CC_61_Undefined: return "CC_61_Undefined";
      case CC_62_Undefined: return "CC_62_Undefined";
      case CC_63_Undefined: return "CC_63_Undefined";
      case CC_64_SustainPedal: return "CC_64_SustainPedal";
      case CC_65_Portamento: return "CC_65_Portamento";
      case CC_66_Sustenuto: return "CC_66_Sustenuto";
      case CC_67_SoftPedal: return "CC_67_SoftPedal";
      case CC_68_LegatoFootswitch: return "CC_68_LegatoFootswitch";
      case CC_69_Hold2: return "CC_69_Hold2";
      case CC_70_SoundController1: return "CC_70_SoundController1";
      case CC_71_SoundController2: return "CC_71_SoundController2";
      case CC_72_SoundController3: return "CC_72_SoundController3";
      case CC_73_SoundController4: return "CC_73_SoundController4";
      case CC_74_SoundController5: return "CC_74_SoundController5";
      case CC_75_SoundController6: return "CC_75_SoundController6";
      case CC_76_SoundController7: return "CC_76_SoundController7";
      case CC_77_SoundController8: return "CC_77_SoundController8";
      case CC_78_SoundController9: return "CC_78_SoundController9";
      case CC_79_SoundController10: return "CC_79_SoundController10";
      case CC_80_GeneralPurposeController5: return "CC_80_GeneralPurposeController5";
      case CC_81_GeneralPurposeController6: return "CC_81_GeneralPurposeController6";
      case CC_82_GeneralPurposeController7: return "CC_82_GeneralPurposeController7";
      case CC_83_GeneralPurposeController8: return "CC_83_GeneralPurposeController8";
      case CC_84_PortamentoControl: return "CC_84_PortamentoControl";
      case CC_85_Undefined: return "CC_85_Undefined";
      case CC_86_Undefined: return "CC_86_Undefined";
      case CC_87_Undefined: return "CC_87_Undefined";
      case CC_88_Undefined: return "CC_88_Undefined";
      case CC_89_Undefined: return "CC_89_Undefined";
      case CC_90_Undefined: return "CC_90_Undefined";
      case CC_91_Effects1Depth: return "CC_91_Effects1Depth";
      case CC_92_Effects2Depth: return "CC_92_Effects2Depth";
      case CC_93_Effects3Depth: return "CC_93_Effects3Depth";
      case CC_94_Effects4Depth: return "CC_94_Effects4Depth";
      case CC_95_Effects5Depth: return "CC_95_Effects5Depth";
      case CC_96_DataEntry: return "CC_96_DataEntry";
      case CC_97_DataEntry: return "CC_97_DataEntry";
      case CC_98_NonRegisteredParameterNumber: return "CC_98_NonRegisteredParameterNumber";
      case CC_99_NonRegisteredParameterNumber: return "CC_99_NonRegisteredParameterNumber";
      case CC_100_RegisteredParameterNumber: return "CC_100_RegisteredParameterNumber";
      case CC_101_RegisteredParameterNumber: return "CC_101_RegisteredParameterNumber";
      case CC_102_Undefined: return "CC_102_Undefined";
      case CC_103_Undefined: return "CC_103_Undefined";
      case CC_104_Undefined: return "CC_104_Undefined";
      case CC_105_Undefined: return "CC_105_Undefined";
      case CC_106_Undefined: return "CC_106_Undefined";
      case CC_107_Undefined: return "CC_107_Undefined";
      case CC_108_Undefined: return "CC_108_Undefined";
      case CC_109_Undefined: return "CC_109_Undefined";
      case CC_110_Undefined: return "CC_110_Undefined";
      case CC_111_Undefined: return "CC_111_Undefined";
      case CC_112_Undefined: return "CC_112_Undefined";
      case CC_113_Undefined: return "CC_113_Undefined";
      case CC_114_Undefined: return "CC_114_Undefined";
      case CC_115_Undefined: return "CC_115_Undefined";
      case CC_116_Undefined: return "CC_116_Undefined";
      case CC_117_Undefined: return "CC_117_Undefined";
      case CC_118_Undefined: return "CC_118_Undefined";
      case CC_119_Undefined: return "CC_119_Undefined";
      case CC_120_AllSoundOff: return "CC_120_AllSoundOff";
      case CC_121_ResetAllControllers: return "CC_121_ResetAllControllers";
      case CC_122_LocalControl: return "CC_122_LocalControl";
      case CC_123_AllNotesOff: return "CC_123_AllNotesOff";
      case CC_124_Omni_mode_off: return "CC_124_Omni_mode_off";
      case CC_125_Omni_mode_on: return "CC_125_Omni_mode_on";
      case CC_126_Poly_mode_on_off: return "CC_126_Poly_mode_on_off";
      case CC_127_Poly_mode_on: return "CC_127_Poly_mode_on";
      default: return "CC_Unknown";
   }
}

// =======================================================================
struct MidiTools
// =======================================================================
{
   DE_CREATE_LOGGER("de.MidiTools")

   //
   // Convert a MIDI keynote to a string
   //

   static std::string
   noteStr( int key );

   static bool
   isBlackPianoKey( int semi )
   {

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

   static size_t
   readU16( uint8_t const* beg, uint8_t const* end, uint16_t & value ) noexcept;

   static size_t
   readU16_be( uint8_t const* beg, uint8_t const* end, uint16_t & value ) noexcept;

   static size_t
   readU32( uint8_t const* beg, uint8_t const* end, uint32_t & value ) noexcept;

   static size_t
   readU32_be( uint8_t const* beg, uint8_t const* end, uint32_t & value ) noexcept;

   //
   // VLQ - Variable-Length Quantity
   //
   static uint32_t
   parseVLQ( uint8_t const* beg, uint8_t const* end, uint32_t & value );

   static void
   testVLQ( int k, std::vector< uint8_t > dat, uint32_t n, uint32_t expectedBytes, uint32_t expectedValue );

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

   static void
   test();

   static uint32_t
   getChannelColor( int channel );
};

} // end namespace midi
} // end namespace de
