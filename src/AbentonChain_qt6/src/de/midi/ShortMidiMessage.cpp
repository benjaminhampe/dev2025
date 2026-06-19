/*
 *  ShortMidiMessage.h
 *
 *  Copyright (c) 2022-2026 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */
#include "ShortMidiMessage.h"
// #include <DarkImage.h>
// #include <de/midi/GeneralMidi.h>

namespace de {
namespace midi {

// =======================================================
// 🎹
// =======================================================
ShortMidiMessage::ShortMidiMessage()
    : m_status(0)
    , m_data1(0)
    , m_data2(0)
    , m_data3(0)
{}

ShortMidiMessage::ShortMidiMessage( uint8_t status, uint8_t data1,
                                    uint8_t data2, uint8_t data3)
    : m_status(status)
    , m_data1(data1)
    , m_data2(data2)
    , m_data3(data3)
{}

uint32_t ShortMidiMessage::pack() const
{
    return (uint32_t(m_data3) << 24)
         | (uint32_t(m_data2) << 16)
         | (uint32_t(m_data1) << 8)
         |  uint32_t(m_status);
}

std::string ShortMidiMessage::str() const
{
    std::ostringstream o; o <<
    dbHex( m_status )<< " " <<
    dbHex( m_data1 ) << " " <<
    dbHex( m_data2 ) << " " <<
    dbHex( m_data3 );
    return o.str();
}

// All_Sound_Off — CC 0x78
// Reset_All_Controllers — CC 0x79
// All_Notes_Off — CC 0x7B

// static
ShortMidiMessage ShortMidiMessage::allSoundsOff(int channel)
{
    ShortMidiMessage m;
    m.m_status = 0xB0 | (channel & 0x0F);  // Control Change Event
    m.m_data1  = 0x78;                     // CC_120_AllSoundOff
    m.m_data2  = 0;                        // ON or OFF
    m.m_data3  = 0;
    return m;
}
// static
ShortMidiMessage ShortMidiMessage::resetAllControllers(int channel)
{
    ShortMidiMessage m;
    m.m_status = 0xB0 | (channel & 0x0F);  // Control Change Event
    m.m_data1  = 0x79;                     // CC_121_ResetAllControllers
    m.m_data2  = 0;                        // ON or OFF
    m.m_data3  = 0;
    return m;
}
// static
ShortMidiMessage ShortMidiMessage::allNotesOff(int channel)
{
    ShortMidiMessage m;
    m.m_status = 0xB0 | (channel & 0x0F);  // Control Change Event
    m.m_data1  = 0x7B;                     // CC_123_AllNotesOff
    m.m_data2  = 0;                        // ON or OFF
    m.m_data3  = 0;
    return m;
}
// static
ShortMidiMessage ShortMidiMessage::CC64_sustainPedal(int channel, bool pedalDown)
{
    ShortMidiMessage m;
    m.m_status = 0xB0 | (channel & 0x0F);  // Control Change Event
    m.m_data1  = 0x40;                     // CC_64_SustainPedal (always 7-bit)
    m.m_data2  = pedalDown ? 127 : 0;      // ON or OFF
    m.m_data3  = 0;
    return m;
}
// static
ShortMidiMessage ShortMidiMessage::CC1_modWheel(int channel, int value_0_127)
{
    const int value = std::clamp(value_0_127,0,127);

    ShortMidiMessage m;
    m.m_status = 0xB0 | (channel & 0x0F);  // CC
    m.m_data1  = CC_1_ModulationWheel; // Mod Wheel
    m.m_data2  = static_cast<uint8_t>(value); // MSB
    m.m_data3  = 0; // LSB
    return m;
}
// static
ShortMidiMessage ShortMidiMessage::CC1_modWheel14bit(int channel, int value_0_16383)
{
    const int value = std::clamp(value_0_16383,0,16383); // 14-bit

    ShortMidiMessage m;
    m.m_status = 0xB0 | (channel & 0x0F);  // CC
    m.m_data1  = CC_1_ModulationWheel; // CC1 Mod Wheel
    m.m_data2  = static_cast<uint8_t>((value >> 7) & 0x7F); // MSB
    m.m_data3  = static_cast<uint8_t>(value & 0x7F); // LSB
    return m;
}
// static
ShortMidiMessage ShortMidiMessage::CC16_pitchBend(int channel, float semitones)
{
    // implied standard pitchbend range
    const float minSemis = -12.0f;
    const float maxSemis = +12.0f;

    // clamp input
    semitones = ::de::clampf(semitones, minSemis, maxSemis);

    // normalize to 0…1
    const float norm = (semitones - minSemis) / (maxSemis - minSemis);

    // convert to 14-bit
    const int v14 = std::clamp( int(norm * 16383.0f + 0.5f), 0, 16383);
    const uint8_t msb = (v14 >> 7) & 0x7F;
    const uint8_t lsb = (v14 & 0x7F);

    ShortMidiMessage msg;
    msg.m_status = uint8_t(0xB0 | (channel & 0x0F));
    msg.m_data1  = CC_16_GeneralPurposeController1;
    msg.m_data2  = msb;
    msg.m_data3  = lsb;
    return msg;
}
// static
ShortMidiMessage ShortMidiMessage::pitchBend(int channel, int value14bit)
{
    // clamp to valid 14-bit range
    if (value14bit < 0) value14bit = 0;
    if (value14bit > 16383) value14bit = 16383;

    ShortMidiMessage m{};
    m.m_status = 0xE0 | (channel & 0x0F);
    m.m_data1  = value14bit & 0x7F;          // LSB
    m.m_data2  = (value14bit >> 7) & 0x7F;   // MSB
    m.m_data3  = 0;
    return m;
}
/*
// static
ShortMidiMessage ShortMidiMessage::CC16_pitchBend(int channel, int value_0_16383)
{
    const int value = std::clamp(value_0_16383,0,16383); // 14-bit

    // normalize to -1.0 … +1.0
    const double norm = (value - 8192) / 8192.0;

    // scale to ±1 octave (still -1…+1, but explicit)
    const double oct = norm; // one octave range

    // convert to 0…16383 again for CC encoding
    int v14 = int((oct * 0.5 + 0.5) * 16383.0);
    if (v14 < 0)      v14 = 0;
    if (v14 > 16383)  v14 = 16383;

    const uint8_t msb = (v14 >> 7) & 0x7F;
    const uint8_t lsb = (v14 & 0x7F);

    ShortMidiMessage msg;
    msg.m_status = uint8_t(0xB0 | (channel & 0x0F)); // CC
    msg.m_data1  = 16;   // CC16 surrogate
    msg.m_data2  = msb;  // MSB
    msg.m_data3  = lsb;  // LSB (your struct supports it)
    return msg;
}
*/

} // end namespace midi.
} // end namespace de.
