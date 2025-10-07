#pragma once
#include <juce_gui_basics/juce_gui_basics_config.h>
#include <juce_gui_basics/native/juce_win32_ComInterfaces.h>
//#include <juce_gui_basics/native/juce_win32_Desktop.h>
//#include <juce_gui_basics/native/juce_win32_Touch.h>
//#include <juce_gui_basics/native/juce_win32_MouseCursor.h>
// #include <juce_gui_basics/mouse/juce_MouseCursor.h>
// #include <juce_gui_basics/mouse/juce_CustomMouseCursorInfo.h>
// #include <juce_gui_basics/native/juce_win32_ScopedThreadDPIAwarenessSetter.h>
// #include <juce_gui_basics/desktop/juce_win32_Desktop.h>
// #include <juce_gui_basics/components/juce_Component.h>
// #include <juce_gui_basics/windows/juce_ComponentPeer.h>

namespace juce
{

#if JUCE_WINDOWS

//==============================================================================
class WindowsBitmapImage  : public ImagePixelData
{
public:
    WindowsBitmapImage (const Image::PixelFormat format,
                        const int w, const int h, const bool clearImage);

    ~WindowsBitmapImage() override;

    std::unique_ptr<ImageType> createType() const override
    {
      return std::make_unique<NativeImageType>();
    }

    std::unique_ptr<LowLevelGraphicsContext> createLowLevelContext() override
    {
        sendDataChangeMessage();
        return std::make_unique<LowLevelGraphicsSoftwareRenderer> (Image (this));
    }

    void initialiseBitmapData (Image::BitmapData& bitmap, int x, int y, Image::BitmapData::ReadWriteMode mode) override
    {
        const auto offset = (size_t) (x * pixelStride + y * lineStride);
        bitmap.data = imageData + offset;
        bitmap.size = (size_t) (lineStride * height) - offset;
        bitmap.pixelFormat = pixelFormat;
        bitmap.lineStride = lineStride;
        bitmap.pixelStride = pixelStride;

        if (mode != Image::BitmapData::readOnly)
            sendDataChangeMessage();
    }

    ImagePixelData::Ptr clone() override
    {
        auto im = new WindowsBitmapImage (pixelFormat, width, height, false);

        for (int i = 0; i < height; ++i)
            memcpy (im->imageData + i * lineStride, imageData + i * lineStride, (size_t) lineStride);

        return im;
    }

    void blitToWindow (HWND hwnd, HDC dc, bool transparent, int x, int y, uint8 updateLayeredWindowAlpha) noexcept;

    HBITMAP hBitmap;
    HGDIOBJ previousBitmap;
    BITMAPV4HEADER bitmapInfo;
    HDC hdc;
    uint8* bitmapData;
    int pixelStride, lineStride;
    uint8* imageData;

private:
    static bool isGraphicsCard32Bit()
    {
        ScopedDeviceContext deviceContext { nullptr };
        return GetDeviceCaps (deviceContext.dc, BITSPIXEL) > 24;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WindowsBitmapImage)
};

#endif

} // namespace juce
