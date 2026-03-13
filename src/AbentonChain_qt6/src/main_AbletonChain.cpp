#include "gui/Track.h"
#include <QApplication>
#include <QMainWindow>

// ------------------------------------------------------------
// main
// ------------------------------------------------------------
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/winico"));

    auto track = new de::gui::track::Track;

    QMainWindow win;
    win.setCentralWidget(track);
    win.resize(1000, 300);
    win.show();
    // win.setWindowIcon(QIcon(":/winico"));
    // win.setWindowIcon(app.windowIcon());
    win.setWindowIcon(QApplication::windowIcon());

    return app.exec();
}

// #include "main.moc"
