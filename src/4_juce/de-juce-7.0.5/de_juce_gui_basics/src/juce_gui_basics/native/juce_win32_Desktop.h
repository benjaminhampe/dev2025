#pragma once
#include <juce_gui_basics/desktop/juce_Desktop.h>
#include <juce_gui_basics/application/juce_Application.h>
#include <juce_gui_basics/windows/juce_ComponentPeer.h>
#include <juce_gui_basics/components/juce_Component.h>

#include <juce_gui_basics/native/juce_win32_ComInterfaces.h>
#include <juce_gui_basics/native/juce_win32_DPIAwareness.h>
#include <juce_gui_basics/native/juce_win32_Desktop.h>
namespace juce
{

HMONITOR getMonitorFromOutput (ComSmartPtr<IDXGIOutput> output);

void setWindowPos (HWND hwnd, Rectangle<int> bounds, UINT flags, bool adjustTopLeft = false);

RECT getWindowScreenRect (HWND hwnd);

RECT getWindowClientRect (HWND hwnd);

void setWindowZOrder (HWND hwnd, HWND insertAfter);

Rectangle<int> rectangleFromRECT (RECT r) noexcept;

RECT RECTFromRectangle (Rectangle<int> r) noexcept;

Point<int> pointFromPOINT (POINT p) noexcept;

POINT POINTFromPoint (Point<int> p) noexcept;

Point<int> convertPhysicalScreenPointToLogical (Point<int> p, HWND h) noexcept;

Point<int> convertLogicalScreenPointToPhysical (Point<int> p, HWND h) noexcept;

Displays::Display const*
getCurrentDisplayFromScaleFactor (HWND hwnd);

template <typename ValueType>
Rectangle<ValueType>
convertPhysicalScreenRectangleToLogical (Rectangle<ValueType> r, HWND h) noexcept
{
    if (isPerMonitorDPIAwareWindow (h))
        return Desktop::getInstance().getDisplays().physicalToLogical (r,
            getCurrentDisplayFromScaleFactor (h));

    return r;
}

template <typename ValueType>
static Rectangle<ValueType> convertLogicalScreenRectangleToPhysical (Rectangle<ValueType> r, HWND h) noexcept
{
    if (isPerMonitorDPIAwareWindow (h))
        return Desktop::getInstance().getDisplays().logicalToPhysical (r, getCurrentDisplayFromScaleFactor (h));

    return r;
}



//==============================================================================
using SettingChangeCallbackFunc = void (*)(void);
extern SettingChangeCallbackFunc settingChangeCallback;

//==============================================================================

//using VBlankListener = ComponentPeer::VBlankListener;

//==============================================================================
class VSyncThread : private Thread,
                    private AsyncUpdater
{
public:
    VSyncThread (ComSmartPtr<IDXGIOutput> out,
                 HMONITOR mon,
                 ComponentPeer::VBlankListener& listener)
        : Thread ("VSyncThread"),
          output (out),
          monitor (mon)
    {
        listeners.push_back (listener);
        startThread (Priority::highest);
    }

    ~VSyncThread() override
    {
        stopThread (-1);
        cancelPendingUpdate();
    }

    void updateMonitor()
    {
        monitor = getMonitorFromOutput (output);
    }

    HMONITOR getMonitor() const noexcept { return monitor; }

    void addListener (ComponentPeer::VBlankListener& listener)
    {
        listeners.push_back (listener);
    }

    bool removeListener (const ComponentPeer::VBlankListener& listener)
    {
        auto it = std::find_if (listeners.cbegin(),
                                listeners.cend(),
                                [&listener] (const auto& l) { return &(l.get()) == &listener; });

        if (it != listeners.cend())
        {
            listeners.erase (it);
            return true;
        }

        return false;
    }

    bool hasNoListeners() const noexcept
    {
        return listeners.empty();
    }

    bool hasListener (const ComponentPeer::VBlankListener& listener) const noexcept
    {
        return std::any_of (listeners.cbegin(),
                            listeners.cend(),
                            [&listener] (const auto& l) { return &(l.get()) == &listener; });
    }

private:
    //==============================================================================
    void run() override
    {
        while (! threadShouldExit())
        {
            if (output->WaitForVBlank() == S_OK)
                triggerAsyncUpdate();
            else
                Thread::sleep (1);
        }
    }

    void handleAsyncUpdate() override
    {
        for (auto& listener : listeners)
            listener.get().onVBlank();
    }

    //==============================================================================
    ComSmartPtr<IDXGIOutput> output;
    HMONITOR monitor = nullptr;
    std::vector<std::reference_wrapper<ComponentPeer::VBlankListener>> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VSyncThread)
    JUCE_DECLARE_NON_MOVEABLE (VSyncThread)
};

//==============================================================================
class VBlankDispatcher : public DeletedAtShutdown
{
public:
    void updateDisplay (ComponentPeer::VBlankListener& listener, HMONITOR monitor)
    {
        if (monitor == nullptr)
        {
            removeListener (listener);
            return;
        }

        auto threadWithListener = threads.end();
        auto threadWithMonitor  = threads.end();

        for (auto it = threads.begin(); it != threads.end(); ++it)
        {
            if ((*it)->hasListener (listener))
                threadWithListener = it;

            if ((*it)->getMonitor() == monitor)
                threadWithMonitor = it;

            if (threadWithListener != threads.end()
                && threadWithMonitor != threads.end())
            {
                if (threadWithListener == threadWithMonitor)
                    return;

                (*threadWithMonitor)->addListener (listener);

                // This may invalidate iterators, so be careful!
                removeListener (threadWithListener, listener);
                return;
            }
        }

        if (threadWithMonitor != threads.end())
        {
            (*threadWithMonitor)->addListener (listener);
            return;
        }

        if (threadWithListener != threads.end())
            removeListener (threadWithListener, listener);

        for (auto adapter : adapters)
        {
            UINT i = 0;
            ComSmartPtr<IDXGIOutput> output;

            while (adapter->EnumOutputs (i, output.resetAndGetPointerAddress()) != DXGI_ERROR_NOT_FOUND)
            {
                if (getMonitorFromOutput (output) == monitor)
                {
                    threads.emplace_back (std::make_unique<VSyncThread> (output, monitor, listener));
                    return;
                }

                ++i;
            }
        }
    }

