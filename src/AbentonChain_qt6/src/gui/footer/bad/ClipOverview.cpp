#include "ClipOverview.h"
#include <QResizeEvent>
#include "App.h"

ClipOverview::ClipOverview(QWidget* parent)
   : QWidget(parent)
   , m_isHovered( false )
   , m_isFocused( false )
{
    setObjectName( "ClipOverview" );
    setContentsMargins( 0,0,0,0 );

    m_imgSrc = QImage( 64,16,QImage::Format_ARGB32 );
    m_imgDst = QImage( 64,16,QImage::Format_ARGB32 );

    setMinimumWidth(64);
    setMinimumHeight(32);
}

ClipOverview::~ClipOverview()
{}

QSize
ClipOverview::computeBestSize() const
{
   QSize ts = QFontMetrics( font() ).tightBoundingRect( m_text ).size();
   QSize is = m_imgDst.size();
   int w = ts.width() + is.width() + 3;
   int h = std::max( ts.height(), is.height() );
   return QSize( w,h );
}


void
ClipOverview::resizeEvent( QResizeEvent* event )
{
   QWidget::resizeEvent( event );
   //update();
}

void
ClipOverview::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();

   if ( w > 0 && h > 0 )
   {
      QPainter dc( this );
      //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

      // Background
      dc.fillRect( rect(), QColor( 200,200,255 ) );

      // Clip - TitleText
      auto const & fm = dc.fontMetrics();
      dc.drawText( 4, h - 4 - fm.ascent(), "ClipView" );

   }

}

void
ClipOverview::enterEvent( QEnterEvent* event )
{
   m_isHovered = true;
   QWidget::enterEvent( event );
}

void
ClipOverview::leaveEvent( QEvent* event )
{
   m_isHovered = false;
   QWidget::leaveEvent( event );
}



void
ClipOverview::focusInEvent( QFocusEvent* event )
{
   m_isFocused = true;
   update();
   QWidget::focusInEvent( event );
}

void
ClipOverview::focusOutEvent( QFocusEvent* event )
{
   m_isFocused = true;
   update();
   QWidget::focusOutEvent( event );
}



void
ClipOverview::mouseMoveEvent( QMouseEvent* event )
{
   int mx = event->x();
   int my = event->y();
   QWidget::mouseMoveEvent( event );
}

void
ClipOverview::mousePressEvent( QMouseEvent* event )
{
   QWidget::mousePressEvent( event );
}

void
ClipOverview::mouseReleaseEvent( QMouseEvent* event )
{
   //   int mx = event->x();
   //   int my = event->y();
   emit clicked( true );

   QWidget::mouseReleaseEvent( event );
}

void
ClipOverview::wheelEvent( QWheelEvent* event )
{
   //   int mx = event->x();
   //   int my = event->y();
   //   me.m_wheelY = event->angleDelta().y();
   QWidget::wheelEvent( event );
}

void
ClipOverview::keyPressEvent( QKeyEvent* event )
{
   QWidget::keyPressEvent( event );
}

void
ClipOverview::keyReleaseEvent( QKeyEvent* event )
{
   QWidget::keyReleaseEvent( event );
}
