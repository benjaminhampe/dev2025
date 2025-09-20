#pragma once
#include <QWidget>
#include <QKeyEvent>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QDial>

//#include "MidiFilePlayerWidget.hpp"
#include "GeneralMidi.hpp"
#include "RtMidiKeyboard.hpp"

struct CC_Control : public QWidget
{
   Q_OBJECT
   RtMidiKeyboard* m_midi;
   QCheckBox* m_chkEnabled;
   QComboBox* m_cbxCC;
   QDial* m_dial;
   int m_channel;
   int m_cc;
   int m_value;
public:
   CC_Control( int channel, int cc, RtMidiKeyboard* kb, QWidget* parent )
      : QWidget( parent )
      , m_midi( kb )
      , m_chkEnabled( nullptr )
      , m_cbxCC( nullptr )
      , m_dial( nullptr )
      , m_channel( channel )
      , m_cc( cc )
      , m_value( 0 )
   {
      m_cbxCC = new QComboBox( this );
      for ( int i = 0; i < 16; i++ )
      {
         m_cbxCC->addItem( QString( "%1" )
            .arg( QString::fromStdString( de::CC_toString(i) ) ), QVariant(i) );
      }
      m_cbxCC->setCurrentIndex( 0 );

      connect( m_cbxCC, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboCC(int)) );

      auto h = new QHBoxLayout();
      //h->setContentsMargins(0,0,0,0);
      h->addWidget( new QLabel("CC: ") );
      h->addWidget( m_cbxCC, 1 );
      h->addWidget( m_dial );
      setLayout( h );
   }

protected slots:

   void onComboCC( int index )
   {
      m_cc = index;

      if ( m_midi )
      {
         std::vector< uint8_t > msg;
         msg.emplace_back( uint8_t(0xB0) | (uint8_t(m_channel) & 0x0F ) );
         msg.emplace_back( m_cc );
         msg.emplace_back( m_value );
         m_midi->sendMessage( msg );
      }
      // setFocus();
   }
};
