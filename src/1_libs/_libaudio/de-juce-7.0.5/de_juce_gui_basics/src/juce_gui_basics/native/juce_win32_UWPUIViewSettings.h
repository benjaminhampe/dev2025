#pragma once
#include <juce_gui_basics/juce_gui_basics_config.h>

#if JUCE_WINDOWS
#include <juce_gui_basics/native/juce_win32_ComInterfaces.h>


namespace juce
{

JUCE_IUNKNOWNCLASS (ITipInvocation, "37c994e7-432b-4834-a2f7-dce1f13b834b")
{
    static CLSID getCLSID() noexcept   { return { 0x4ce576fa, 0x83dc, 0x4f88, { 0x95, 0x1c, 0x9d, 0x07, 0x82, 0xb4, 0xe3, 0x76 } }; }

    JUCE_COMCALL Toggle (HWND) = 0;
};

struct HSTRING_PRIVATE;
typedef HSTRING_PRIVATE* HSTRING;

struct IInspectable : public IUnknown
{
    JUCE_COMCALL GetIids (ULONG* ,IID**) = 0;
    JUCE_COMCALL GetRuntimeClassName (HSTRING*) = 0;
    JUCE_COMCALL GetTrustLevel (void*) = 0;
};

JUCE_COMCLASS (IUIViewSettingsInterop, "3694dbf9-8f68-44be-8ff5-195c98ede8a6")  : public IInspectable
{
    JUCE_COMCALL GetForWindow (HWND, REFIID, void**) = 0;
};

JUCE_COMCLASS (IUIViewSettings, "c63657f6-8850-470d-88f8-455e16ea2c26")  : public IInspectable
{
    enum UserInteractionMode
    {
        Mouse = 0,
        Touch = 1
    };

    JUCE_COMCALL GetUserInteractionMode (UserInteractionMode*) = 0;
};

} // namespace juce

#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL (juce::ITipInvocation, 0x37c994e7, 0x432b, 0x4834, 0xa2, 0xf7, 0xdc, 0xe1, 0xf1, 0x3b, 0x83, 0x4b)
#endif

#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL (juce::IUIViewSettingsInterop, 0x3694dbf9, 0x8f68, 0x44be, 0x8f, 0xf5, 0x19, 0x5c, 0x98, 0xed, 0xe8, 0xa6)
__CRT_UUID_DECL (juce::IUIViewSettings,        0xc63657f6, 0x8850, 0x470d, 0x88, 0xf8, 0x45, 0x5e, 0x16, 0xea, 0x2c, 0x26)
#endif

namespace juce
{

struct UWPUIViewSettings
{
    UWPUIViewSettings();

private:
    //==============================================================================
    struct ComBaseModule
    {
        ComBaseModule() = default;
        ComBaseModule (LPCWSTR libraryName);
        ComBaseModule (ComBaseModule&& o);
        ~ComBaseModule();

        void release();
        ComBaseModule& operator= (ComBaseModule&& o);

        HMODULE h = {};
    };

    using RoInitializeFuncPtr           = HRESULT (WINAPI*) (int);
    using RoGetActivationFactoryFuncPtr = HRESULT (WINAPI*) (HSTRING, REFIID, void**);
    using WindowsCreateStringFuncPtr    = HRESULT (WINAPI*) (LPCWSTR,UINT32, HSTRING*);
    using WindowsDeleteStringFuncPtr    = HRESULT (WINAPI*) (HSTRING);

    ComBaseModule comBaseDLL;
    ComSmartPtr<IUIViewSettingsInterop> viewSettingsInterop;

    RoInitializeFuncPtr roInitialize;
    RoGetActivationFactoryFuncPtr roGetActivationFactory;
    WindowsCreateStringFuncPtr createHString;
    WindowsDeleteStringFuncPtr deleteHString;
};

} // namespace juce

#endif // JUCE_WINDOWS
