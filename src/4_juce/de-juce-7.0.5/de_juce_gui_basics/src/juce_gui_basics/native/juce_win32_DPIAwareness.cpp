#include <juce_gui_basics/native/juce_win32_DPIAwareness.h>
#include <juce_gui_basics/application/juce_Application.h>

namespace juce
{
   
#if JUCE_DEBUG
int numActiveScopedDpiAwarenessDisablers = 0;
bool isInScopedDPIAwarenessDisabler() { return numActiveScopedDpiAwarenessDisablers > 0; }
#endif

void loadDPIAwarenessFunctions()
{
    setProcessDPIAware = (SetProcessDPIAwareFunc) getUser32Function ("SetProcessDPIAware");

    constexpr auto shcore = "SHCore.dll";
    LoadLibraryA (shcore);
    const auto shcoreModule = GetModuleHandleA (shcore);

    if (shcoreModule == nullptr)
        return;

    getDPIForMonitor                    = (GetDPIForMonitorFunc) GetProcAddress (shcoreModule, "GetDpiForMonitor");
    setProcessDPIAwareness              = (SetProcessDPIAwarenessFunc) GetProcAddress (shcoreModule, "SetProcessDpiAwareness");

   #if JUCE_WIN_PER_MONITOR_DPI_AWARE
    getDPIForWindow                     = (GetDPIForWindowFunc) getUser32Function ("GetDpiForWindow");
    getProcessDPIAwareness              = (GetProcessDPIAwarenessFunc) GetProcAddress (shcoreModule, "GetProcessDpiAwareness");
    getWindowDPIAwarenessContext        = (GetWindowDPIAwarenessContextFunc) getUser32Function ("GetWindowDpiAwarenessContext");
    setThreadDPIAwarenessContext        = (SetThreadDPIAwarenessContextFunc) getUser32Function ("SetThreadDpiAwarenessContext");
    getThreadDPIAwarenessContext        = (GetThreadDPIAwarenessContextFunc) getUser32Function ("GetThreadDpiAwarenessContext");
    getAwarenessFromDPIAwarenessContext = (GetAwarenessFromDpiAwarenessContextFunc) getUser32Function ("GetAwarenessFromDpiAwarenessContext");
    setProcessDPIAwarenessContext       = (SetProcessDPIAwarenessContextFunc) getUser32Function ("SetProcessDpiAwarenessContext");
    enableNonClientDPIScaling           = (EnableNonClientDPIScalingFunc) getUser32Function ("EnableNonClientDpiScaling");
   #endif
}

void setDPIAwareness()
{
    if (hasCheckedForDPIAwareness)
        return;

    hasCheckedForDPIAwareness = true;

    if (! JUCEApplicationBase::isStandaloneApp())
        return;

    loadDPIAwarenessFunctions();

    if (setProcessDPIAwarenessContext != nullptr
        && setProcessDPIAwarenessContext (DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
        return;

    if (setProcessDPIAwareness != nullptr && enableNonClientDPIScaling != nullptr
        && SUCCEEDED (setProcessDPIAwareness (DPI_Awareness::DPI_Awareness_Per_Monitor_Aware)))
        return;

    if (setProcessDPIAwareness != nullptr && getDPIForMonitor != nullptr
        && SUCCEEDED (setProcessDPIAwareness (DPI_Awareness::DPI_Awareness_System_Aware)))
        return;

    if (setProcessDPIAware != nullptr)
        setProcessDPIAware();
}

double getGlobalDPI()
{
    setDPIAwareness();

    ScopedDeviceContext deviceContext { nullptr };
    return (GetDeviceCaps (deviceContext.dc, LOGPIXELSX) + GetDeviceCaps (deviceContext.dc, LOGPIXELSY)) / 2.0;
}

double getScaleFactorForWindow (HWND h)
{
    // NB. Using a local function here because we need to call this method from the plug-in wrappers
    // which don't load the DPI-awareness functions on startup
    static auto localGetDPIForWindow = (GetDPIForWindowFunc) getUser32Function ("GetDpiForWindow");

    if (localGetDPIForWindow != nullptr)
        return (double) localGetDPIForWindow (h) / USER_DEFAULT_SCREEN_DPI;

    return 1.0;
}

bool isPerMonitorDPIAwareProcess()
{
   #if ! JUCE_WIN_PER_MONITOR_DPI_AWARE
    return false;
   #else
    static bool dpiAware = []() -> bool
    {
        setDPIAwareness();

        if (! JUCEApplication::isStandaloneApp())
            return false;

        if (getProcessDPIAwareness == nullptr)
            return false;

        DPI_Awareness context;
        getProcessDPIAwareness (nullptr, &context);

        return context == DPI_Awareness::DPI_Awareness_Per_Monitor_Aware;
    }();

    return dpiAware;
   #endif
}

bool isPerMonitorDPIAwareWindow (HWND nativeWindow)
{
   #if ! JUCE_WIN_PER_MONITOR_DPI_AWARE
    return false;
   #else
    setDPIAwareness();

    if (getWindowDPIAwarenessContext != nullptr
        && getAwarenessFromDPIAwarenessContext != nullptr)
    {
        return (getAwarenessFromDPIAwarenessContext (getWindowDPIAwarenessContext (nativeWindow))
                  == DPI_Awareness::DPI_Awareness_Per_Monitor_Aware);
    }

    return isPerMonitorDPIAwareProcess();
   #endif
}

bool isPerMonitorDPIAwareThread (GetThreadDPIAwarenessContextFunc getThreadDPIAwarenessContextIn,
                                 GetAwarenessFromDpiAwarenessContextFunc getAwarenessFromDPIAwarenessContextIn)
{
   #if ! JUCE_WIN_PER_MONITOR_DPI_AWARE
    return false;
   #else
    setDPIAwareness();

    if (getThreadDPIAwarenessContextIn != nullptr
        && getAwarenessFromDPIAwarenessContextIn != nullptr)
    {
        return (getAwarenessFromDPIAwarenessContextIn (getThreadDPIAwarenessContextIn())
                  == DPI_Awareness::DPI_Awareness_Per_Monitor_Aware);
    }

    return isPerMonitorDPIAwareProcess();
   #endif
}


//==============================================================================
class ScopedThreadDPIAwarenessSetter::NativeImpl
{
public:
    static auto& getFunctions()
    {
        struct Functions
        {
            SetThreadDPIAwarenessContextFunc setThreadAwareness             = (SetThreadDPIAwarenessContextFunc) getUser32Function ("SetThreadDpiAwarenessContext");
            GetWindowDPIAwarenessContextFunc getWindowAwareness             = (GetWindowDPIAwarenessContextFunc) getUser32Function ("GetWindowDpiAwarenessContext");
            GetThreadDPIAwarenessContextFunc getThreadAwareness             = (GetThreadDPIAwarenessContextFunc) getUser32Function ("GetThreadDpiAwarenessContext");
            GetAwarenessFromDpiAwarenessContextFunc getAwarenessFromContext = (GetAwarenessFromDpiAwarenessContextFunc) getUser32Function ("GetAwarenessFromDpiAwarenessContext");

            bool isLoaded() const noexcept
            {
                return setThreadAwareness != nullptr
                    && getWindowAwareness != nullptr
                    && getThreadAwareness != nullptr
                    && getAwarenessFromContext != nullptr;
            }

            Functions() = default;
            JUCE_DECLARE_NON_COPYABLE (Functions)
            JUCE_DECLARE_NON_MOVEABLE (Functions)
        };

        static const Functions functions;
        return functions;
    }

    explicit NativeImpl (HWND nativeWindow [[maybe_unused]])
    {
       #if JUCE_WIN_PER_MONITOR_DPI_AWARE
        if (const auto& functions = getFunctions(); functions.isLoaded())
        {
            auto dpiAwareWindow = (functions.getAwarenessFromContext (functions.getWindowAwareness (nativeWindow))
                                   == DPI_Awareness::DPI_Awareness_Per_Monitor_Aware);

            auto dpiAwareThread = (functions.getAwarenessFromContext (functions.getThreadAwareness())
                                   == DPI_Awareness::DPI_Awareness_Per_Monitor_Aware);

            if (dpiAwareWindow && ! dpiAwareThread)
                oldContext = functions.setThreadAwareness (DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
            else if (! dpiAwareWindow && dpiAwareThread)
                oldContext = functions.setThreadAwareness (DPI_AWARENESS_CONTEXT_UNAWARE);
        }
       #endif
    }

    ~NativeImpl()
    {
        if (oldContext != nullptr)
            getFunctions().setThreadAwareness (oldContext);
    }

private:
    DPI_AWARENESS_CONTEXT oldContext = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NativeImpl)
    JUCE_DECLARE_NON_MOVEABLE (NativeImpl)
};

ScopedThreadDPIAwarenessSetter::ScopedThreadDPIAwarenessSetter (void* nativeWindow)
{
    pimpl = std::make_unique<NativeImpl> ((HWND) nativeWindow);
}

ScopedThreadDPIAwarenessSetter::~ScopedThreadDPIAwarenessSetter() = default;

static auto& getScopedDPIAwarenessDisablerFunctions()
{
    struct Functions
    {
        GetThreadDPIAwarenessContextFunc localGetThreadDpiAwarenessContext = (GetThreadDPIAwarenessContextFunc) getUser32Function ("GetThreadDpiAwarenessContext");
        GetAwarenessFromDpiAwarenessContextFunc localGetAwarenessFromDpiAwarenessContextFunc = (GetAwarenessFromDpiAwarenessContextFunc) getUser32Function ("GetAwarenessFromDpiAwarenessContext");
        SetThreadDPIAwarenessContextFunc localSetThreadDPIAwarenessContext = (SetThreadDPIAwarenessContextFunc) getUser32Function ("SetThreadDpiAwarenessContext");

        Functions() = default;
        JUCE_DECLARE_NON_COPYABLE (Functions)
        JUCE_DECLARE_NON_MOVEABLE (Functions)
    };

    static const Functions functions;
    return functions;
}

ScopedDPIAwarenessDisabler::ScopedDPIAwarenessDisabler()
{
    const auto& functions = getScopedDPIAwarenessDisablerFunctions();

    if (! isPerMonitorDPIAwareThread (functions.localGetThreadDpiAwarenessContext, functions.localGetAwarenessFromDpiAwarenessContextFunc))
        return;

    if (auto* localSetThreadDPIAwarenessContext = functions.localSetThreadDPIAwarenessContext)
    {
        previousContext = localSetThreadDPIAwarenessContext (DPI_AWARENESS_CONTEXT_UNAWARE);

       #if JUCE_DEBUG
        ++numActiveScopedDpiAwarenessDisablers;
       #endif
    }
}

ScopedDPIAwarenessDisabler::~ScopedDPIAwarenessDisabler()
{
    if (previousContext != nullptr)
    {
        if (auto* localSetThreadDPIAwarenessContext = getScopedDPIAwarenessDisablerFunctions().localSetThreadDPIAwarenessContext)
            localSetThreadDPIAwarenessContext ((DPI_AWARENESS_CONTEXT) previousContext);

       #if JUCE_DEBUG
        --numActiveScopedDpiAwarenessDisablers;
       #endif
    }
}



} // end namespace juce
