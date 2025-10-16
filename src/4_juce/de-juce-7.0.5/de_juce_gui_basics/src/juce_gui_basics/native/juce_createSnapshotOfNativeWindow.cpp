#include "juce_createSnapshotOfNativeWindow.h"
#include "juce_win32_BitmapImage.h"
#include "juce_win32_Desktop.h"

namespace juce
{
   //==============================================================================

   #if JUCE_WINDOWS
   Image createSnapshotOfNativeWindow (void* nativeWindowHandle)
   {
       auto hwnd = (HWND) nativeWindowHandle;

       auto r = convertPhysicalScreenRectangleToLogical (rectangleFromRECT (getWindowScreenRect (hwnd)), hwnd);
       const auto w = r.getWidth();
       const auto h = r.getHeight();

       auto nativeBitmap = new WindowsBitmapImage (Image::RGB, w, h, true);
       Image bitmap (nativeBitmap);

       ScopedDeviceContext deviceContext { hwnd };

       if (isPerMonitorDPIAwareProcess())
       {
           auto scale = getScaleFactorForWindow (hwnd);
           auto prevStretchMode = SetStretchBltMode (nativeBitmap->hdc, HALFTONE);
           SetBrushOrgEx (nativeBitmap->hdc, 0, 0, nullptr);

           StretchBlt (nativeBitmap->hdc, 0, 0, w, h,
                       deviceContext.dc, 0, 0, roundToInt (w * scale), roundToInt (h * scale),
                       SRCCOPY);

           SetStretchBltMode (nativeBitmap->hdc, prevStretchMode);
       }
       else
       {
           BitBlt (nativeBitmap->hdc, 0, 0, w, h, deviceContext.dc, 0, 0, SRCCOPY);
       }

       return SoftwareImageType().convert (bitmap);
   }

   #endif

} // namespace juce
