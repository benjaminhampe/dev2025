#pragma once
#include "public.sdk/source/vst/vsteditcontroller.h"

class Controller : public Steinberg::Vst::EditController
{
public:
    Controller() = default;

    static Steinberg::FUnknown*
    createInstance(void*)
    {
        return (Steinberg::Vst::IEditController*)new Controller();
    }

    Steinberg::tresult PLUGIN_API
    initialize(FUnknown* context) SMTG_OVERRIDE;

    Steinberg::IPlugView* PLUGIN_API
    createView(const char* name) SMTG_OVERRIDE;
};
