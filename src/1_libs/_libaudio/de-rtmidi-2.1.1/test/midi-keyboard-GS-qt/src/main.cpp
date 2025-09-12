/*
 *  main.cpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source software. No warranty or guarantee.
 */

// #include ".hpp"

#include "MainWindow.hpp"
#include <QApplication>

int main( int argc, char *argv[] )
{
   srand((unsigned) time(nullptr));

   RtMidiKeyboard amsynth;

   QApplication app( argc, argv );

   auto mainWindow = new MainWindow( &amsynth );
   mainWindow->show();

   app.exec();

   //amsynth.Stop();

   return 0;
}
