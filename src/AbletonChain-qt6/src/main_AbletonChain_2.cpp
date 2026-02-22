// #include "PluginWidget.h"
#include "PluginManagerWidget.h"
#include <QApplication>
#include <QMainWindow>

// ------------------------------------------------------------
// main
// ------------------------------------------------------------
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    auto manager = new PluginManagerWidget;

    auto scroll = new QScrollArea;
    scroll->setContentsMargins(0,0,0,0);
    scroll->setWidgetResizable(true);
    scroll->setWidget(manager);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QMainWindow win;
    win.setCentralWidget(scroll);
    win.resize(1400, 400);
    win.show();

    return app.exec();
}

// #include "main.moc"
