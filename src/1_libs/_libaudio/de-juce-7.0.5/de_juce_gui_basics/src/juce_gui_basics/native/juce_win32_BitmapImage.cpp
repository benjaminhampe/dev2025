#include <juce_gui_basics/native/juce_win32_BitmapImage.h>
#include <juce_gui_basics/native/juce_win32_Desktop.h>
//#include <juce_gui_basics/native/juce_win32_Touch.h>
//#include <juce_gui_basics/native/juce_win32_MouseCursor.h>
// #include <juce_gui_basics/mouse/juce_MouseCursor.h>
// #include <juce_gui_basics/mouse/juce_CustomMouseCursorInfo.h>
// #include <juce_gui_basics/native/juce_win32_ScopedThreadDPIAwarenessSetter.h>
// #include <juce_gui_basics/desktop/juce_win32_Desktop.h>
// #include <juce_gui_basics/components/juce_Component.h>
// #include <juce_gui_basics/windows/juce_ComponentPeer.h>

#if JUCE_WINDOWS


namespace juce
{

WindowsBitmapImage::WindowsBitmapImage (const Image::PixelFormat format,
                  const int w, const int h, const bool clearImage)
  : ImagePixelData (format, w, h)
{
  jassert (format == Image::RGB || format == Image::ARGB);

  static bool alwaysUse32Bits = isGraphicsCard32Bit(); // NB: for 32-bit cards, it's faster to use a 32-bit image.

  pixelStride = (alwaysUse32Bits || format == Image::ARGB) ? 4 : 3;
  lineStride = -((w * pixelStride + 3) & ~3);

  zerostruct (bitmapInfo);
  bitmapInfo.bV4Size     = sizeof (BITMAPV4HEADER);
  bitmapInfo.bV4Width    = w;
  bitmapInfo.bV4Height   = h;
  bitmapInfo.bV4Planes   = 1;
  bitmapInfo.bV4CSType   = 1;
  bitmapInfo.bV4BitCount = (unsigned short) (pixelStride * 8);

  if (format == Image::ARGB)
  {
      bitmapInfo.bV4AlphaMask      = 0xff000000;
      bitmapInfo.bV4RedMask        = 0xff0000;
      bitmapInfo.bV4GreenMask      = 0xff00;
      bitmapInfo.bV4BlueMask       = 0xff;
      bitmapInfo.bV4V4Compression  = BI_BITFIELDS;
  }
  else
  {
      bitmapInfo.bV4V4Compression  = BI_RGB;
  }

  {
      ScopedDeviceContext deviceContext { nullptr };
      hdc = CreateCompatibleDC (deviceContext.dc);
  }

  SetMapMode (hdc, MM_TEXT);

  hBitmap = CreateDIBSection (hdc, (BITMAPINFO*) &(bitmapInfo), DIB_RGB_COLORS,
                              (void**) &bitmapData, nullptr, 0);

  if (hBitmap != nullptr)
      previousBitmap = SelectObject (hdc, hBitmap);

  if (format == Image::ARGB && clearImage)
      zeromem (bitmapData, (size_t) std::abs (h * lineStride));

  imageData = bitmapData - (lineStride * (h - 1));
}

WindowsBitmapImage::~WindowsBitmapImage()
{
  SelectObject (hdc, previousBitmap); // Selecting the previous bitmap before deleting the DC avoids a warning in BoundsChecker
  DeleteDC (hdc);
  DeleteObject (hBitmap);
}
/*
std::unique_ptr<ImageType> createType() const override    { return std::make_unique<NativeImageType>(); }

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
*/

void WindowsBitmapImage::blitToWindow (HWND hwnd, HDC dc, bool transparent, int x, int y, uint8 updateLayeredWindowAlpha) noexcept
{
  SetMapMode (dc, MM_TEXT);

  if (transparent)
  {
      auto windowBounds = getWindowScreenRect (hwnd);

      POINT p = { -x, -y };
      POINT pos = { windowBounds.left, windowBounds.top };
      SIZE size = { windowBounds.right - windowBounds.left,
                    windowBounds.bottom - windowBounds.top };

      BLENDFUNCTION bf;
      bf.AlphaFormat = 1 /*AC_SRC_ALPHA*/;
      bf.BlendFlags = 0;
      bf.BlendOp = AC_SRC_OVER;
      bf.SourceConstantAlpha = updateLayeredWindowAlpha;

      UpdateLayeredWindow (hwnd, nullptr, &pos, &size, hdc, &p, 0, &bf, 2 /*ULW_ALPHA*/);
  }
  else
  {
      StretchDIBits (dc,
                     x, y, width, height,
                     0, 0, width, height,
                     bitmapData, (const BITMAPINFO*) &bitmapInfo,
                     DIB_RGB_COLORS, SRCCOPY);
  }
}

} // namespace juce

#endif
