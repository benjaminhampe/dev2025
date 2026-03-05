/// (c) 2017 - 20180 Benjamin Hampe <benjaminhampe@gmx.de>

#pragma once
/*
#include <iostream>

struct Test1
{
   Test1()
   {
      std::cout << "Test1()" << std::endl;
   }
   virtual ~Test1()
   {
      std::cout << "~Test1()" << std::endl;
   }

};

struct Test2 : public Test1
{
   Test2()
      : Test1()
   {
      std::cout << "Test2()" << std::endl;
   }

   ~Test2() override
   {
      std::cout << "~Test2()" << std::endl;
   }

};
*/
#include <QMainWindow>
#include <LiveCommon.h>

struct App;

class Window : public QMainWindow
{
   Q_OBJECT
   DE_CREATE_LOGGER("Window")
   App & m_app;
   //Test2 m_test2;
public:
   Window( App & app );
   ~Window() override;
   void createMenuBar();
public slots:
   void onMenuFileExit();
   void onMenuFileOpen();
   void onShowAboutDialog();
protected:
   void closeEvent( QCloseEvent* event ) override;

};
