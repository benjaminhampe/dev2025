#pragma once
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QWidget>
#include <QKeyEvent>
#include "ClockWidget.hpp"

struct MainWindow : public QMainWindow
{
   MainWindow()
      : QMainWindow()
   {
      setWindowTitle( QString::fromStdString( "ClockStdThread-qt" ) );
      setWindowIcon( QIcon(":/winico") ); // Compile in res/resource.qrc, see CMakeLists.txt

      // auto statusText = QString("Audio: %1 @ %2Hz %3x %4 frames, MIDI: %5")
                     // .arg( QString::fromStdString( config.current_audio_driver ) )
                     // .arg( m_amsynth->m_sampleRate )
                     // .arg( m_amsynth->m_channels )
                     // .arg( m_amsynth->m_frames )
                     // .arg( QString::fromStdString( config.current_midi_driver ) );

      // statusBar()->showMessage( statusText );

      //GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
      auto vbox = new QVBoxLayout();
      vbox->setContentsMargins(0,0,0,0);

      auto clock = new ClockWidget( this );
      vbox->addWidget( clock );

      //gtk_container_add(GTK_CONTAINER(window), vbox);
      auto body = new QWidget( this );
      body->setLayout( vbox );
      setCentralWidget( body );
   }
};
