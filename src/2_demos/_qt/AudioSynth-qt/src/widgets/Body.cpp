#include "Body.hpp"
#include <QTimerEvent>
#include <QPainter>
#include <iostream>

Body::Body( QWidget* parent )
   : QSplitter( Qt::Vertical, parent)
   , m_updateTimerId( 0 )
{
   setAttribute( Qt::WA_OpaquePaintEvent );
   setObjectName( "Body" );
   setContentsMargins( 0,0,0,0 );
   setChildrenCollapsible( true );
   startUpdateTimer();
}

Body::~Body()
{
   stopUpdateTimer();
}

void Body::startUpdateTimer()
{
//   if ( m_updateTimerId == 0 )
//   {
//      m_updateTimerId = startTimer( 67, Qt::CoarseTimer );
//      std::cout << "Body.startUpdateTimer()\n";
//   }
}

void Body::stopUpdateTimer()
{
//   if ( m_updateTimerId != 0 )
//   {
//      std::cout << "Body.stopUpdateTimer()\n";
//      killTimer( m_updateTimerId );
//      m_updateTimerId = 0;
//   }
}

void
Body::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_updateTimerId )
   {
      update();
   }
}

void
Body::paintEvent( QPaintEvent* event )
{
  //std::cout << "Body.paintEvent()\n";
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

