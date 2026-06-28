#include "Editor.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Slider.H>
#include <cstring>

namespace SineFLTK {

Editor::Editor(Steinberg::Vst::EditController* controller)
    : m_controller(controller)
{
}

Editor::~Editor()
{
    if (m_window)
        delete m_window;
}

//======================== FUnknown ========================

Steinberg::tresult PLUGIN_API Editor::queryInterface(const Steinberg::TUID iid, void** obj)
{
    if (Steinberg::FUnknownPrivate::iidEqual(iid, Steinberg::IPlugView::iid))
    {
        *obj = static_cast<Steinberg::IPlugView*>(this);
        addRef();
        return Steinberg::kResultOk;
    }

    *obj = nullptr;
    return Steinberg::kNoInterface;
}

Steinberg::uint32 PLUGIN_API Editor::addRef()
{
    return ++m_refCount;
}

Steinberg::uint32 PLUGIN_API Editor::release()
{
    Steinberg::uint32 r = --m_refCount;
    if (r == 0)
        delete this;
    return r;
}

//======================== IPlugView ========================

Steinberg::tresult PLUGIN_API Editor::isPlatformTypeSupported(const char* type)
{
    if (std::strcmp(type, Steinberg::kPlatformTypeHWND) == 0)
        return Steinberg::kResultOk;

    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API Editor::attached(void* parent, Steinberg::FIDString type)
{
    m_window = new Fl_Window(300, 200, "Sine Synth");

    m_ampSlider = new Fl_Slider(20, 20, 260, 30);
    m_ampSlider->type(FL_HORIZONTAL);
    m_ampSlider->bounds(0.0, 1.0);
    m_ampSlider->value(0.5);
    m_ampSlider->callback(cb_amp, this);

    m_freqSlider = new Fl_Slider(20, 70, 260, 30);
    m_freqSlider->type(FL_HORIZONTAL);
    m_freqSlider->bounds(20.0, 20000.0);
    m_freqSlider->value(440.0);
    m_freqSlider->callback(cb_freq, this);

    m_phaseSlider = new Fl_Slider(20, 120, 260, 30);
    m_phaseSlider->type(FL_HORIZONTAL);
    m_phaseSlider->bounds(0.0, 6.2831853);
    m_phaseSlider->value(0.0);
    m_phaseSlider->callback(cb_phase, this);

    m_window->end();
    m_window->show();

    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::removed()
{
    if (m_window)
        m_window->hide();
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::onWheel(float)
{
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::onKeyDown(Steinberg::char16, Steinberg::int16, Steinberg::int16)
{
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::onKeyUp(Steinberg::char16, Steinberg::int16, Steinberg::int16)
{
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::getSize(Steinberg::ViewRect* size)
{
    size->left = 0;
    size->top = 0;
    size->right = 300;
    size->bottom = 200;
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::onSize(Steinberg::ViewRect*)
{
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::onFocus(Steinberg::TBool)
{
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::setFrame(Steinberg::IPlugFrame*)
{
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::canResize()
{
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API Editor::checkSizeConstraint(Steinberg::ViewRect*)
{
    return Steinberg::kResultOk;
}

//======================== Callbacks ========================

void Editor::cb_amp(Fl_Widget* w, void* data)
{
    Editor* self = static_cast<Editor*>(data);
    float v = static_cast<Fl_Slider*>(w)->value();
    self->m_controller->setParamNormalized(0, v);
}

void Editor::cb_freq(Fl_Widget* w, void* data)
{
    Editor* self = static_cast<Editor*>(data);
    float v = static_cast<Fl_Slider*>(w)->value();
    self->m_controller->setParamNormalized(1, v);
}

void Editor::cb_phase(Fl_Widget* w, void* data)
{
    Editor* self = static_cast<Editor*>(data);
    float v = static_cast<Fl_Slider*>(w)->value();
    self->m_controller->setParamNormalized(2, v);
}

} // namespace SineFLTK
