#include "ArrangeContent.h"
#include "App.h"
#include "Draw.h"

// ============================================================================
Arrangement::Arrangement( App & app, QWidget* parent )
   : QWidget( parent )
   , m_app( app )
{
   setObjectName( "Arrangement" );
   setContentsMargins(0,0,0,0);
   setMouseTracking( true );
   //setMinimumWidth( 250 );

   m_hasFocus = false;
   m_isHovered = false;
   m_master = nullptr;
   m_selected = nullptr;
   m_headHeight = 20;
   m_setHeight = 24;
   m_pinHeight = 8;
   m_trackHeight = 64; //16 + 17;
   m_trackSpace = 2;
   m_footHeight = m_headHeight;

   m_dropTarget = new ArrangeDropTarget( m_app, this );
}

void
Arrangement::updateLayout()
{
   int w = width();
   int h = height();

   int x = 0;
   int y = 0;

   //int avail = h - int(m_returns.size() + 1) * m_trackHeight;

   for ( int i = 0; i < m_user.size(); ++i )
   {
      setWidgetBounds( m_user[ i ], QRect( x,y,w,m_trackHeight ) );
      y += m_trackHeight + 1;
   }


   int h_dt = h - (y + 1) - ( 1 + int(m_returns.size()) ) * (m_trackHeight + 1);
   setWidgetBounds( m_dropTarget, QRect( x,y,w,h_dt ) );

   y = h - ( 1 + int(m_returns.size()) ) * (m_trackHeight + 1);

   for ( int i = 0; i < m_returns.size(); ++i )
   {
      setWidgetBounds( m_returns[ i ], QRect( x,y,w,m_trackHeight ) );
      y += m_trackHeight + 1;
   }

   setWidgetBounds( m_master, QRect( x,y,w,m_trackHeight ) );
}

void
Arrangement::resizeEvent( QResizeEvent* event )
{
   updateLayout();
   QWidget::resizeEvent( event );
}

void
Arrangement::paintEvent( QPaintEvent* event )
{
   QWidget::paintEvent( event );
}

void
Arrangement::enterEvent( QEnterEvent* event )
{
   m_isHovered = true;
   update();
   QWidget::enterEvent( event );
}
void
Arrangement::leaveEvent( QEvent* event )
{
   m_isHovered = false;
   update();
   QWidget::leaveEvent( event );
}

void
Arrangement::focusInEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusInEvent( event );
}

void
Arrangement::focusOutEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusOutEvent( event );
}

void
Arrangement::mouseMoveEvent( QMouseEvent* event )
{
   int mx = event->x();
   int my = event->y();
   //updateLayout();
   QWidget::mouseMoveEvent( event );
}

void
Arrangement::mousePressEvent( QMouseEvent* event )
{
   //updateLayout();
   QWidget::mousePressEvent( event );
}

void
Arrangement::mouseReleaseEvent( QMouseEvent* event )
{
   //   int mx = event->x();
   //   int my = event->y();

   QWidget::mouseReleaseEvent( event );
}

void
Arrangement::wheelEvent( QWheelEvent* event )
{
   //   int mx = event->x();
   //   int my = event->y();
   //   me.m_wheelY = event->angleDelta().y();
   QWidget::wheelEvent( event );
}

void
Arrangement::keyPressEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyPress(",event->key(),")")
   QWidget::keyPressEvent( event );
}

void
Arrangement::keyReleaseEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyRelease(",event->key(),")")
   QWidget::keyReleaseEvent( event );
}
