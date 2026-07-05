#include "public.sdk/source/main/pluginfactory.h"
#include "Processor.h"
#include "Controller.h"
#include "pluginterfaces/base/funknown.h"

BEGIN_FACTORY_DEF("Benjamin",
                  "https://example.com",
                  "mailto:dev@example.com")

DEF_CLASS2(
    INLINE_UID_FROM_FUID(FUID(0xAB, 0xE7, 0x10, 0x7C)),
    Steinberg::PClassInfo::kManyInstances,
    "Audio Module Class",
    "SineWGL",
    0,
    "Instrument|Synth",
    "1.0.0",
    "VST 3.8.0",
    Processor::createInstance
)

DEF_CLASS2(
    INLINE_UID_FROM_FUID(FUID(0xAB, 0xE7, 0x10, 0x7D)),
    Steinberg::PClassInfo::kManyInstances,
    "Component Controller Class",
    "SineWGLController",
    0,
    "",
    "1.0.0",
    "VST 3.8.0",
    Controller::createInstance
)

END_FACTORY
