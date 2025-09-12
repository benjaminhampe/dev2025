/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#include <juce_gui_basics/native/juce_win32_Windowing.h>
#include <juce_gui_basics/keyboard/juce_TextInputTarget.h>
#include <juce_gui_basics/misc/juce_DropShadower.h>
#include <juce_gui_basics/desktop/juce_Desktop.h>
#include <juce_gui_basics/windows/juce_AlertWindow.h>
#include <juce_gui_basics/windows/juce_NativeMessageBox.h>
#include <juce_gui_basics/native/juce_win32_ComponentPeer.h>
#include <juce_gui_basics/windows/juce_AlertWindowMappings.h>
#include <juce_gui_basics/mouse/juce_MouseInputSource_SourceList.h>
#include <juce_gui_basics/keyboard/juce_SystemClipboard.h>
#include <commctrl.h>

#if JUCE_MODULE_AVAILABLE_juce_audio_plugin_client
 #include <juce_audio_plugin_client/AAX/juce_AAX_Modifier_Injector.h>
#endif

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wcast-function-type")

#undef GetSystemMetrics // multimon overrides this for some reason and causes a mess..

// these are in the windows SDK, but need to be repeated here for GCC..
#ifndef GET_APPCOMMAND_LPARAM
 #define GET_APPCOMMAND_LPARAM(lParam)     ((short) (HIWORD (lParam) & ~FAPPCOMMAND_MASK))

 #define FAPPCOMMAND_MASK                  0xF000
 #define APPCOMMAND_MEDIA_NEXTTRACK        11
 #define APPCOMMAND_MEDIA_PREVIOUSTRACK    12
 #define APPCOMMAND_MEDIA_STOP             13
 #define APPCOMMAND_MEDIA_PLAY_PAUSE       14
#endif

#ifndef WM_APPCOMMAND
 #define WM_APPCOMMAND                     0x0319
#endif

bool juce_isRunningInWine();


//==============================================================================
#if ! JUCE_MINGW
extern RTL_OSVERSIONINFOW getWindowsVersionInfo();
#endif



namespace juce
{

// (This internal function is used by the plugin client module)
bool offerKeyMessageToJUCEWindow (MSG& m);
bool offerKeyMessageToJUCEWindow (MSG& m)   { return HWNDComponentPeer::offerKeyMessageToJUCEWindow (m); }

//==============================================================================
static DWORD getProcess (HWND hwnd)
{
    DWORD result = 0;
    GetWindowThreadProcessId (hwnd, &result);
    return result;
}

/*  Returns true if the viewComponent is embedded into a window
    owned by the foreground process.
*/
bool isEmbeddedInForegroundProcess (Component* c)
{
    if (c == nullptr)
        return false;

    auto* peer = c->getPeer();
    auto* hwnd = peer != nullptr ? static_cast<HWND> (peer->getNativeHandle()) : nullptr;

    if (hwnd == nullptr)
        return true;

    const auto fgProcess    = getProcess (GetForegroundWindow());
    const auto ownerProcess = getProcess (GetAncestor (hwnd, GA_ROOTOWNER));
    return fgProcess == ownerProcess;
}

bool JUCE_CALLTYPE Process::isForegroundProcess()
{
    if (auto fg = GetForegroundWindow())
        return getProcess (fg) == GetCurrentProcessId();

    return true;
}

// N/A on Windows as far as I know.
void JUCE_CALLTYPE Process::makeForegroundProcess() {}
void JUCE_CALLTYPE Process::hide() {}

//==============================================================================
static BOOL CALLBACK enumAlwaysOnTopWindows (HWND hwnd, LPARAM lParam)
{
    if (IsWindowVisible (hwnd))
    {
        DWORD processID = 0;
        GetWindowThreadProcessId (hwnd, &processID);

        if (processID == GetCurrentProcessId())
        {
            WINDOWINFO info{};

            if (GetWindowInfo (hwnd, &info)
                 && (info.dwExStyle & WS_EX_TOPMOST) != 0)
            {
                *reinterpret_cast<bool*> (lParam) = true;
                return FALSE;
            }
        }
    }

    return TRUE;
}

bool juce_areThereAnyAlwaysOnTopWindows()
{
    bool anyAlwaysOnTopFound = false;
    EnumWindows (&enumAlwaysOnTopWindows, (LPARAM) &anyAlwaysOnTopFound);
    return anyAlwaysOnTopFound;
}

//==============================================================================
#if JUCE_MSVC
 // required to enable the newer dialog box on vista and above
 #pragma comment(linker,                             \
         "\"/MANIFESTDEPENDENCY:type='Win32' "       \
         "name='Microsoft.Windows.Common-Controls' " \
         "version='6.0.0.0' "                        \
         "processorArchitecture='*' "                \
         "publicKeyToken='6595b64144ccf1df' "        \
         "language='*'\""                            \
     )
