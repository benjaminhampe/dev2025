#include "public.sdk/source/vst/vsteditcontroller.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Slider.H>

namespace SineFLTK {

class Editor : public Steinberg::IPlugView
{
public:
    explicit Editor(Steinberg::Vst::EditController* controller);
    ~Editor();

    // FUnknown
    Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID iid, void** obj) SMTG_OVERRIDE;
    Steinberg::uint32 PLUGIN_API addRef() SMTG_OVERRIDE;
    Steinberg::uint32 PLUGIN_API release() SMTG_OVERRIDE;

    // IPlugView
    Steinberg::tresult PLUGIN_API isPlatformTypeSupported(const char* type) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API attached(void* parent, Steinberg::FIDString type) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API removed() SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API onWheel(float distance) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API onKeyDown(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API onKeyUp(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API getSize(Steinberg::ViewRect* size) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API onSize(Steinberg::ViewRect* newSize) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API onFocus(Steinberg::TBool state) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API setFrame(Steinberg::IPlugFrame* frame) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API canResize() SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API checkSizeConstraint(Steinberg::ViewRect* rect) SMTG_OVERRIDE;

private:
    Steinberg::Vst::EditController* m_controller;
    Steinberg::uint32 m_refCount = 1;

    Fl_Window* m_window = nullptr;
    Fl_Slider* m_ampSlider = nullptr;
    Fl_Slider* m_freqSlider = nullptr;
    Fl_Slider* m_phaseSlider = nullptr;

    static void cb_amp(Fl_Widget*, void*);
    static void cb_freq(Fl_Widget*, void*);
    static void cb_phase(Fl_Widget*, void*);
};

} // namespace SineFLTK
