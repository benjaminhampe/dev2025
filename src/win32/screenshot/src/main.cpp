#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <stdint.h>

static bool SaveBMP(const wchar_t* path, HBITMAP hBitmap)
{
    BITMAP bmp;
    if (!GetObject(hBitmap, sizeof(bmp), &bmp))
        return false;

    BITMAPINFOHEADER bi{};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    const DWORD imageSize = bmp.bmWidth * bmp.bmHeight * 4;

    // Allocate buffer for pixel data
    uint8_t* pixels = (uint8_t*)malloc(imageSize);
    if (!pixels)
        return false;

    HDC hdc = GetDC(NULL);
    bool ok = false;

    if (GetDIBits(hdc, hBitmap, 0, bmp.bmHeight, pixels,
                  (BITMAPINFO*)&bi, DIB_RGB_COLORS))
    {
        HANDLE hFile = CreateFileW(path, GENERIC_WRITE, 0, NULL,
                                   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            BITMAPFILEHEADER bf{};
            bf.bfType = 0x4D42; // 'BM'
            bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
            bf.bfSize = bf.bfOffBits + imageSize;

            DWORD written = 0;
            WriteFile(hFile, &bf, sizeof(bf), &written, NULL);
            WriteFile(hFile, &bi, sizeof(bi), &written, NULL);
            WriteFile(hFile, pixels, imageSize, &written, NULL);

            CloseHandle(hFile);
            ok = true;
        }
    }

    ReleaseDC(NULL, hdc);
    free(pixels);
    return ok;
}

//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
int main(int argc, char* argv[])
{
    auto hInstance = GetModuleHandle(0);

    // Get desktop DC
    HDC hScreenDC = GetDC(NULL);

    // Determine desktop size
    int width  = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    // Create compatible DC + bitmap
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    SelectObject(hMemDC, hBitmap);

    // Copy screen → bitmap
    BitBlt(hMemDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

    // Save to BMP
    SaveBMP(L"screenshot.bmp", hBitmap);

    // Cleanup
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);

    return 0;
}