#endif

class WindowsMessageBoxBase  : private AsyncUpdater
{
public:
    WindowsMessageBoxBase (Component* comp,
                           std::unique_ptr<ModalComponentManager::Callback>&& cb)
        : associatedComponent (comp),
          callback (std::move (cb))
    {
    }

    virtual int getResult() = 0;

    HWND getParentHWND() const
    {
        if (associatedComponent != nullptr)
            return (HWND) associatedComponent->getWindowHandle();

        return nullptr;
    }

    using AsyncUpdater::triggerAsyncUpdate;

private:
    void handleAsyncUpdate() override
    {
        const auto result = getResult();

        if (callback != nullptr)
            callback->modalStateFinished (result);

        delete this;
    }

    Component::SafePointer<Component> associatedComponent;
    std::unique_ptr<ModalComponentManager::Callback> callback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WindowsMessageBoxBase)
};

class PreVistaMessageBox  : public WindowsMessageBoxBase
{
public:
    PreVistaMessageBox (const MessageBoxOptions& opts,
                        UINT extraFlags,
                        std::unique_ptr<ModalComponentManager::Callback>&& cb)
        : WindowsMessageBoxBase (opts.getAssociatedComponent(), std::move (cb)),
          flags (extraFlags | getMessageBoxFlags (opts.getIconType())),
          title (opts.getTitle()), message (opts.getMessage())
    {
    }

    int getResult() override
    {
        const auto result = MessageBoxW(getParentHWND(),
                                       message.toWideCharPointer(),
                                       title.toWideCharPointer(), flags);

        if (result == IDYES || result == IDOK)     return 0;
        if (result == IDNO && ((flags & 1) != 0))  return 1;

        return 2;
    }

private:
    static UINT getMessageBoxFlags (MessageBoxIconType iconType) noexcept
    {
        // this window can get lost behind JUCE windows which are set to be alwaysOnTop
        // so if there are any set it to be topmost
        const auto topmostFlag = juce_areThereAnyAlwaysOnTopWindows() ? MB_TOPMOST : 0;

        const auto iconFlags = [&]() -> decltype (topmostFlag)
        {
            switch (iconType)
            {
                case MessageBoxIconType::QuestionIcon:  return MB_ICONQUESTION;
                case MessageBoxIconType::WarningIcon:   return MB_ICONWARNING;
                case MessageBoxIconType::InfoIcon:      return MB_ICONINFORMATION;
                case MessageBoxIconType::NoIcon:        break;
            }

            return 0;
        }();

        return static_cast<UINT> (MB_TASKMODAL | MB_SETFOREGROUND | topmostFlag | iconFlags);
    }

    const UINT flags;
    const String title, message;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PreVistaMessageBox)
};

using TaskDialogIndirectFunc =
         HRESULT (WINAPI*) (const TASKDIALOGCONFIG*, INT*, INT*, BOOL*);

static TaskDialogIndirectFunc taskDialogIndirect = nullptr;

class WindowsTaskDialog  : public WindowsMessageBoxBase
{
public:
    WindowsTaskDialog (const MessageBoxOptions& opts,
                       std::unique_ptr<ModalComponentManager::Callback>&& cb)
        : WindowsMessageBoxBase (opts.getAssociatedComponent(), std::move (cb)),
          iconType (opts.getIconType()),
          title (opts.getTitle()), message (opts.getMessage()),
          button1 (opts.getButtonText (0)), button2 (opts.getButtonText (1)), button3 (opts.getButtonText (2))
    {
    }

