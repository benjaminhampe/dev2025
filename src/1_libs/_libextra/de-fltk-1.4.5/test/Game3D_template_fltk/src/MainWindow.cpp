#include "MainWindow.h"


MainWindow::MainWindow(int w, int h)
    : Fl_Window(w, h)
{
    auto& ui = App::getInstance()->ui;

    begin();

    ui.canvas = new Canvas(0, 0, 300, 300);
    ui.logbox = new LogBox(300,0,500, 280);

    // ui.btnLogbox1 = new Fl_Light_Button(300, 280, 500, 20, "Double-Buffered");
    // ui.btnLogbox1->callback(toggle_double);
    // ui.btnLogbox1->user_data(ui.canvas);
    // ui.btnLogbox1->value(1);
    // ui.logbox->buffer(new Fl_Text_Buffer());

    // topwin->end();
    // topwin->resizable(win);
    // topwin->label("Click GL panel to reshape");
    // topwin->show(argc, argv);

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
