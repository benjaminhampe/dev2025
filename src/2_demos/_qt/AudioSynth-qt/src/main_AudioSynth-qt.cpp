//////////////////////////////////////////////////////////////////////////////
/// @file main_AudioSynth-qt.cpp
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////
#include "MainWindow.hpp"
#include <QApplication>
#include <QGuiApplication>

#include <GVstPlugin.hpp>

int main( int argc, char** argv )
{
   volatile ComInit ole;

   QString appName = "Benni AudioSynth using libQt, libDarkGPU and libRtAudio";
   QApplication::setStyle("windows");
   QApplication app( argc, argv );
   app.setApplicationName( appName );
   //   QString const appPath = qApp->applicationDirPath() + "/";
   //   QString const stylePath( appPath + "themes/darcula.css" );
   //   app.setStyleSheet( "file:///" + stylePath );
   //   dbAddFontFamily( "fontawesome.ttf" );
   //   dbAddFontFamily( "la-regular-400.ttf" );
   //   dbAddFontFamily( "la-brands-400.ttf" );
   //   dbAddFontFamily( "la-solid-900.ttf" );
   //   QString const appPath = qApp->applicationDirPath() + "/";
   //   QString const cssPath( appPath + "themes/darcula.css" );
   //   app.setStyleSheet( "file:///" + cssPath );
   MainWindow mainWindow;
   mainWindow.setWindowTitle( appName );
   mainWindow.setWindowIcon( QIcon(":/winico") ); // Compile in res/resource.qrc, see CMakeLists.txt
   mainWindow.show();
   return app.exec();
}
