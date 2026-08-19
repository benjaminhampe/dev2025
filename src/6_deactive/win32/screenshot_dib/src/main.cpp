#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <stdint.h>

static bool SaveBMP(const wchar_t* path,
                    const void* pixels,
                    int width,
                    int height)
{
    const uint32_t imageSize = width * height * 4;

    BITMAPFILEHEADER bf{};
    bf.bfType = 0x4D42; // 'BM'
    bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bf.bfSize = bf.bfOffBits + imageSize;

    BITMAPINFOHEADER bi{};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    HANDLE hFile = CreateFileW(path, GENERIC_WRITE, 0, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    DWORD written = 0;
    WriteFile(hFile, &bf, sizeof(bf), &written, NULL);
    WriteFile(hFile, &bi, sizeof(bi), &written, NULL);
    WriteFile(hFile, pixels, imageSize, &written, NULL);

    CloseHandle(hFile);
    return true;
}

//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
int main(int argc, char* argv[])
{
    auto hInstance = GetModuleHandle(0);

    // Desktop DC
    HDC hScreenDC = GetDC(NULL);

    const int width  = GetSystemMetrics(SM_CXSCREEN);
    const int height = GetSystemMetrics(SM_CYSCREEN);

    // Prepare DIB section description
    BITMAPINFO bi{};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = width;
    bi.bmiHeader.biHeight      = height;   // bottom-up
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* pixelPtr = nullptr;

    // Create DIB section
    HBITMAP hDIB = CreateDIBSection(hScreenDC,
                                    &bi,
                                    DIB_RGB_COLORS,
                                    &pixelPtr,
                                    NULL,
                                    0);

    if (!hDIB || !pixelPtr)
        return 1;

    // Memory DC for BitBlt
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    SelectObject(hMemDC, hDIB);

    // Copy screen → DIB section
    BitBlt(hMemDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

    // Save BMP
    SaveBMP(L"screenshot_dib.bmp", pixelPtr, width, height);

    // Cleanup
    DeleteDC(hMemDC);
    DeleteObject(hDIB);
    ReleaseDC(NULL, hScreenDC);

    return 0;
}
