#pragma once
#include <de/Core.h>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tree.H>

struct TarInspector : public Fl_Double_Window
{
    TarInspector(std::string uri, int X, int Y, int W, int H);
    ~TarInspector();

    void resize(int X, int Y, int W, int H);
};
