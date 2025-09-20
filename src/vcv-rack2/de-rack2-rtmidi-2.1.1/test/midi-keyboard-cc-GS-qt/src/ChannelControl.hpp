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

#include "GeneralMidi.hpp"
#include "RtMidiKeyboard.hpp"

#include "CC_Control.hpp"

struct ChannelControl : public QWidget
{
   Q_OBJECT
public:

   RtMidiKeyboard* m_midi;
   QCheckBox* m_chkEnabled;
   QComboBox* m_cbxProgram;
   int m_channel;
   int m_program;
   std::vector< CC_Control* > m_ccs;

   ChannelControl( int channel, RtMidiKeyboard* kb, QWidget* parent )
      : QWidget( parent )
      , m_midi( kb )
      , m_chkEnabled( nullptr )
      , m_cbxProgram( nullptr )
      , m_channel( channel )
      , m_program( 0 )
   {
      m_chkEnabled = new QCheckBox( QString("Channel[%1]").arg(channel), this );
      m_chkEnabled->setCheckable( true );

      if ( m_channel == 0 )
      {
         m_chkEnabled->setChecked( true );
      }

      m_cbxProgram = new QComboBox( this );
      for ( int i = 0; i < 128; i++ )
      {
         m_cbxProgram->addItem( QString( "%1" )
            .arg( QString::fromStdString( de::GM_toString(i) ) ), QVariant(i) );
      }
      m_cbxProgram->setCurrentIndex( 0 );

      connect( m_cbxProgram, SIGNAL(currentIndexChanged(int)), this, SLOT(onProgramCombo(int)) );

      auto h = new QHBoxLayout();
      h->setContentsMargins(0,0,0,0);
      h->addWidget( m_chkEnabled );
      h->addWidget( m_cbxProgram, 1 );
      setLayout( h );
   }

protected slots:

   void onProgramCombo( int index )
   {
      m_program = index;

      if ( m_midi )
      {
         std::vector< uint8_t > msg;
         msg.emplace_back( uint8_t(0xC0) | (uint8_t(m_channel) & 0x0F ) );
         msg.emplace_back( m_program );
         m_midi->sendMessage( msg );
      }
      parentWidget()->setFocus();
   }

};

