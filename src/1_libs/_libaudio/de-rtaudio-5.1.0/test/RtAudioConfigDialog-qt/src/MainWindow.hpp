#pragma once
#include <sstream>
#include <chrono>
#include <cmath>
#include <QMainWindow>
#include <QScreen>
#include <QWidget>
#include <QDesktopWidget>
#include <QWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QDial>
#include <QDebug>
#include <QThread>
#include <QWidget>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QScrollArea>

class MainWindow : public QMainWindow
{
   Q_OBJECT
public:
   MainWindow();
   ~MainWindow() override;
   void createMenuBar();
public slots:
   void onMenuFileOpen();
protected:
   // de::audio::MixerStream m_endPoint;
};
