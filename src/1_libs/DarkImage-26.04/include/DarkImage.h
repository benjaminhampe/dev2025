#pragma once
#include <de/Core.h>
#include <de/Math.h>
#include <de/Color.h>
#include <de/ColorGradient.h>
#include <de/Font5x8.h>
#include <de/MathMusic.h>
#include <de/Math3D.h>
#include <de/Math3D_Intersection.h>
#include <de/Math3D_Quaternion.h>

#include <de/image/Image.h>
#include <de/image/ImagePainter.h>
#include <de/image/Bresenham.h>
//#include <de/os/VideoModes.h>
#include <de/image/font/Font.h>

#include <de/os/CommonDialogs.h>

#ifdef _WIN32
    #include <de/os/win32/ComInit.h>
#endif

de::Image
dbImageFromLinearColorGradient(
    const de::LinearColorGradient & cg,
    int w,
    int h,
    bool bVertical);

bool
dbImageToLinearColorGradient(
    de::LinearColorGradient & cg,
    const de::Image &img,
    bool bVertical);

de::Font5x8::TextSize
dbTextSize(
    const std::string& text,
    const de::Font5x8& font );

de::TextSize
dbTextSize(
    const std::wstring& text,
    const de::Font& font );

void
dbAddFontFamily(
    const std::string& familyName,
    const uint8_t* pData,
    const uint64_t nBytes );

void
dbAddFontFamily(
    const std::string& familyName,
    const std::string& uri );

void
dbPrepareFont(
    const de::Font& font,
    const std::string& uri );

void
dbPrepareFont(
    const de::Font& font,
    const uint8_t* dataPtr,
    const uint64_t dataSize );
