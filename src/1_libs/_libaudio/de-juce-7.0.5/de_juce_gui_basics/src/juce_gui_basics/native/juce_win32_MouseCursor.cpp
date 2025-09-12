#include <juce_gui_basics/native/juce_win32_MouseCursor.h>

#include <juce_gui_basics/components/juce_Component.h>
#include <juce_gui_basics/windows/juce_ComponentPeer.h>
#include <juce_gui_basics/native/juce_win32_BitmapImage.h>

namespace juce
{

namespace IconConverters
{
   /*
    struct IconDestructor
    {
        void operator() (HICON ptr) const { if (ptr != nullptr) DestroyIcon (ptr); }
    };

    using IconPtr = std::unique_ptr<std::remove_pointer_t<HICON>, IconDestructor>;
   */

   // ============================================================================================
   Image createImageFromHICON (HICON icon)
   // ============================================================================================
    {
        if (icon == nullptr)
            return {};

        struct ScopedICONINFO   : public ICONINFO
        {
            ScopedICONINFO()
            {
                hbmColor = nullptr;
                hbmMask = nullptr;
            }

            ~ScopedICONINFO()
            {
                if (hbmColor != nullptr)
                    ::DeleteObject (hbmColor);

                if (hbmMask != nullptr)
                    ::DeleteObject (hbmMask);
            }
        };

        ScopedICONINFO info;

        if (! ::GetIconInfo (icon, &info))
            return {};

        BITMAP bm;

        if (! (::GetObject (info.hbmColor, sizeof (BITMAP), &bm)
               && bm.bmWidth > 0 && bm.bmHeight > 0))
            return {};

        ScopedDeviceContext deviceContext { nullptr };

        if (auto* dc = ::CreateCompatibleDC (deviceContext.dc))
        {
            BITMAPV5HEADER header = {};
            header.bV5Size = sizeof (BITMAPV5HEADER);
            header.bV5Width = bm.bmWidth;
            header.bV5Height = -bm.bmHeight;
            header.bV5Planes = 1;
            header.bV5Compression = BI_RGB;
            header.bV5BitCount = 32;
            header.bV5RedMask = 0x00FF0000;
            header.bV5GreenMask = 0x0000FF00;
            header.bV5BlueMask = 0x000000FF;
            header.bV5AlphaMask = 0xFF000000;
            header.bV5CSType = 0x57696E20; // 'Win '
            header.bV5Intent = LCS_GM_IMAGES;

            uint32* bitmapImageData = nullptr;

            if (auto* dib = ::CreateDIBSection (deviceContext.dc, (BITMAPINFO*) &header, DIB_RGB_COLORS,
                                                (void**) &bitmapImageData, nullptr, 0))
            {
                auto oldObject = ::SelectObject (dc, dib);

                auto numPixels = bm.bmWidth * bm.bmHeight;
                auto numColourComponents = (size_t) numPixels * 4;

                // Windows icon data comes as two layers, an XOR mask which contains the bulk
                // of the image data and an AND mask which provides the transparency. Annoyingly
                // the XOR mask can also contain an alpha channel, in which case the transparency
                // mask should not be applied, but there's no way to find out a priori if the XOR
                // mask contains an alpha channel.

                HeapBlock<bool> opacityMask (numPixels);
                memset (bitmapImageData, 0, numColourComponents);
                ::DrawIconEx (dc, 0, 0, icon, bm.bmWidth, bm.bmHeight, 0, nullptr, DI_MASK);

                for (int i = 0; i < numPixels; ++i)
                    opacityMask[i] = (bitmapImageData[i] == 0);

                Image result = Image (Image::ARGB, bm.bmWidth, bm.bmHeight, true);
                Image::BitmapData imageData (result, Image::BitmapData::readWrite);

                memset (bitmapImageData, 0, numColourComponents);
                ::DrawIconEx (dc, 0, 0, icon, bm.bmWidth, bm.bmHeight, 0, nullptr, DI_NORMAL);
                memcpy (imageData.data, bitmapImageData, numColourComponents);

                auto imageHasAlphaChannel = [&imageData, numPixels]()
                {
                    for (int i = 0; i < numPixels; ++i)
                        if (imageData.data[i * 4] != 0)
                            return true;

                    return false;
                };

                if (! imageHasAlphaChannel())
                    for (int i = 0; i < numPixels; ++i)
                        imageData.data[i * 4] = opacityMask[i] ? 0xff : 0x00;

                ::SelectObject (dc, oldObject);
                ::DeleteObject (dib);
                ::DeleteDC (dc);

                return result;
            }

            ::DeleteDC (dc);
        }

        return {};
    }


   // ============================================================================================
   HICON createHICONFromImage (const Image& image, const BOOL isIcon, int hotspotX, int hotspotY)
   // ============================================================================================
    {
        auto nativeBitmap = new WindowsBitmapImage(
            Image::ARGB, image.getWidth(), image.getHeight(), true);
        Image bitmap (nativeBitmap);

        {
            Graphics g (bitmap);
            g.drawImageAt (image, 0, 0);
        }

        auto mask = CreateBitmap (image.getWidth(), image.getHeight(), 1, 1, nullptr);

        ICONINFO info;
        info.fIcon = isIcon;
        info.xHotspot = (DWORD) hotspotX;
        info.yHotspot = (DWORD) hotspotY;
        info.hbmMask = mask;
        info.hbmColor = nativeBitmap->hBitmap;

        auto hi = CreateIconIndirect (&info);
        DeleteObject (mask);
        return hi;
    }
} // namespace IconConverters


//==============================================================================

//static
void
MouseCursor::PlatformSpecificHandle::showInWindow (PlatformSpecificHandle* handle, ComponentPeer* peer)
{
   SetCursor ([&]
   {
      if (handle != nullptr && handle->impl != nullptr && peer != nullptr)
          return handle->impl->getCursor (*peer);

      return LoadCursor (nullptr, IDC_ARROW);
   }());
}

/*
//==============================================================================
class MouseCursor::PlatformSpecificHandle
{
public:
    explicit PlatformSpecificHandle (const MouseCursor::StandardCursorType type)
        : impl (makeHandle (type)) {}

    explicit PlatformSpecificHandle (const CustomMouseCursorInfo& info)
        : impl (makeHandle (info)) {}

    static void showInWindow (PlatformSpecificHandle* handle, ComponentPeer* peer)
    {
        SetCursor ([&]
        {
            if (handle != nullptr && handle->impl != nullptr && peer != nullptr)
                return handle->impl->getCursor (*peer);

            return LoadCursor (nullptr, IDC_ARROW);
        }());
    }

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
*/

} // namespace juce
