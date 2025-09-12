#pragma once
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QWidget>
#include <QKeyEvent>
#include "MidiPlayerWidget.hpp"

struct MainWindow : public QMainWindow
{
   MidiPlayerWidget* m_midiPlayer;

   MainWindow()
      : QMainWindow()
   {
      setWindowTitle( QString::fromStdString( "MidiPlayer-GS-qt" ) );
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

      m_midiPlayer = new MidiPlayerWidget( this );
      vbox->addWidget( m_midiPlayer );

      //gtk_container_add(GTK_CONTAINER(window), vbox);
      auto body = new QWidget( this );
      body->setLayout( vbox );
      setCentralWidget( body );
   }

protected:

/*
   void noteOn( int midiNote )
   {
      if ( m_amsynth ) m_amsynth->sendNoteOn( 0, midiNote, 127);
   }

   void noteOff( int midiNote )
   {
      if ( m_amsynth ) m_amsynth->sendNoteOff( 0, midiNote, 100);
   }

   void keyPressEvent( QKeyEvent* event ) override
   {
      int const key = event->key();

      if ( !event->isAutoRepeat() )
      {
         int k = 59;
         if ( key == Qt::Key_1 ) { noteOn( k ); } k++;
         if ( key == Qt::Key_Q ) { noteOn( k ); } k++;
         if ( key == Qt::Key_2 ) { noteOn( k ); } k++;
         if ( key == Qt::Key_W ) { noteOn( k ); } k++;
         if ( key == Qt::Key_3 ) { noteOn( k ); } k++;
         if ( key == Qt::Key_E ) { noteOn( k ); } k++;
         if ( key == Qt::Key_R ) { noteOn( k ); } k++;
         if ( key == Qt::Key_5 ) { noteOn( k ); } k++;
         if ( key == Qt::Key_T ) { noteOn( k ); } k++;
         if ( key == Qt::Key_6 ) { noteOn( k ); } k++;
         if ( key == Qt::Key_Z ) { noteOn( k ); } k++;
         if ( key == Qt::Key_7 ) { noteOn( k ); } k++;
         if ( key == Qt::Key_U ) { noteOn( k ); } k++;
         if ( key == Qt::Key_I ) { noteOn( k ); } k++;
         if ( key == Qt::Key_9 ) { noteOn( k ); } k++;
         if ( key == Qt::Key_O ) { noteOn( k ); } k++;
         if ( key == Qt::Key_0 ) { noteOn( k ); } k++;
         if ( key == Qt::Key_P ) { noteOn( k ); } k++;
         if ( key == Qt::Key_Udiaeresis ) { noteOn( k ); } k++;
         if ( key == Qt::Key_acute ) { noteOn( k ); } k++; // only works with long press, f Qt.
         if ( key == Qt::Key_Plus ) { noteOn( k ); } k++;

         k = 48-12;
         if ( key == Qt::Key_Greater || key == Qt::Key_Less ) { noteOn( k ); } k++;
         if ( key == Qt::Key_A ) { noteOn( k ); } k++;
         if ( key == Qt::Key_Y ) { noteOn( k ); } k++;
         if ( key == Qt::Key_S ) { noteOn( k ); } k++;
         if ( key == Qt::Key_X ) { noteOn( k ); } k++;
         //if ( key == Qt::Key_D ) { noteOn( k ); } k++;
         if ( key == Qt::Key_C ) { noteOn( k ); } k++;
         if ( key == Qt::Key_F ) { noteOn( k ); } k++;
         if ( key == Qt::Key_V ) { noteOn( k ); } k++;
         if ( key == Qt::Key_G ) { noteOn( k ); } k++;
         if ( key == Qt::Key_B ) { noteOn( k ); } k++;
         if ( key == Qt::Key_H ) { noteOn( k ); } k++;
         if ( key == Qt::Key_N ) { noteOn( k ); } k++;
         if ( key == Qt::Key_M ) { noteOn( k ); } k++;
         if ( key == Qt::Key_K ) { noteOn( k ); } k++;
         if ( key == Qt::Key_Comma ) { noteOn( k ); } k++;
         if ( key == Qt::Key_L ) { noteOn( k ); } k++;
         if ( key == Qt::Key_Period ) { noteOn( k ); } k++;
         if ( key == Qt::Key_Minus ) { noteOn( k ); } k++;
         if ( key == Qt::Key_Odiaeresis ) { noteOn( k ); } k++; // Ö
         if ( key == Qt::Key_Adiaeresis ) { noteOn( k ); } k++; // Ä
         if ( key == Qt::Key_NumberSign ) { noteOn( k ); } k++; // #
      }

      QWidget::keyPressEvent(event);
   }
   void keyReleaseEvent( QKeyEvent* event ) override
   {
      int const key = event->key();

      // printf("KeyCode = %d\n", key); fflush(stdout);

      if ( !event->isAutoRepeat() )
      {
         int k = 59;
         if ( key == Qt::Key_1 ) { noteOff( k ); } k++;
         if ( key == Qt::Key_Q ) { noteOff( k ); } k++;
         if ( key == Qt::Key_2 ) { noteOff( k ); } k++;
         if ( key == Qt::Key_W ) { noteOff( k ); } k++;
         if ( key == Qt::Key_3 ) { noteOff( k ); } k++;
         if ( key == Qt::Key_E ) { noteOff( k ); } k++;
         if ( key == Qt::Key_R ) { noteOff( k ); } k++;
         if ( key == Qt::Key_5 ) { noteOff( k ); } k++;
         if ( key == Qt::Key_T ) { noteOff( k ); } k++;
         if ( key == Qt::Key_6 ) { noteOff( k ); } k++;
         if ( key == Qt::Key_Z ) { noteOff( k ); } k++;
         if ( key == Qt::Key_7 ) { noteOff( k ); } k++;
         if ( key == Qt::Key_U ) { noteOff( k ); } k++;
         if ( key == Qt::Key_I ) { noteOff( k ); } k++;
         if ( key == Qt::Key_9 ) { noteOff( k ); } k++;
         if ( key == Qt::Key_O ) { noteOff( k ); } k++;
         if ( key == Qt::Key_0 ) { noteOff( k ); } k++;
         if ( key == Qt::Key_P ) { noteOff( k ); } k++;
         if ( key == Qt::Key_Udiaeresis ) { noteOff( k ); } k++;
         if ( key == Qt::Key_acute ) { noteOff( k ); } k++; // only works with long press, f Qt.
         if ( key == Qt::Key_Plus ) { noteOff( k ); } k++;
         k = 48-12;
         if ( key == Qt::Key_Greater || key == Qt::Key_Less ) { noteOff( k ); } k++;
         if ( key == Qt::Key_A ) { noteOff( k ); } k++;
         if ( key == Qt::Key_Y ) { noteOff( k ); } k++;
         if ( key == Qt::Key_S ) { noteOff( k ); } k++;
         if ( key == Qt::Key_X ) { noteOff( k ); } k++;
         //if ( key == Qt::Key_D ) { noteOff( k ); } k++;
         if ( key == Qt::Key_C ) { noteOff( k ); } k++;
         if ( key == Qt::Key_F ) { noteOff( k ); } k++;
         if ( key == Qt::Key_V ) { noteOff( k ); } k++;
         if ( key == Qt::Key_G ) { noteOff( k ); } k++;
         if ( key == Qt::Key_B ) { noteOff( k ); } k++;
         if ( key == Qt::Key_H ) { noteOff( k ); } k++;
         if ( key == Qt::Key_N ) { noteOff( k ); } k++;
         if ( key == Qt::Key_M ) { noteOff( k ); } k++;
         if ( key == Qt::Key_K ) { noteOff( k ); } k++;
         if ( key == Qt::Key_Comma ) { noteOff( k ); } k++;
         if ( key == Qt::Key_L ) { noteOff( k ); } k++;
         if ( key == Qt::Key_Period ) { noteOff( k ); } k++;
         if ( key == Qt::Key_Minus ) { noteOff( k ); } k++;
         if ( key == Qt::Key_Odiaeresis ) { noteOff( k ); } k++; // Ö
         if ( key == Qt::Key_Adiaeresis ) { noteOff( k ); } k++; // Ä
         if ( key == Qt::Key_NumberSign ) { noteOff( k ); } k++; // #
      }
      QWidget::keyReleaseEvent(event);
   }
*/
};
