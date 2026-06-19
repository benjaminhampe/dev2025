/*
 *  ShortMidiMessage.h
 *
 *  Copyright (c) 2022-2026 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */
#pragma once
#include <DarkImage.h>
#include <de/midi/GeneralMidi.h>

namespace de {
namespace midi {

// =======================================================
struct ShortMidiMessage // 🎹
// =======================================================
{
    uint8_t m_status;
    uint8_t m_data1;
    uint8_t m_data2;
    uint8_t m_data3;

    ShortMidiMessage();
    ShortMidiMessage(uint8_t status,
                     uint8_t data1,
                     uint8_t data2 = 0,
                     uint8_t data3 = 0);

    uint32_t pack() const;
    std::string str() const;
    uint8_t midiCommand() const { return m_status & 0xF0; }
    uint8_t midiChannel() const { return m_status & 0x0F; }
    // All_Sound_Off — CC 0x78
    // Reset_All_Controllers — CC 0x79
    // All_Notes_Off — CC 0x7B
    static ShortMidiMessage allSoundsOff(int channel);
    static ShortMidiMessage resetAllControllers(int channel);
    static ShortMidiMessage allNotesOff(int channel);
    static ShortMidiMessage CC64_sustainPedal(int channel, bool pedalDown);
    static ShortMidiMessage CC1_modWheel(int channel, int value_0_127);
    static ShortMidiMessage CC1_modWheel14bit(int channel, int value_0_16383);
    static ShortMidiMessage CC16_pitchBend(int channel, float semitones);
    static ShortMidiMessage pitchBend(int channel, int value14bit);
 // static ShortMidiMessage CC16_pitchBend(int channel, int value_0_16383);
};

// using ShortMidiMessageListener =
//     std::function<void(const ShortMidiMessage&)>;

} // end namespace midi.
} // end namespace de.
