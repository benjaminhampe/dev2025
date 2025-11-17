#pragma once
#include <cstdint>
#include <string>
#include <de_nanovg.h>

// 📊
struct Partial
{
    float amplitude; // in [0,1]
    float detune; // max. detune in cent
    float phase = 0.0f;
    float phaseIncrement = 1.0f;
    int16_t partial; // 1,2,N x Even and Odd multiples of BaseFrequency = Harmonics/Partials
    bool audible = true;
    bool editable = true;
    NVGcolor color;

    std::string str() const
    {
        return std::to_string(partial);
    }

    float A() const
    {
        return amplitude;
    }
};
