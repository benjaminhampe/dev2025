#pragma once

#include "bogaudio.hpp"
#include "mixer_expander.hpp"
#include "bogaudio_dsp_signal.hpp"

extern Model* modelMix8;
extern Model* modelMix8x;

namespace bogaudio {

typedef MixerExpanderMessage<8> Mix8ExpanderMessage;

} // namespace bogaudio
