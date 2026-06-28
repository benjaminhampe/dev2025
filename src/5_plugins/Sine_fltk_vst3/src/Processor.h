#pragma once
#include "public.sdk/source/vst/vstaudioeffect.h"

namespace SineFLTK {

class Processor : public Steinberg::Vst::AudioEffect
{
public:
    Processor();
    static Steinberg::FUnknown*
    createInstance(void*)
    {
        return (Steinberg::Vst::IAudioProcessor*)new Processor();
    }

    Steinberg::tresult PLUGIN_API
    initialize(FUnknown* context) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API
    process(Steinberg::Vst::ProcessData& data) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API
    setState(Steinberg::IBStream* state) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API
    getState(Steinberg::IBStream* state) SMTG_OVERRIDE;

private:
    float amplitude = 0.5f;
    float frequency = 440.0f;
    float phase = 0.0f;
    double sampleRate = 44100.0;
};

} // namespace SineFLTK
