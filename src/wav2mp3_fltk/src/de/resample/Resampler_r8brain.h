#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

class Resampler_r8brain
{
public:
    Resampler_r8brain();
    ~Resampler_r8brain();
    bool resample(const Sound & src, Sound & dst, int32_t dstRate);

private:
    struct Impl;
    Impl* _d = nullptr;
};

} // end namespace sound.
} // end namespace de.
