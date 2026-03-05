#include "ClipContent.h"
#include <QResizeEvent>

ClipContent::ClipContent( LiveSkin& skin, QWidget* parent )
   : QWidget(parent)
   , m_skin( skin )
   , m_hasFocus( false )
{
   setObjectName( "ClipContent" );
   setContentsMargins( 0,0,0,0 );
   setMinimumHeight( 64 );

   m_clipEditor = new ClipEditor( skin, this );

   auto v = new QVBoxLayout();
   v->setContentsMargins( 1,1,1,1 );
   v->setSpacing( 1 );
   v->addWidget( m_clipEditor,1 );/*
   v->setAlignment( m_btnEnabled, Qt::AlignHCenter );
   v->setAlignment( m_title, Qt::AlignHCenter );
   v->setAlignment( m_btnMore, Qt::AlignHCenter );
   v->setAlignment( m_btnEditor, Qt::AlignHCenter );*/
   setLayout( v );

}

ClipContent::~ClipContent()
{
}

void
ClipContent::resizeEvent( QResizeEvent* event )
{
   QWidget::resizeEvent( event );

   int w = event->size().width();
   int h = event->size().height();
   //std::cout << "w = " << w << ", h = " << h << std::endl;

   //if ( w < 4 ) return;
   //if ( h < 4 ) return;

   //update();
}

void ClipContent::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();
   if ( w < 4 ) return;
   if ( h < 4 ) return;

   //std::cout << "w = " << w << ", h = " << h << std::endl;

   QPainter dc( this );
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
//   dc.fillRect( rect(), m_skin.panelBlendColor );
//   dc.fillRect( rect(), m_skin.panelBlendColor );

//   int r = skin.radius;
//   int x = 0;
//   int y = 0;
//   auto fgColor = hasFocus() ? skin.focusColor : skin.panelColor;
//   drawRoundRectFill( dc, x,y, w,h, fgColor, r,r );
//   //drawRoundRectFill( dc, x, y, w, h, bgColor, rx, ry );
//   //drawRoundRectFill( dc, x+1, y+1, w-2, h-2, fgColor, rx, ry );

   QWidget::paintEvent( event );
}

void
ClipContent::focusInEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusInEvent( event );
}

void
ClipContent::focusOutEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusOutEvent( event );
}

void
ClipContent::enterEvent( QEnterEvent* event )
{
   QWidget::enterEvent( event );
}

void
ClipContent::leaveEvent( QEvent* event )
{
   QWidget::leaveEvent( event );
}
