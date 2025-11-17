#pragma once
#include <windows.h>

class Editor {
public:
    bool open(void* parent);
    void close();
    void toggleFullscreen();

private:
    HWND hwnd = nullptr;
    HDC hdc = nullptr;
    HGLRC glrc = nullptr;
    bool fullscreen = false;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void initGL();
    void render();
};
