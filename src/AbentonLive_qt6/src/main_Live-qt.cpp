//////////////////////////////////////////////////////////////////////////////
/// @file main_AudioSynth-qt.cpp
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////
#include "Window.h"
#include <QApplication>
#include <QGuiApplication>

#include "App.h"

int main( int argc, char** argv )
{
   //volatile ComInit ole;

   QApplication::setStyle("windows");
   QApplication app( argc, argv );

   App live;
   app.setApplicationName( live.appName );

   int retVal = app.exec();

   delete live.m_window;

   return retVal;
}
