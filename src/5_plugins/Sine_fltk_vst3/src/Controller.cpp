#include "Controller.h"
#include "Editor.h"

namespace SineFLTK {

Steinberg::tresult PLUGIN_API
Controller::initialize(FUnknown* context)
{
    EditController::initialize(context);

    parameters.addParameter(STR16("Amplitude"), nullptr, 0, 0.5, Steinberg::Vst::ParameterInfo::kCanAutomate, 0);
    parameters.addParameter(STR16("Frequency"), STR16("Hz"), 0, 440.0, Steinberg::Vst::ParameterInfo::kCanAutomate, 1);
    parameters.addParameter(STR16("Phase"), STR16("rad"), 0, 0.0, Steinberg::Vst::ParameterInfo::kCanAutomate, 2);

    return Steinberg::kResultOk;
}

Steinberg::IPlugView* PLUGIN_API
Controller::createView(const char* name)
{
    if (strcmp(name, Steinberg::Vst::ViewType::kEditor) == 0)
        return new Editor(this);

    return nullptr;
}

} // end namespace SineFLTK
