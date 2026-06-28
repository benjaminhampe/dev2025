#include "Processor.h"
#include <cmath>
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"
// using namespace Steinberg;
// using namespace Steinberg::Vst;
namespace SineFLTK {

Processor::Processor() {}

Steinberg::tresult PLUGIN_API
Processor::initialize(FUnknown* context)
{
    AudioEffect::initialize(context);
    addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API
Processor::process(Steinberg::Vst::ProcessData& data)
{
    if (data.numOutputs == 0)
        return Steinberg::kResultOk;

    if (data.processContext)
        sampleRate = data.processContext->sampleRate;

    // Read parameter changes
    if (data.inputParameterChanges)
    {
        int32_t count = data.inputParameterChanges->getParameterCount();
        for (int32_t i = 0; i < count; i++)
        {
            Steinberg::Vst::IParameterChanges* pc = data.inputParameterChanges;
            Steinberg::Vst::IParamValueQueue* queue = pc->getParameterData(i);
            if (!queue) continue;

            int32_t id = queue->getParameterId();
            int32_t points = queue->getPointCount();
            Steinberg::Vst::ParamValue value;
            int32_t sampleOffset;

            if (queue->getPoint(points - 1, sampleOffset, value) == Steinberg::kResultTrue)
            {
                if (id == 0) amplitude = value;
                if (id == 1) frequency = value;
                if (id == 2) phase = value;
            }
        }
    }

    // Audio output
    Steinberg::Vst::AudioBusBuffers& out = data.outputs[0];
    float* left = out.channelBuffers32[0];
    float* right = out.channelBuffers32[1];

    for (int32_t i = 0; i < data.numSamples; i++)
    {
        float s = amplitude * std::sin(phase);
        phase += 2.0 * M_PI * frequency / sampleRate;
        if (phase > 2.0 * M_PI) phase -= 2.0 * M_PI;

        left[i] = s;
        right[i] = s;
    }

    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API
Processor::setState(Steinberg::IBStream* state)
{
    return Steinberg::kResultOk;
}
Steinberg::tresult PLUGIN_API
Processor::getState(Steinberg::IBStream* state)
{
    return Steinberg::kResultOk;
}

} // end namespace SineFLTK
