#pragma once
/// STL
#include <sstream>
#include <chrono>
#include <cmath>
/// QT
#include <QMainWindow>
#include <QScreen>
#include <QWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QDial>
#include <QDebug>
#include <QThread>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QScrollArea>
/// Basic Widgets - Benjamin Hampe (c) 2022 <benjaminhampe@gmx.de>
#include "GEngine.hpp"
#include "GVolume.hpp"
#include "GLevelMeter.hpp"
#include "GLFO.hpp"
#include "GADSR.hpp"
#include "GDelay.hpp"
#include "GMixer.hpp"
#include "GLowPass3.hpp"
#include "GBassBoost.hpp"
#include "GOverdrive.hpp"
#include "GLoudnessLFO.hpp"
#include "GSoundTouch.hpp"
#include "GOszilloskop.hpp"
#include "GSpektrometer.hpp"
#include "GOszilloskop3D.hpp"
#include "GPianoKeyboard.hpp"
#include "GMidiMasterKeyboard.hpp"

/// Body (GRack) + Children (GDeck) - Benjamin Hampe (c) 2022 <benjaminhampe@gmx.de>
#include "Body.hpp"
#include "GRack.hpp"
#include "GDeck.hpp"
#include "GMasterDeck.hpp"
#include "GAudioRecorder.hpp"
#include "GFilePlayerDeck.hpp"
#include "GMidiSequencer.hpp"
#include "GDetuneSynthDeck.hpp"
#include "GBetterSynthDeck.hpp"
#include "GVstDeck.hpp"
#include "GSequencerCtl.hpp"


/// (c) 2022 Benjamin Hampe <benjaminhampe@gmx.de>
class MainWindow : public QMainWindow
{
   Q_OBJECT
public:
   MainWindow();
   ~MainWindow() override;
   void createMenuBar();
public slots:
   void onShowAboutDialog();
   void onMenuFileExit();
   void onMenuFileOpen();
   void updateViewMenu();

   void noteOn( int midiNote, int velocity = 90 );
   void noteOff( int midiNote, int velocity = 90 );
protected:
   void keyPressEvent( QKeyEvent* event ) override;
   void keyReleaseEvent( QKeyEvent* event ) override;

//   void timerEvent( QTimerEvent* event) override;
//   void paintEvent( QPaintEvent* event ) override;
//   void resizeEvent( QResizeEvent* event ) override;

//   void mousePressEvent( QMouseEvent* event ) override;
//   void mouseReleaseEvent( QMouseEvent* event ) override;
//   void mouseMoveEvent( QMouseEvent* event ) override;
//   void wheelEvent( QWheelEvent* event ) override;

protected:
   DE_CREATE_LOGGER("MainWindow")

   std::vector< de::audio::IDspChainElement* > m_synths;

   QOpenGLContext* m_customContext;
   QOffscreenSurface* m_offscreenSurface;

   GEngine* m_audioEngine;
   GAudioRecorder* m_audioRecorder;
   GOszilloskop3D* m_waveform3D;
   GMasterDeck* m_master;
   GMixer* m_mixer;
   GFilePlayerDeck* m_deckAB;
   GFilePlayerDeck* m_deckCD;
   GFilePlayerDeck* m_deckEF;
   GVstDeck* m_vstDeck;
   GDetuneSynthDeck* m_detuneSynth;
   GBetterSynthDeck* m_betterSynth;
   GSequencerCtl* m_sequencer;
   //GPianoKeyboard* m_piano2D;
   GMidiMasterKeyboard* m_piano2D;
   //GMidiSequencer* m_midiSequencer;
   //Rack_Sequencer16* m_seq16;
   QMenuBar* m_menuBar;
   QMenu* m_fileMenu;
   QAction* m_act_open;
   QAction* m_act_exit;
   QMenu* m_viewMenu;
   QAction* m_act_show_master;
   QAction* m_act_show_oszi_3d;
   QAction* m_act_show_recorder;
   QAction* m_act_show_mixer;
   QAction* m_act_show_playerAB;
   QAction* m_act_show_playerCD;
   QAction* m_act_show_playerEF;
   QAction* m_act_show_seq16;
   QAction* m_act_show_detunesynth;
   QAction* m_act_show_bettersynth;
   QAction* m_act_show_vsthost;
   QAction* m_act_show_clipeditor;
   QAction* m_act_show_piano2d;
   QMenu* m_helpMenu;
   QAction* m_act_help_about;
};
