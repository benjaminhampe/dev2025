#include "GRack.hpp"
#include <QTimerEvent>
#include <QPainter>
#include <iostream>

GRack::GRack( QWidget* parent )
   : QWidget(parent)
   , m_updateTimerId( 0 )
{
   //setAttribute( Qt::WA_OpaquePaintEvent );
   setObjectName( "GRack" );
   setContentsMargins( 0,0,0,0 );
   //startUpdateTimer();
}

GRack::~GRack()
{
   //stopUpdateTimer();
}

void GRack::startUpdateTimer()
{
   if ( m_updateTimerId == 0 )
   {
      m_updateTimerId = startTimer( 67, Qt::CoarseTimer );
      std::cout << "GRack.startUpdateTimer()\n";
   }
}

void GRack::stopUpdateTimer()
{
   if ( m_updateTimerId != 0 )
   {
      std::cout << "GRack.stopUpdateTimer()\n";
      killTimer( m_updateTimerId );
      m_updateTimerId = 0;
   }
}

void
GRack::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_updateTimerId )
   {
      update();
   }
}

void
GRack::paintEvent( QPaintEvent* event )
{
  //std::cout << "GRack.paintEvent()\n";
   // int w = width();
   // int h = height();

   // QPainter dc(this);
   // dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

   // // Draw deck background rectangle
   // dc.setPen( QPen( QColor( 255,255,255,255 ) ) );
   // dc.setBrush( QBrush( m_deckColor ) );
   // dc.drawRect( QRect(0,0,w-1,h-1) );

   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   dc.fillRect( rect(), QColor( 180,180,200 ) );
//   int w = width();
//   int h = height();

   QWidget::paintEvent( event );
}

