#pragma once
#include "App.h"


class MainWindow : public Fl_Window
{
public:
    MainWindow(int w, int h);

    void resize(int X, int Y, int W, int H) override;
};

