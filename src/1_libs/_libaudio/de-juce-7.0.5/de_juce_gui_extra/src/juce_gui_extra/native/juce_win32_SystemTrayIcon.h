#pragma once
#include <juce_gui_extra/misc/juce_SystemTrayIconComponent.h>
#include <juce_core/native/juce_win32_ComSmartPtr.h>

namespace juce
{

//==============================================================================
class SystemTrayIconComponent::Pimpl
//==============================================================================
{
public:
    Pimpl (SystemTrayIconComponent& owner_, HICON hicon, HWND hwnd);
    ~Pimpl();
    void updateIcon (HICON hicon);
    void setToolTip (const String& toolTip);
    void handleTaskBarEvent (const LPARAM lParam);
    static Pimpl* getPimpl (HWND hwnd);
    static LRESULT CALLBACK hookedWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT windowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void showBubble (const String& title, const String& content);
    SystemTrayIconComponent& owner;
    NOTIFYICONDATAW iconData;
private:
    WNDPROC originalWndProc;
    const DWORD taskbarCreatedMessage;
    enum { WM_TRAYNOTIFY = WM_USER + 100 };
    void notify (DWORD message) noexcept;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
};


} // namespace juce
