#pragma once
#include <cstdint>
#include <string>

struct SineOvertone
{
    float amplitude;
    float frequency; // = octave + semitone + centDetune;
    float phase;
    float phaseIncrement;

    int octave;
    int semitone;
    float detune; // in cent

    std::string name; // e.g. A4

    std::string str() const
    {
        static const char* noteNames[12] = {
            "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
        };

        std::string s = noteNames[semitone % 12];
        s += std::to_string(octave);
        return s;
    }
};
