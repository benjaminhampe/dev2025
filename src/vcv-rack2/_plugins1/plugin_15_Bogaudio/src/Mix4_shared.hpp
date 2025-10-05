#pragma once

#include "bogaudio.hpp"
#include "mixer_expander.hpp"
#include "bogaudio_dsp_signal.hpp"

extern Model* modelMix4;
extern Model* modelMix4x;

namespace bogaudio {

typedef MixerExpanderMessage<4> Mix4ExpanderMessage;

} // namespace bogaudio
