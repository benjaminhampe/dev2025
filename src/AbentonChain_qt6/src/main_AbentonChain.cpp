#include "gui/MainWindow.h"
#include "App.h"
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>

void dbLoadFont(QString uri)
{
    int id = QFontDatabase::addApplicationFont(uri);

    if (id == -1)
    {
        qWarning() << "Failed to load font file " << uri;
    }
    else
    {
        qWarning() << "Loaded font["<<id<<"] file " <<uri;
    }

    QStringList families =
        QFontDatabase::applicationFontFamilies(id);

    qDebug() << "Loaded font families:" << families;
}

// ------------------------------------------------------------
// main
// ------------------------------------------------------------
int main(int argc, char **argv)
{
// Fixes OpenGL issues with Qt6:
// OpenGLWidget returns fully broken widget size with Qt's dpi scaling
// Has (1/x) error or so. Bad math.
#ifdef Q_OS_WINDOWS
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
#endif

    qputenv("QT3D_RENDERER", "opengl");
    qputenv("QSG_RHI_BACKEND", "opengl");

    // Is this really necessary?
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    //QCoreApplication::setAttribute(Qt::AA_NativeWindows);

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/winico"));

    dbLoadFont(":/fonts/NotoSans-Bold.ttf");
    dbLoadFont(":/fonts/NotoSans-BoldItalic.ttf");
    dbLoadFont(":/fonts/NotoSans-Italic.ttf");
    dbLoadFont(":/fonts/NotoSans-Regular.ttf");

    // Example: set default application font
    // QFont font(families.at(0));
    // QApplication::setFont(font);

    // ... create your UI here ...



    MainWindow win;
    // win.setWindowIcon(QIcon(":/winico"));
    // win.setWindowIcon(app.windowIcon());
    win.setWindowIcon(QApplication::windowIcon());

    int retVal = app.exec();

    delete App::instance();

    return retVal;
}

// #include "main.moc"