    void removeListener (const ComponentPeer::VBlankListener& listener)
    {
        for (auto it = threads.begin(); it != threads.end(); ++it)
            if (removeListener (it, listener))
                return;
    }

    void reconfigureDisplays()
    {
        adapters.clear();

        ComSmartPtr<IDXGIFactory> factory;
        JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wlanguage-extension-token")
        CreateDXGIFactory (__uuidof (IDXGIFactory), (void**)factory.resetAndGetPointerAddress());
        JUCE_END_IGNORE_WARNINGS_GCC_LIKE

        UINT i = 0;
        ComSmartPtr<IDXGIAdapter> adapter;

        while (factory->EnumAdapters (i, adapter.resetAndGetPointerAddress()) != DXGI_ERROR_NOT_FOUND)
        {
            adapters.push_back (adapter);
            ++i;
        }

        for (auto& thread : threads)
            thread->updateMonitor();

        threads.erase (std::remove_if (threads.begin(),
                                       threads.end(),
                                       [] (const auto& thread) { return thread->getMonitor() == nullptr; }),
                       threads.end());
    }

    JUCE_DECLARE_SINGLETON_SINGLETHREADED (VBlankDispatcher, false)

private:
    //==============================================================================
    using Threads = std::vector<std::unique_ptr<VSyncThread>>;

    VBlankDispatcher()
    {
        reconfigureDisplays();
    }

    ~VBlankDispatcher() override
    {
        threads.clear();
        clearSingletonInstance();
    }

    // This may delete the corresponding thread and invalidate iterators,
    // so be careful!
    bool removeListener (Threads::iterator it, const ComponentPeer::VBlankListener& listener)
    {
        if ((*it)->removeListener (listener))
        {
            if ((*it)->hasNoListeners())
                threads.erase (it);

            return true;
        }

        return false;
    }

    //==============================================================================
    std::vector<ComSmartPtr<IDXGIAdapter>> adapters;
    Threads threads;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VBlankDispatcher)
    JUCE_DECLARE_NON_MOVEABLE (VBlankDispatcher)
};

class Desktop::NativeDarkModeChangeDetectorImpl
{
public:
    NativeDarkModeChangeDetectorImpl()
    {
       #if ! JUCE_MINGW
        const auto winVer = getWindowsVersionInfo();

        if (winVer.dwMajorVersion >= 10 && winVer.dwBuildNumber >= 17763)
        {
            const auto uxtheme = "uxtheme.dll";
            LoadLibraryA (uxtheme);
            const auto uxthemeModule = GetModuleHandleA (uxtheme);

            if (uxthemeModule != nullptr)
            {
                shouldAppsUseDarkMode = (ShouldAppsUseDarkModeFunc) GetProcAddress (uxthemeModule, MAKEINTRESOURCEA (132));

                if (shouldAppsUseDarkMode != nullptr)
                    darkModeEnabled = shouldAppsUseDarkMode() && ! isHighContrast();
            }
        }
       #endif
    }

    ~NativeDarkModeChangeDetectorImpl()
    {
        UnhookWindowsHookEx (hook);
    }

    bool isDarkModeEnabled() const noexcept  { return darkModeEnabled; }

private:
    static bool isHighContrast()
    {
        HIGHCONTRASTW highContrast {};

        if (SystemParametersInfoW (SPI_GETHIGHCONTRAST, sizeof (highContrast), &highContrast, false))
            return highContrast.dwFlags & HCF_HIGHCONTRASTON;

        return false;
    }

    static LRESULT CALLBACK callWndProc (int nCode, WPARAM wParam, LPARAM lParam)
    {
        auto* params = reinterpret_cast<CWPSTRUCT*> (lParam);

        if (nCode >= 0
            && params != nullptr
            && params->message == WM_SETTINGCHANGE
            && params->lParam != 0
            && CompareStringOrdinal(
                  reinterpret_cast<LPWCH> (params->lParam), -1, L"ImmersiveColorSet", -1, true) == CSTR_EQUAL)
        {
            Desktop::getInstance().nativeDarkModeChangeDetectorImpl->colourSetChanged();
        }

        return CallNextHookEx ({}, nCode, wParam, lParam);
    }

    void colourSetChanged()
    {
        if (shouldAppsUseDarkMode != nullptr)
        {
            const auto wasDarkModeEnabled = std::exchange (darkModeEnabled, shouldAppsUseDarkMode() && ! isHighContrast());

            if (darkModeEnabled != wasDarkModeEnabled)
                Desktop::getInstance().darkModeChanged();
        }
    }

    using ShouldAppsUseDarkModeFunc = bool (WINAPI*)();
    ShouldAppsUseDarkModeFunc shouldAppsUseDarkMode = nullptr;

    bool darkModeEnabled = false;
    HHOOK hook { SetWindowsHookEx (WH_CALLWNDPROC,
                                   callWndProc,
                                   (HINSTANCE) juce::Process::getCurrentModuleInstanceHandle(),
                                   GetCurrentThreadId()) };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NativeDarkModeChangeDetectorImpl)
};





} // end namespace juce
