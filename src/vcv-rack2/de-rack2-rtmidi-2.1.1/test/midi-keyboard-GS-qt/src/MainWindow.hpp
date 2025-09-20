#pragma once
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QWidget>
#include <QKeyEvent>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTextEdit>

//#include "MidiFilePlayerWidget.hpp"
#include "GeneralMidi.hpp"
#include "RtMidiKeyboard.hpp"

struct MainWindow : public QMainWindow
{
   Q_OBJECT
   RtMidiKeyboard* m_amsynth;
   //MidiFilePlayerWidget* m_midiFilePlayer;
   int m_channel;
   int m_program;

   QTextEdit* m_logBox;

public:
   MainWindow( RtMidiKeyboard* amsynth )
      : m_amsynth( amsynth )
      , m_channel( 0 )
      , m_program( 0 )
   {
      setWindowTitle( QString::fromStdString( "MidiKeyboard | Microsoft GS Wavetable Synth | Qt | RtMidi" ) );
      setWindowIcon( QIcon(":/winico") ); // Compile in res/resource.qrc, see CMakeLists.txt

      auto statusText = QString("<benjaminhampe@gmx.de> | Press keys on computer keyboard for noteOn/Off events.");
      statusBar()->showMessage( statusText );

      auto v = new QVBoxLayout();
      //v->setContentsMargins(0,0,0,0);

      auto channelCombo = new QComboBox( this );
      for ( int i = 0; i < 16; i++ )
      {
         channelCombo->addItem( QString( "Channel %1" ).arg(i), QVariant(i) );
      }
      channelCombo->setCurrentIndex( 0 );

      connect( channelCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onChannelCombo(int)) );

      auto programCombo = new QComboBox( this );
      for ( int i = 0; i < 127; i++ )
      {
         programCombo->addItem( QString( "%1" )
            .arg( QString::fromStdString( de::GM_toString(i) ) ), QVariant(i) );
      }
      programCombo->setCurrentIndex( 0 );

      connect( programCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onProgramCombo(int)) );


      auto h1 = new QHBoxLayout();
      h1->addWidget( new QLabel("Channel: ") );
      h1->addWidget( channelCombo, 1 );
      v->addLayout( h1 );

      auto h2 = new QHBoxLayout();
      h2->addWidget( new QLabel("Program: ") );
      h2->addWidget( programCombo, 1 );
      v->addLayout( h2 );

      m_logBox = new QTextEdit( this );
      m_logBox->setFocusPolicy( Qt::NoFocus );
      v->addWidget( m_logBox );

      auto body = new QWidget( this );
      body->setLayout( v );
      setCentralWidget( body );

      logString( QString("FocusPolicy = %1").arg(focusPolicy()) );
      setFocusPolicy( Qt::ClickFocus ); // Must be >0 or no key press events
      logString( QString("FocusPolicy = %1").arg(focusPolicy()) );

      setFocus();
   }

protected slots:

   void logString( QString msg )
   {
      m_logBox->append( msg );
   }

   void onChannelCombo( int index )
   {
      m_channel = index;
      logString( QString("Channel[%1]").arg(m_channel) );
      setFocus();
   }

   void onProgramCombo( int index )
   {
      m_program = index;
      logString( QString("Channel[%1] %2").arg(m_channel).arg( QString::fromStdString( de::GM_toString(m_program) ) ) );

      if ( m_amsynth )
      {
         std::vector< uint8_t > msg;
         msg.emplace_back( uint8_t(0xC0) | (uint8_t(m_channel) & 0x0F ) );
         msg.emplace_back( m_program );
         m_amsynth->sendMessage( msg );
      }
      setFocus();
   }

protected:

   void noteOn( int midiNote, int velocity = 111 )
   {
      if ( m_amsynth ) m_amsynth->sendNoteOn( m_channel, midiNote, velocity );
      logString( QString("NoteOn ch(%1) note(%2) velocity(%3)")
         .arg(m_channel).arg(midiNote).arg(velocity) );
   }

   void noteOff( int midiNote, int velocity = 100 )
   {
      if ( m_amsynth ) m_amsynth->sendNoteOff( m_channel, midiNote, velocity );
      logString( QString("NoteOff ch(%1) note(%2) velocity(%3)")
         .arg(m_channel).arg(midiNote).arg(velocity) );
   }

   void keyPressEvent( QKeyEvent* event ) override
   {
      int const key = event->key();
      //logString( QString("KeyPress(%1)").arg(key) );

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
      //logString( QString("KeyRelease(%1)").arg(key) );

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
};