    int getResult() override
    {
        TASKDIALOGCONFIG config{};

        config.cbSize         = sizeof (config);
        config.hwndParent     = getParentHWND();
        config.pszWindowTitle = title.toWideCharPointer();
        config.pszContent     = message.toWideCharPointer();
        config.hInstance      = (HINSTANCE) Process::getCurrentModuleInstanceHandle();

        if (iconType == MessageBoxIconType::QuestionIcon)
        {
            if (auto* questionIcon = LoadIcon (nullptr, IDI_QUESTION))
            {
                config.hMainIcon = questionIcon;
                config.dwFlags |= TDF_USE_HICON_MAIN;
            }
        }
        else
        {
            auto icon = [this]() -> LPWSTR
            {
                switch (iconType)
                {
                    case MessageBoxIconType::WarningIcon:   return TD_WARNING_ICON;
                    case MessageBoxIconType::InfoIcon:      return TD_INFORMATION_ICON;

                    case MessageBoxIconType::QuestionIcon:  JUCE_FALLTHROUGH
                    case MessageBoxIconType::NoIcon:
                        break;
                }

                return nullptr;
            }();

            if (icon != nullptr)
                config.pszMainIcon = icon;
        }

        std::vector<TASKDIALOG_BUTTON> buttons;

        for (const auto* buttonText : { &button1, &button2, &button3 })
            if (buttonText->isNotEmpty())
                buttons.push_back ({ (int) buttons.size(), buttonText->toWideCharPointer() });

        config.pButtons = buttons.data();
        config.cButtons = (UINT) buttons.size();

        int buttonIndex = 0;
        taskDialogIndirect (&config, &buttonIndex, nullptr, nullptr);

        return buttonIndex;
    }

    static bool loadTaskDialog()
    {
        static bool hasChecked = false;

        if (! hasChecked)
        {
            hasChecked = true;

            const auto comctl = "Comctl32.dll";
            LoadLibraryA (comctl);
            const auto comctlModule = GetModuleHandleA (comctl);

            if (comctlModule != nullptr)
                taskDialogIndirect = (TaskDialogIndirectFunc) GetProcAddress (comctlModule, "TaskDialogIndirect");
        }

        return taskDialogIndirect != nullptr;
    }

private:
    MessageBoxIconType iconType;
    String title, message, button1, button2, button3;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WindowsTaskDialog)
};

static std::unique_ptr<WindowsMessageBoxBase> createMessageBox (const MessageBoxOptions& options,
                                                                std::unique_ptr<ModalComponentManager::Callback> callback)
{
    const auto useTaskDialog =
       #if JUCE_MODAL_LOOPS_PERMITTED
        callback != nullptr &&
       #endif
        SystemStats::getOperatingSystemType() >= SystemStats::WinVista
          && WindowsTaskDialog::loadTaskDialog();

    if (useTaskDialog)
        return std::make_unique<WindowsTaskDialog> (options, std::move (callback));

    const auto extraFlags = [&options]
    {
        const auto numButtons = options.getNumButtons();

        if (numButtons == 3)
            return MB_YESNOCANCEL;

        if (numButtons == 2)
            return options.getButtonText (0) == "OK" ? MB_OKCANCEL
                                                     : MB_YESNO;

        return MB_OK;
    }();

    return std::make_unique<PreVistaMessageBox> (options, (UINT) extraFlags, std::move (callback));
}

static int showDialog (const MessageBoxOptions& options,
                       ModalComponentManager::Callback* callbackIn,
                       AlertWindowMappings::MapFn mapFn)
{
   #if JUCE_MODAL_LOOPS_PERMITTED
    if (callbackIn == nullptr)
    {
        jassert (mapFn != nullptr);

        auto messageBox = createMessageBox (options, nullptr);
        return mapFn (messageBox->getResult());
    }
   #endif

    auto messageBox = createMessageBox (options,
                                        AlertWindowMappings::getWrappedCallback (callbackIn, mapFn));

    messageBox->triggerAsyncUpdate();
    messageBox.release();

    return 0;
}

