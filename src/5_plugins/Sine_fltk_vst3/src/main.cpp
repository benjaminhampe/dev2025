#include "public.sdk/source/main/pluginfactory.h"
#include "Processor.h"
#include "Controller.h"
#include "pluginterfaces/base/funknown.h"

BEGIN_FACTORY_DEF("Benjamin",
                  "https://example.com",
                  "mailto:dev@example.com")

DEF_CLASS2(
    INLINE_UID_FROM_FUID(FUID(0x01, 0x02, 0x03, 0x04)),
    Steinberg::PClassInfo::kManyInstances,
    "Audio Module Class",
    "SineFLTK",
    0,
    "Instrument|Synth",
    "1.0.0",
    "VST 3.8.0",
    SineFLTK::Processor::createInstance
)

DEF_CLASS2(
    INLINE_UID_FROM_FUID(FUID(0x11, 0x12, 0x13, 0x14)),
    Steinberg::PClassInfo::kManyInstances,
    "Component Controller Class",
    "SineFLTKController",
    0,
    "",
    "1.0.0",
    "VST 3.8.0",
    SineFLTK::Controller::createInstance
)

END_FACTORY

/*
namespace SineFLTK {

static const Steinberg::TUID ProcessorCID =
{
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C,
    0x0D, 0x0E, 0x0F, 0x10
};

static const Steinberg::TUID ControllerCID =
{
    0x11, 0x12, 0x13, 0x14,
    0x15, 0x16, 0x17, 0x18,
    0x19, 0x1A, 0x1B, 0x1C,
    0x1D, 0x1E, 0x1F, 0x20
};

} // namespace SineFLTK

BEGIN_FACTORY_DEF("Benjamin",
                  "https://example.com",
                  "mailto:dev@example.com")

DEF_CLASS(
    SineFLTK::ProcessorCID,
    Steinberg::PClassInfo::kManyInstances,
    "Audio Module Class",
    "SineFLTK",
    0,
    "Instrument|Synth"
)

DEF_CLASS(
    SineFLTK::ControllerCID,
    Steinberg::PClassInfo::kManyInstances,
    "Component Controller Class",
    "SineFLTKController",
    0,
    ""
)

END_FACTORY
*/

/*
namespace SineFLTK {

// static const Steinberg::FUID ProcessorUID (0xA1B2C3D4, 0x1111, 0x2222, 0x3333);
// static const Steinberg::FUID ControllerUID(0xA1B2C3D4, 0x4444, 0x5555, 0x6666);
// TUID is typedef'd as char[16] in your SDK.
// Use values in 0x00–0x7F to avoid narrowing warnings/errors.

static const Steinberg::TUID ProcessorCID =
{
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C,
    0x0D, 0x0E, 0x0F, 0x10
};

static const Steinberg::TUID ControllerCID =
{
    0x11, 0x12, 0x13, 0x14,
    0x15, 0x16, 0x17, 0x18,
    0x19, 0x1A, 0x1B, 0x1C,
    0x1D, 0x1E, 0x1F, 0x20
};

} // namespace SineFLTK

#define stringPluginName "SineFLTK"

void dumb()
{
    auto a = Steinberg::Vst::kVstAudioEffectClass;
}

#define FULL_VERSION_STR "1.0.0"


BEGIN_FACTORY_DEF("Benjamin",
                  "https://example.com",
                  "mailto:dev@example.com")

DEF_CLASS2(
    { 0x01, 0x02, 0x03, 0x04,
      0x05, 0x06, 0x07, 0x08,
      0x09, 0x0A, 0x0B, 0x0C,
      0x0D, 0x0E, 0x0F, 0x10 },
    Steinberg::PClassInfo::kManyInstances,
    "Audio Module Class",
    "SineFLTK",
    0,
    "Instrument|Synth",
    "1.0.0",
    "VST 3.8.0",
    SineFLTK::Processor::createInstance
)

DEF_CLASS2(
    SineFLTK::ControllerCID,
    Steinberg::PClassInfo::kManyInstances,
    Steinberg::Vst::kComponentControllerClass,
    "SineFLTKController",
    Steinberg::Vst::kSimpleMode,
    "",
    FULL_VERSION_STR,
    Steinberg::Vst::kVstVersionString,
    SineFLTK::Controller::createInstance
)

END_FACTORY
*/
