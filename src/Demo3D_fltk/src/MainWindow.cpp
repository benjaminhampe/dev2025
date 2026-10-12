#include "MainWindow.h"

MainWindow::MainWindow(int w, int h)
    : Fl_Window(w, h)
{
    begin();

    auto& ui = App::getInstance()->ui;
    ui.canvas = new Canvas(0, 0, 300, 300);
    ui.logbox = new LogBox(300,0,500, 280);

    end();
}

void MainWindow::resize(int X, int Y, int W, int H)
{
    Fl_Window::resize(X, Y, W, H);

    const float zoom = Fl::screen_scale(0);

    const int hL = zoom * 200; // hLogbox
    const int hC = H - hL; // hCanvas

    auto& ui = App::getInstance()->ui;

    ui.canvas->resize(0,0,W,hC);
    ui.logbox->resize(0,hC,W,hL);
}