#if JUCE_MODAL_LOOPS_PERMITTED
void JUCE_CALLTYPE NativeMessageBox::showMessageBox (MessageBoxIconType iconType,
                                                     const String& title, const String& message,
                                                     Component* associatedComponent)
{
    showDialog (MessageBoxOptions()
                  .withIconType (iconType)
                  .withTitle (title)
                  .withMessage (message)
                  .withButton (TRANS("OK"))
                  .withAssociatedComponent (associatedComponent),
                nullptr, AlertWindowMappings::messageBox);
}

int JUCE_CALLTYPE NativeMessageBox::show (const MessageBoxOptions& options)
{
    return showDialog (options, nullptr, AlertWindowMappings::noMapping);
}
#endif

void JUCE_CALLTYPE NativeMessageBox::showMessageBoxAsync (MessageBoxIconType iconType,
                                                          const String& title, const String& message,
                                                          Component* associatedComponent,
                                                          ModalComponentManager::Callback* callback)
{
    showDialog (MessageBoxOptions()
                  .withIconType (iconType)
                  .withTitle (title)
                  .withMessage (message)
                  .withButton (TRANS("OK"))
                  .withAssociatedComponent (associatedComponent),
                callback, AlertWindowMappings::messageBox);
}

bool JUCE_CALLTYPE NativeMessageBox::showOkCancelBox (MessageBoxIconType iconType,
                                                      const String& title, const String& message,
                                                      Component* associatedComponent,
                                                      ModalComponentManager::Callback* callback)
{
    return showDialog (MessageBoxOptions()
                         .withIconType (iconType)
                         .withTitle (title)
                         .withMessage (message)
                         .withButton (TRANS("OK"))
                         .withButton (TRANS("Cancel"))
                         .withAssociatedComponent (associatedComponent),
                       callback, AlertWindowMappings::okCancel) != 0;
}

int JUCE_CALLTYPE NativeMessageBox::showYesNoCancelBox (MessageBoxIconType iconType,
                                                        const String& title, const String& message,
                                                        Component* associatedComponent,
                                                        ModalComponentManager::Callback* callback)
{
    return showDialog (MessageBoxOptions()
                         .withIconType (iconType)
                         .withTitle (title)
                         .withMessage (message)
                         .withButton (TRANS("Yes"))
                         .withButton (TRANS("No"))
                         .withButton (TRANS("Cancel"))
                         .withAssociatedComponent (associatedComponent),
                       callback, AlertWindowMappings::yesNoCancel);
}

int JUCE_CALLTYPE NativeMessageBox::showYesNoBox (MessageBoxIconType iconType,
                                                  const String& title, const String& message,
                                                  Component* associatedComponent,
                                                  ModalComponentManager::Callback* callback)
{
    return showDialog (MessageBoxOptions()
                         .withIconType (iconType)
                         .withTitle (title)
                         .withMessage (message)
                         .withButton (TRANS("Yes"))
                         .withButton (TRANS("No"))
                         .withAssociatedComponent (associatedComponent),
                       callback, AlertWindowMappings::okCancel);
}

void JUCE_CALLTYPE NativeMessageBox::showAsync (const MessageBoxOptions& options,
                                                ModalComponentManager::Callback* callback)
{
    showDialog (options, callback, AlertWindowMappings::noMapping);
}

void JUCE_CALLTYPE NativeMessageBox::showAsync (const MessageBoxOptions& options,
                                                std::function<void (int)> callback)
{
    showAsync (options, ModalCallbackFunction::create (callback));
}

//==============================================================================
bool MouseInputSource::SourceList::addSource()
{
    auto numSources = sources.size();

    if (numSources == 0 || canUseMultiTouch())
    {
        addSource (numSources, numSources == 0 ? MouseInputSource::InputSourceType::mouse
                                               : MouseInputSource::InputSourceType::touch);
        return true;
    }

    return false;
}

