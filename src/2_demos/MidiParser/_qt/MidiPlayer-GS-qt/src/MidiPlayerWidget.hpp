#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>

#include <QTimerEvent>

#include <QTextEdit>

#include "MidiPlayer.hpp"
#include "ClockThread.hpp"

//////////////////////////////////////////////////////////////////////////////////
struct MidiPlayerWidget : public QWidget, public IClockThreadListener
//////////////////////////////////////////////////////////////////////////////////
{
   Q_OBJECT
   DE_CREATE_LOGGER("MidiPlayerWidget")
   //de::midi::MidiPlayer m_midiPlayer;
   ClockThread m_clockThread;
   int m_playTimer;

   RtMidiKeyboard m_midiOutput;
   de::midi::file::File m_midiFile;
   de::midi::file::Listener m_midiFileListener;
   de::midi::Parser m_midiParser;
   //MidiPlayerThread m_worker;

   QPushButton* m_btnPlay;
   QPushButton* m_btnPause;
   QPushButton* m_btnStop;
   QLabel* m_lblFileName;
   QPushButton* m_btnLoad;
   QLabel* m_lblPosition;
   QSlider* m_sldPosition;
   QLabel* m_lblDuration;
   QTextEdit* m_logBox;

   QPushButton* m_btnClear;
public:
   MidiPlayerWidget( QWidget* parent )
      : QWidget(parent)
      , m_playTimer( 0 )
   {
      m_btnPlay = new QPushButton("Play", this);
      m_btnPause = new QPushButton("Pause", this);
      m_btnStop = new QPushButton("Stop", this);
      m_lblFileName = new QLabel("Unknown.mid", this);
      m_btnLoad = new QPushButton("Load Midi file", this);
      m_lblPosition = new QLabel("00:00", this);
      m_sldPosition = new QSlider( Qt::Horizontal, this );
      m_sldPosition->setMinimum(0);
      m_sldPosition->setMaximum(1000);
      m_lblDuration = new QLabel("00:00", this);
      m_logBox = new QTextEdit( this );
      m_btnClear = new QPushButton("Clear", this);

      auto h1 = new QHBoxLayout();
      h1->addWidget( m_btnPlay );
      h1->addWidget( m_btnPause );
      h1->addWidget( m_btnStop );
      h1->addWidget( m_lblFileName, 1 );
      h1->addWidget( m_btnLoad );

      auto h2 = new QHBoxLayout();
      h2->addWidget( m_lblPosition );
      h2->addWidget( m_sldPosition, 1 );
      h2->addWidget( m_lblDuration );

      auto v3 = new QVBoxLayout();
      v3->addWidget( m_btnClear );

      auto h3 = new QHBoxLayout();
      h3->addWidget( m_logBox,1 );
      h3->addLayout( v3 );

      auto v = new QVBoxLayout();
      v->addLayout( h1 );
      v->addLayout( h2 );
      v->addLayout( h3 );

      setLayout( v );

      connect( m_btnPlay, SIGNAL(clicked(bool)), this, SLOT(play()) );
      connect( m_btnPause, SIGNAL(clicked(bool)), this, SLOT(pause()) );
      connect( m_btnStop, SIGNAL(clicked(bool)), this, SLOT(stop()) );
      connect( m_btnLoad, SIGNAL(clicked(bool)), this, SLOT(load()) );
      connect( m_btnClear, SIGNAL(clicked(bool)), this, SLOT(clearLogBox()) );

      connect( m_sldPosition, SIGNAL(valueChanged(int)), this, SLOT(positionSlider(int)) );

      connect( this, SIGNAL(asyncLogStringEvent(QString)),
               this, SLOT(logString(QString)), Qt::QueuedConnection );

      m_clockThread.setListener( this );

      // Connect MIDI File to Listener
      m_midiFileListener.setFile( &m_midiFile );
      // Connect MIDI Listener to Parser
      m_midiParser.addListener( &m_midiFileListener );

   }

   void clockThreadStarted() override
   {
      DE_DEBUG("")
   }

   void clockThreadStopped() override
   {
      m_lblPosition->setText( QString::fromStdString( de::StringUtil::seconds(0.0) ) );
   }

   void clockThreadPaused() override
   {
      DE_DEBUG("")
   }

   void clockThreadResumed() override
   {
      DE_DEBUG("")
   }

   void clockWorkOrange( int64_t nanosecondsNow ) override
   {
      QString msg = QString( "%1" ).arg(1e-6 * nanosecondsNow);
      emit asyncLogStringEvent( std::move( msg ) );

      //uint64_t tick = m_midiFile.getTickFromNanosecond( 1e-9 * nanosecondsNow );

   }

signals:
   void asyncLogStringEvent( QString msg );

public slots:
   void logString( QString msg )
   {
      m_logBox->append( msg );
   }

   void load()
   {
      QString fileNameQ = QFileDialog::getOpenFileName( this,
         tr("Open MIDI file (.mid, .midi, .smf)"),
         "",
         tr("Image Files (*.mid *.midi *.smf)") );

      std::string fileName = fileNameQ.toStdString();
      if ( fileName.empty() )
      {
         DE_WARN("Got empty fileName")
         return;
      }

      DE_DEBUG("Load MIDI file ", fileName)

      stop();
      m_midiFile.reset();
      m_midiParser.parse( fileName );
      m_lblFileName->setText( fileNameQ );
      m_lblPosition->setText(QString("00:00"));
      m_lblDuration->setText(QString::fromStdString( de::StringUtil::seconds( m_midiFile.getDurationInSec() )));
      m_logBox->clear();
      //play();
   }

   void play()
   {
      m_clockThread.play();
      startPlayTimer();
   }

   void pause()
   {
      m_clockThread.pause();
   }

   void stop()
   {
      m_clockThread.stop();
      stopPlayTimer();

      blockSignals( true );

      m_sldPosition->setValue(0);

      blockSignals( false );
   }

   void clearLogBox()
   {
      m_logBox->clear();
   }

   void positionSlider(int value)
   {
      logString( QString("PositionSlider = %1").arg(value) );
   }

protected:

   void startPlayTimer()
   {
      stopPlayTimer();
      m_playTimer = startTimer( 100 );
   }

   void stopPlayTimer()
   {
      if ( m_playTimer )
      {
         killTimer( m_playTimer );
         m_playTimer = 0;
      }
   }

   void timerEvent( QTimerEvent* event ) override
   {
      if ( event->timerId() == m_playTimer )
      {
         double const time = m_clockThread.getTimeInSeconds();
         m_lblPosition->setText( QString::fromStdString( de::StringUtil::seconds(time) ) );
      }
   }

   void paintEvent( QPaintEvent* event ) override
   {
      /*
      QPainter dc( this );
      dc.drawImage( rect(), m_imgBackground.img, m_imgBackground.img.rect() );

      QFont font("Tahoma", 12, 400, true );
      dc.setFont( font );
      dc.setPen( QColor(255,128,20) );

      dc.drawText( 300,360, "Win64 port 2023 by" );
      dc.drawText( 280,380, "benjaminhampe@gmx.de" );

      */
      QWidget::paintEvent(event);
   }


};

