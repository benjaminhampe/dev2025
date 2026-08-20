#include <de/win32/win32_Load_Shell_Icon.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    // #define _WIN32_WINNT  0x0A00   // Windows 8 API freischalten
    // #define WINVER        0x0A00
    // #define _WIN32_WINNT  0x0602   // Windows 8 API freischalten
    // #define WINVER        0x0602
    #include <windows.h>            // only for Window ICOn
    #include <shellapi.h>
    //#include "../res/resource.h"    // only for Window ICOn
    // #include <shellscalingapi.h>    // Für SetProcessDpiAwarenessContext()
    // #include <winuser.h>
    // #include <dwmapi.h>
#endif

std::shared_ptr<de::Image>
load_shell_icon(const std::string& uri, int ico_w, int ico_h, bool bOpenIcon)
{
#ifdef _WIN32
    auto path = de::FileSystem::makeWinPath(uri);
    // HIMAGELIST imgList;
    // SHGetImageList(SHIL_SMALL, IID_IImageList, (void**)&imgList);
    // SHIL_SMALL → 16×16
    // SHIL_LARGE → 32×32
    // SHIL_EXTRALARGE → 48×48
    // SHIL_JUMBO → 256×256

    UINT style = SHGFI_ICON; // | SHGFI_SHELLICONSIZE // SHGFI_SMALLICON
    if (bOpenIcon) style |= SHGFI_OPENICON;

    SHFILEINFOA sfi{};
    SHGetFileInfoA(
        path.c_str(),
        FILE_ATTRIBUTE_NORMAL,
        &sfi,
        sizeof(sfi),
        style
    );

    if (!sfi.hIcon)
    {
        DE_ERROR("No sfi.hIcon. ", path)
        return nullptr;
    }

    // -----------------------------------------
    // 1. Query icon size
    // -----------------------------------------
    int large_ico_w = GetSystemMetrics(SM_CXICON);
    int large_ico_h = GetSystemMetrics(SM_CYICON);
    int small_ico_w = GetSystemMetrics(SM_CXSMICON);
    int small_ico_h = GetSystemMetrics(SM_CYSMICON);

    if (ico_w < 1) ico_w = large_ico_w;
    if (ico_h < 1) ico_h = large_ico_h;

    // -----------------------------------------
    // 3. Create a top-down 32-bit RGBA DIB section YOU control
    // -----------------------------------------
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = ico_w;
    bi.bmiHeader.biHeight = -ico_h;       // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;     // RGBA
    bi.bmiHeader.biCompression = BI_RGB;

    void* dibPixels = nullptr;
    HDC hdc = CreateCompatibleDC(nullptr);
    HBITMAP hbm = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &dibPixels, nullptr, 0);

    SelectObject(hdc, hbm);

    // -----------------------------------------
    // 4. Draw the icon DIRECTLY into your DIB
    // -----------------------------------------
    DrawIconEx(
        hdc,
        0, 0,
        sfi.hIcon,
        ico_w, ico_h,
        0,
        nullptr,
        DI_NORMAL
    );

    // -----------------------------------------
    // 2. Create image FIRST
    // -----------------------------------------

    auto img = std::make_shared<de::Image>(ico_w, ico_h);

    // -----------------------------------------
    // 5. Copy DIB pixels → FLTK pixels (RGBA)
    // -----------------------------------------
    const unsigned char* src = (const unsigned char*)dibPixels;

    for (int y = 0; y < ico_h; ++y)
    {
        for (int x = 0; x < ico_w; ++x)
        {
            uint8_t B = src[(y*ico_w + x)*4 + 0];
            uint8_t G = src[(y*ico_w + x)*4 + 1];
            uint8_t R = src[(y*ico_w + x)*4 + 2];
            uint8_t A = src[(y*ico_w + x)*4 + 3];
            img->setPixel(x,y,dbRGB(R,G,B,A));
        }
    }

    // -----------------------------------------
    // 6. Cleanup
    // -----------------------------------------
    DeleteObject(hbm);
    DeleteDC(hdc);
    DestroyIcon(sfi.hIcon);

    return img;
#else

    // img->fill(dbRGB(255,128,32)); // HalfLife Orange.

    DE_ERROR("Not implemented")

    return nullptr;
#endif
}