bool MouseInputSource::SourceList::canUseTouch()
{
    return canUseMultiTouch();
}

Point<float> MouseInputSource::getCurrentRawMousePosition()
{
    POINT mousePos;
    GetCursorPos (&mousePos);

    auto p = pointFromPOINT (mousePos);

    if (isPerMonitorDPIAwareThread())
        p = Desktop::getInstance().getDisplays().physicalToLogical (p);

    return p.toFloat();
}

void MouseInputSource::setRawMousePosition (Point<float> newPosition)
{
    auto newPositionInt = newPosition.roundToInt();

   #if JUCE_WIN_PER_MONITOR_DPI_AWARE
    if (isPerMonitorDPIAwareThread())
        newPositionInt = Desktop::getInstance().getDisplays().logicalToPhysical (newPositionInt);
   #endif

    auto point = POINTFromPoint (newPositionInt);
    SetCursorPos (point.x, point.y);
}

//==============================================================================
class ScreenSaverDefeater   : public Timer
{
public:
    ScreenSaverDefeater()
    {
        startTimer (10000);
        timerCallback();
    }

    void timerCallback() override
    {
        if (Process::isForegroundProcess())
        {
            INPUT input = {};
            input.type = INPUT_MOUSE;
            input.mi.mouseData = MOUSEEVENTF_MOVE;

            SendInput (1, &input, sizeof (INPUT));
        }
    }
};

static std::unique_ptr<ScreenSaverDefeater> screenSaverDefeater;

void Desktop::setScreenSaverEnabled (const bool isEnabled)
{
    if (isEnabled)
        screenSaverDefeater = nullptr;
    else if (screenSaverDefeater == nullptr)
        screenSaverDefeater.reset (new ScreenSaverDefeater());
}

bool Desktop::isScreenSaverEnabled()
{
    return screenSaverDefeater == nullptr;
}

//==============================================================================
void LookAndFeel::playAlertSound()
{
    MessageBeep (MB_OK);
}

//==============================================================================
void SystemClipboard::copyTextToClipboard (const String& text)
{
    if (OpenClipboard (nullptr) != 0)
    {
        if (EmptyClipboard() != 0)
        {
            auto bytesNeeded = CharPointer_UTF16::getBytesRequiredFor (text.getCharPointer()) + 4;

            if (bytesNeeded > 0)
            {
                if (auto bufH = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, bytesNeeded + sizeof (WCHAR)))
                {
                    if (auto* data = static_cast<WCHAR*> (GlobalLock (bufH)))
                    {
                        text.copyToUTF16 (data, bytesNeeded);
                        GlobalUnlock (bufH);

                        SetClipboardData (CF_UNICODETEXT, bufH);
                    }
                }
            }
        }

        CloseClipboard();
    }
}

String SystemClipboard::getTextFromClipboard()
{
    String result;

    if (OpenClipboard (nullptr) != 0)
    {
        if (auto bufH = GetClipboardData (CF_UNICODETEXT))
        {
            if (auto* data = (const WCHAR*) GlobalLock (bufH))
            {
                result = String (data, (size_t) (GlobalSize (bufH) / sizeof (WCHAR)));
                GlobalUnlock (bufH);
            }
        }

        CloseClipboard();
    }

    return result;
}

//==============================================================================
static auto extractFileHICON (const File& file)
{
    WORD iconNum = 0;
    WCHAR name[MAX_PATH * 2];
    file.getFullPathName().copyToUTF16 (name, sizeof (name));

    return IconConverters::IconPtr {
            ExtractAssociatedIconW (
               (HINSTANCE) Process::getCurrentModuleInstanceHandle(),
                                                            name,
                                                            &iconNum) };
}

Image juce_createIconForFile (const File& file)
{
    if (const auto icon = extractFileHICON (file))
        return IconConverters::createImageFromHICON (icon.get());

    return {};
}

//==============================================================================
JUCE_END_IGNORE_WARNINGS_GCC_LIKE

} // namespace juce
