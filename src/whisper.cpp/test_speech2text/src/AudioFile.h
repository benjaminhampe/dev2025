#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>

struct AudioFile
{
    enum
    {
        None = 0,
        ST_Interleaved = (1 << 1),
        ST_S8  = (1 << 2),
        ST_S16 = (1 << 3),
        ST_S24 = (1 << 4),
        ST_S32 = (1 << 5),
        ST_F32 = (1 << 6),
        ST_F64 = (1 << 7)
    };

    uint64_t frame_count = 0;
    uint32_t sample_rate = 0;
    uint16_t channels = 0;
    uint16_t sample_type = 0;
    std::vector< uint8_t > samples;

    double durationInSec() const
    {
        if (sample_rate < 1)
        {
            return 0.0;
        }

        return double(frame_count) / double(sample_rate);
    }

    float maximum() const
    {
        float maxv = 0.0f;

        auto p = reinterpret_cast<const float*>(samples.data());

        for (size_t i = 0; i < frame_count * channels; ++i)
        {
            float s = *p++;
            maxv = std::max(maxv, fabs(s));
        }
        return maxv;
    }

    std::string str() const
    {
        std::ostringstream o; o <<
        "ch(" << channels << "), "
        "sr(" << sample_rate << "), "
        "fc(" << frame_count << "), "
        "st(" << sample_type << "), "
        "t(" << maximum() << "), "
        "t(" << durationInSec() << "), "
        "samples(" << samples.size() << ")";
        return o.str();
    }
};


