#include "GSequencerCtl.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>

GSequencerCtl::GSequencerCtl( QWidget* parent )
   : QWidget( parent )
{
   setObjectName( "GSequencerCtl" );
   setContentsMargins(0,0,0,0);

   m_playButton = new QPushButton( "Play", this );
   m_stopButton = new QPushButton( "Stop", this );
   m_sequencer = new GSequencer( this );
   m_scrollX = new QScrollBar( Qt::Horizontal, this );
   m_scrollY = new QScrollBar( Qt::Vertical, this );
   auto v = new QVBoxLayout();
   v->setContentsMargins(0,0,0,0);
   v->setSpacing( 1 );
   v->addWidget( m_playButton );
   v->addWidget( m_stopButton );

   auto hScroll = new QHBoxLayout();
   hScroll->setContentsMargins(0,0,0,0);
   hScroll->setSpacing( 1 );
   hScroll->addWidget( m_sequencer,1 );
   hScroll->addWidget( m_scrollY );

   auto vScroll = new QVBoxLayout();
   vScroll->setContentsMargins(0,0,0,0);
   vScroll->setSpacing( 1 );
   vScroll->addLayout( hScroll );
   vScroll->addWidget( m_scrollX );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 1 );
   h->addLayout( v );
   h->addLayout( vScroll,1 );

   setLayout( h );



   connect( m_playButton, SIGNAL(clicked(bool)), this, SLOT(on_playButton(bool)) );
   connect( m_stopButton, SIGNAL(clicked(bool)), this, SLOT(on_stopButton(bool)) );
}

GSequencerCtl::~GSequencerCtl()
{
}

void
GSequencerCtl::on_playButton( bool checked )
{
   m_sequencer->play( 0 );
}

void
GSequencerCtl::on_stopButton( bool checked )
{
   m_sequencer->stop();
}

// void
// GSequencerCtl::hideEvent( QHideEvent* event )
// {
   // DE_DEBUG("")
   // QWidget::hideEvent( event );
// }

// void
// GSequencerCtl::showEvent( QShowEvent* event )
// {
   // DE_DEBUG("")
   // QWidget::showEvent( event );
   // //event->accept();
// }
