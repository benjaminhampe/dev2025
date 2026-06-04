#pragma once
#include <de/midi/GeneralMidi.h>
#include <DarkImage.h>

namespace de {
namespace midi {

// =======================================================================
struct MidiTools
// =======================================================================
{
    static std::string
    noteStr( int key );

    static bool
    isBlackPianoKey( int semi );

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

