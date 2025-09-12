#pragma once
#include <juce_gui_basics/mouse/juce_MouseCursor.h>
#include <juce_gui_basics/mouse/juce_CustomMouseCursorInfo.h>
#include <juce_gui_basics/native/juce_win32_Desktop.h>

namespace juce
{

//==============================================================================
namespace IconConverters
//==============================================================================
{
   HICON createHICONFromImage (const Image& image, const BOOL isIcon, int hotspotX, int hotspotY);

   Image createImageFromHICON (HICON icon);

   struct IconDestructor
   {
      void operator() (HICON ptr) const { if (ptr != nullptr) DestroyIcon (ptr); }
   };

   // used by win32_ComponentPeer.h
   // used by win32_Windowing.cpp
   using IconPtr = std::unique_ptr<std::remove_pointer_t<HICON>, IconDestructor>;

} // namespace IconConverters

//==============================================================================
class MouseCursor::PlatformSpecificHandle
//==============================================================================
{
public:
    static void showInWindow (PlatformSpecificHandle* handle, ComponentPeer* peer);

    explicit PlatformSpecificHandle (const MouseCursor::StandardCursorType type)
        : impl (makeHandle (type)) {}

    explicit PlatformSpecificHandle (const CustomMouseCursorInfo& info)
        : impl (makeHandle (info)) {}

private:
    struct Impl
    {
        virtual ~Impl() = default;
        virtual HCURSOR getCursor (ComponentPeer&) = 0;
    };

    class BuiltinImpl : public Impl
    {
    public:
        explicit BuiltinImpl (HCURSOR cursorIn)
            : cursor (cursorIn) {}

        HCURSOR getCursor (ComponentPeer&) override { return cursor; }

    private:
        HCURSOR cursor;
    };

    class ImageImpl : public Impl
    {
    public:
        explicit ImageImpl (const CustomMouseCursorInfo& infoIn) : info (infoIn) {}

        HCURSOR getCursor (ComponentPeer& peer) override
        {
            JUCE_ASSERT_MESSAGE_THREAD;

            static auto getCursorSize = getCursorSizeForPeerFunction();

            const auto size = getCursorSize (peer);
            const auto iter = cursorsBySize.find (size);

            if (iter != cursorsBySize.end())
                return iter->second.get();

            const auto logicalSize = info.image.getScaledBounds();
            const auto scale = (float) size / (float) unityCursorSize;
            const auto physicalSize = logicalSize * scale;

            const auto& image = info.image.getImage();
            const auto rescaled = image.rescaled (roundToInt ((float) physicalSize.getWidth()),
                                                  roundToInt ((float) physicalSize.getHeight()));

            const auto effectiveScale = rescaled.getWidth() / logicalSize.getWidth();

            const auto hx = jlimit (0, rescaled.getWidth(),  roundToInt ((float) info.hotspot.x * effectiveScale));
            const auto hy = jlimit (0, rescaled.getHeight(), roundToInt ((float) info.hotspot.y * effectiveScale));

            return cursorsBySize.emplace (size, CursorPtr { IconConverters::createHICONFromImage (rescaled, false, hx, hy) }).first->second.get();
        }

    private:
        struct CursorDestructor
        {
            void operator() (HCURSOR ptr) const { if (ptr != nullptr) DestroyCursor (ptr); }
        };

        using CursorPtr = std::unique_ptr<std::remove_pointer_t<HCURSOR>, CursorDestructor>;

        const CustomMouseCursorInfo info;
        std::map<int, CursorPtr> cursorsBySize;
    };

    static auto getCursorSizeForPeerFunction() -> int (*) (ComponentPeer&)
    {
        static const auto getDpiForMonitor = []() -> GetDPIForMonitorFunc
        {
            constexpr auto library = "SHCore.dll";
            LoadLibraryA (library);

            if (auto* handle = GetModuleHandleA (library))
                return (GetDPIForMonitorFunc) GetProcAddress (handle, "GetDpiForMonitor");

            return nullptr;
        }();

        static const auto getSystemMetricsForDpi = []() -> GetSystemMetricsForDpiFunc
        {
            constexpr auto library = "User32.dll";
            LoadLibraryA (library);

            if (auto* handle = GetModuleHandleA (library))
                return (GetSystemMetricsForDpiFunc) GetProcAddress (handle, "GetSystemMetricsForDpi");

            return nullptr;
        }();

        if (getDpiForMonitor == nullptr || getSystemMetricsForDpi == nullptr)
            return [] (ComponentPeer&) { return unityCursorSize; };

        return [] (ComponentPeer& p)
        {
            const ScopedThreadDPIAwarenessSetter threadDpiAwarenessSetter { p.getNativeHandle() };

            UINT dpiX = 0, dpiY = 0;

            if (auto* monitor = MonitorFromWindow ((HWND) p.getNativeHandle(), MONITOR_DEFAULTTONULL))
                if (SUCCEEDED (getDpiForMonitor (monitor, MDT_Default, &dpiX, &dpiY)))
                    return getSystemMetricsForDpi (SM_CXCURSOR, dpiX);

            return unityCursorSize;
        };
    }

    static constexpr auto unityCursorSize = 32;

    static std::unique_ptr<Impl> makeHandle (const CustomMouseCursorInfo& info)
    {
        return std::make_unique<ImageImpl> (info);
    }

    static std::unique_ptr<Impl> makeHandle (const MouseCursor::StandardCursorType type)
    {
        LPCTSTR cursorName = IDC_ARROW;

        switch (type)
        {
            case NormalCursor:
            case ParentCursor:                  break;
            case NoCursor:                      return std::make_unique<BuiltinImpl> (nullptr);
            case WaitCursor:                    cursorName = IDC_WAIT; break;
            case IBeamCursor:                   cursorName = IDC_IBEAM; break;
            case PointingHandCursor:            cursorName = MAKEINTRESOURCE(32649); break;
            case CrosshairCursor:               cursorName = IDC_CROSS; break;

            case LeftRightResizeCursor:
            case LeftEdgeResizeCursor:
            case RightEdgeResizeCursor:         cursorName = IDC_SIZEWE; break;

            case UpDownResizeCursor:
            case TopEdgeResizeCursor:
            case BottomEdgeResizeCursor:        cursorName = IDC_SIZENS; break;

            case TopLeftCornerResizeCursor:
            case BottomRightCornerResizeCursor: cursorName = IDC_SIZENWSE; break;

            case TopRightCornerResizeCursor:
            case BottomLeftCornerResizeCursor:  cursorName = IDC_SIZENESW; break;

            case UpDownLeftRightResizeCursor:   cursorName = IDC_SIZEALL; break;

            case DraggingHandCursor:
            {
                static const unsigned char dragHandData[]
                    { 71,73,70,56,57,97,16,0,16,0,145,2,0,0,0,0,255,255,255,0,0,0,0,0,0,33,249,4,1,0,0,2,0,44,0,0,0,0,16,0,
                      16,0,0,2,52,148,47,0,200,185,16,130,90,12,74,139,107,84,123,39,132,117,151,116,132,146,248,60,209,138,
                      98,22,203,114,34,236,37,52,77,217,247,154,191,119,110,240,193,128,193,95,163,56,60,234,98,135,2,0,59 };

                return makeHandle ({ ScaledImage (ImageFileFormat::loadFrom (dragHandData, sizeof (dragHandData))), { 8, 7 } });
            }

            case CopyingCursor:
            {
                static const unsigned char copyCursorData[]
                    { 71,73,70,56,57,97,21,0,21,0,145,0,0,0,0,0,255,255,255,0,128,128,255,255,255,33,249,4,1,0,0,3,0,44,0,0,0,0,21,0,
                      21,0,0,2,72,4,134,169,171,16,199,98,11,79,90,71,161,93,56,111,78,133,218,215,137,31,82,154,100,200,86,91,202,142,
                      12,108,212,87,235,174, 15,54,214,126,237,226,37,96,59,141,16,37,18,201,142,157,230,204,51,112,252,114,147,74,83,
                      5,50,68,147,208,217,16,71,149,252,124,5,0,59,0,0 };

                return makeHandle ({ ScaledImage (ImageFileFormat::loadFrom (copyCursorData, sizeof (copyCursorData))), { 1, 3 } });
            }

            case NumStandardCursorTypes: JUCE_FALLTHROUGH
            default:
                jassertfalse; break;
        }

        return std::make_unique<BuiltinImpl> ([&]
        {
            if (auto* c = LoadCursor (nullptr, cursorName))
                return c;

            return LoadCursor (nullptr, IDC_ARROW);
        }());
    }

    std::unique_ptr<Impl> impl;
};


} // namespace juce
