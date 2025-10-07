#include <juce_gui_basics/native/juce_isWindowOnCurrentVirtualDesktop.h>
#include <juce_core/native/juce_win32_ComSmartPtr.h>

//==============================================================================
#if JUCE_WINDOWS


namespace juce
{

JUCE_COMCLASS (JuceIVirtualDesktopManager, "a5cd92ff-29be-454c-8d04-d82879fb3f1b") : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE IsWindowOnCurrentVirtualDesktop(
         __RPC__in HWND topLevelWindow,
         __RPC__out BOOL * onCurrentDesktop) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetWindowDesktopId(
         __RPC__in HWND topLevelWindow,
         __RPC__out GUID * desktopId) = 0;

    virtual HRESULT STDMETHODCALLTYPE MoveWindowToDesktop(
         __RPC__in HWND topLevelWindow,
         __RPC__in REFGUID desktopId) = 0;
};

JUCE_COMCLASS (JuceVirtualDesktopManager, "aa509086-5ca9-4c25-8f95-589d3c07b48a");

} // end namespace juce

#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL (juce::JuceIVirtualDesktopManager, 0xa5cd92ff, 0x29be, 0x454c, 0x8d, 0x04, 0xd8, 0x28, 0x79, 0xfb, 0x3f, 0x1b)
__CRT_UUID_DECL (juce::JuceVirtualDesktopManager,  0xaa509086, 0x5ca9, 0x4c25, 0x8f, 0x95, 0x58, 0x9d, 0x3c, 0x07, 0xb4, 0x8a)
#endif

bool juce::isWindowOnCurrentVirtualDesktop (void* x)
{
    if (x == nullptr)
        return false;

    static auto* desktopManager = []
    {
        JuceIVirtualDesktopManager* result = nullptr;

        JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wlanguage-extension-token")

        if (SUCCEEDED (CoCreateInstance (__uuidof (JuceVirtualDesktopManager), nullptr, CLSCTX_ALL, IID_PPV_ARGS (&result))))
            return result;

        JUCE_END_IGNORE_WARNINGS_GCC_LIKE

        return static_cast<JuceIVirtualDesktopManager*> (nullptr);
    }();

    BOOL current = false;

    if (auto* dm = desktopManager)
        if (SUCCEEDED (dm->IsWindowOnCurrentVirtualDesktop (static_cast<HWND> (x), &current)))
            return current != false;

    return true;
}

#else
 bool juce::isWindowOnCurrentVirtualDesktop (void*) { return true; }
 juce::ScopedDPIAwarenessDisabler::ScopedDPIAwarenessDisabler()  { ignoreUnused (previousContext); }
 juce::ScopedDPIAwarenessDisabler::~ScopedDPIAwarenessDisabler() {}
#endif
