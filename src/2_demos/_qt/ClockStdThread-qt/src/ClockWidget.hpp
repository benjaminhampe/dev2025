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

#include "ClockStdThread.hpp"
#include "ClockBeginThreadEx.hpp"

//////////////////////////////////////////////////////////////////////////////////
struct NonAllocStringContainer
//////////////////////////////////////////////////////////////////////////////////
{
   std::vector< char* > m_logStrings;
   size_t m_index;

   NonAllocStringContainer( size_t n = 100 )
   {
      m_logStrings.resize( n );
      for ( char* &e : m_logStrings )
      {
         e = new char[ 64 ];
      }
      clear();
   }

   ~NonAllocStringContainer()
   {
      for ( char* &e : m_logStrings )
      {
         delete [] e;
      }
   }

   void clear()
   {
      for ( char* &e : m_logStrings )
      {
         memset( e, 0, 64 );
      }
      m_index = 0;
   }

   void add( double nanoseconds )
   {
      if ( m_index < m_logStrings.size() )
      {
         memset( m_logStrings[ m_index ], 0, 64 );
         sprintf( m_logStrings[ m_index ], "%lf", 1e-6 * nanoseconds );
         ++m_index;
      }
   }

   void add( char* msg )
   {
      if ( m_index < m_logStrings.size() )
      {
         memcpy( m_logStrings[ m_index ], msg, 64 );
         ++m_index;
      }
   }

   bool isFull() const
   {
      return m_index >= m_logStrings.size();
   }
};

//////////////////////////////////////////////////////////////////////////////////
struct ClockWidget : public QWidget, public IClockThreadListener
//////////////////////////////////////////////////////////////////////////////////
{
   Q_OBJECT
   DE_CREATE_LOGGER("ClockWidget")
   //ClockStdThread m_clockThread;
   ClockBeginThreadEx m_clockThread;
   int m_playTimer;

   char* m_tmp;
   NonAllocStringContainer m_nasc;

   QPushButton* m_btnPlay;
   QPushButton* m_btnPause;
   QPushButton* m_btnStop;
   QLabel* m_lblPosition;
   QTextEdit* m_logBox;

   QPushButton* m_btnClear;
public:
   ClockWidget( QWidget* parent )
      : QWidget(parent)
      , m_playTimer( 0 )
   {
      m_tmp = new char[64];
      memset( m_tmp, 0, 64 );

      m_btnPlay = new QPushButton("Play", this);
      m_btnPause = new QPushButton("Pause", this);
      m_btnStop = new QPushButton("Stop", this);
      m_lblPosition = new QLabel("00:00", this);
      m_logBox = new QTextEdit( this );
      m_btnClear = new QPushButton("Clear", this);

      auto h1 = new QHBoxLayout();
      h1->addWidget( m_btnPlay );
      h1->addWidget( m_btnPause );
      h1->addWidget( m_btnStop );

      auto h2 = new QHBoxLayout();
      h2->addWidget( m_lblPosition );

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
      connect( m_btnClear, SIGNAL(clicked(bool)), this, SLOT(clearLogBox()) );

//      connect( this, SIGNAL(syncLogCharEvent(char*)),
//               this, SLOT(logChar(char*)) );

      connect( this, SIGNAL(asyncLogStringEvent(QString)),
               this, SLOT(logString(QString)), Qt::QueuedConnection );

      m_clockThread.setListener( this );
   }

   ~ClockWidget() override
   {
      delete [] m_tmp;
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

   void clockWorkOrange( int64_t nanoseconds ) override
   {
      QString msg = QString( "%1" ).arg(1e-6 * nanoseconds);
      emit asyncLogStringEvent( std::move( msg ) );

//      memset( m_tmp, 0, 64 );
//      sprintf( m_tmp, "%lf", 1e-6 * nanoseconds );
//      emit syncLogCharEvent( m_tmp );

      //uint64_t tick = m_midiFile.getTickFromNanosecond( 1e-9 * nanosecondsNow );

   }

signals:
   void asyncLogStringEvent( QString msg );
   void syncLogCharEvent( char* msg );

public slots:
   void logString( QString msg )
   {
      m_logBox->append( msg );
   }

   void logChar( char* msg )
   {
      m_nasc.add( msg );
      if ( m_nasc.isFull() )
      {
         for ( size_t i = 0; i < m_nasc.m_logStrings.size(); ++i )
         {
            m_logBox->append( QString::fromLatin1( m_nasc.m_logStrings[i] ) );
         }
         m_nasc.clear();
      }
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
   }

   void clearLogBox()
   {
      m_logBox->clear();
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
      QWidget::paintEvent(event);
   }


};

