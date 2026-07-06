#pragma once

#include "pluginterfaces/gui/iplugview.h"
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"

#include <windows.h>
#include <gl/GL.h>

class Editor : public Steinberg::IPlugView
{
public:
    Editor();
    Editor(Steinberg::Vst::IEditController* controller);
    virtual ~Editor();

    // IPlugView
    Steinberg::tresult PLUGIN_API isPlatformTypeSupported(Steinberg::FIDString type) override;
    Steinberg::tresult PLUGIN_API attached(void* parentWindow,
                                           Steinberg::FIDString type) override;
    Steinberg::tresult PLUGIN_API removed() override;
    Steinberg::tresult PLUGIN_API onSize(Steinberg::ViewRect* newSize) override;
    Steinberg::tresult PLUGIN_API getSize(Steinberg::ViewRect* size) override;
    Steinberg::tresult PLUGIN_API onFocus(Steinberg::TBool state) override;
    Steinberg::tresult PLUGIN_API setFrame(Steinberg::IPlugFrame* frame) override;

    // Required pure virtuals in your SDK
    Steinberg::tresult PLUGIN_API onWheel(float distance) override;
    Steinberg::tresult PLUGIN_API onKeyDown(Steinberg::char16 key,
                                            Steinberg::int16 keyCode,
                                            Steinberg::int16 modifiers) override;
    Steinberg::tresult PLUGIN_API onKeyUp(Steinberg::char16 key,
                                          Steinberg::int16 keyCode,
                                          Steinberg::int16 modifiers) override;
    Steinberg::tresult PLUGIN_API canResize() override;
    Steinberg::tresult PLUGIN_API checkSizeConstraint(Steinberg::ViewRect* rect) override;

    // FUnknown
    Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID iid,
                                                 void** obj) override;
    Steinberg::uint32 PLUGIN_API addRef() override;
    Steinberg::uint32 PLUGIN_API release() override;

private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void registerWindowClass();
    bool setupPixelFormat(HDC hdc);

    Steinberg::uint32              m_refCount;
    HWND                           m_parentHWND;
    HWND                           m_childHWND;
    HDC                            m_hdc;
    HGLRC                          m_hGL;
    Steinberg::IPlugFrame*         m_plugFrame;
    Steinberg::Vst::IEditController* m_controller;
};
