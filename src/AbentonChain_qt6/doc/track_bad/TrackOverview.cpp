#include "TrackOverview.h"
#include <QResizeEvent>
#include "App.h"
// #include "Draw.h"

TrackOverview::TrackOverview( QWidget* parent )
   : QWidget(parent)
   , m_isHovered( false )
   , m_isFocused( false )
   , m_text("1-Audio")
{
    setObjectName("TrackOverview");
    setContentsMargins( 0,0,0,0 );

    m_imgSrc = QImage( 64,16,QImage::Format_ARGB32 );
    m_imgDst = QImage( 64,16,QImage::Format_ARGB32 );

    setMinimumWidth(64);
    setMinimumHeight(32);
}

TrackOverview::~TrackOverview()
{}

void
TrackOverview::setText( QString txt )
{
   m_text = txt;
   m_textSize = QFontMetrics( font() ).tightBoundingRect( m_text ).size();
   parentWidget()->update();
   //update();
}

QSize
TrackOverview::computeBestSize() const
{
   QSize ts = m_textSize;
   QSize is = m_imgDst.size();
   int w = ts.width() + is.width() + 3;
   int h = std::max( ts.height(), is.height() );
   return QSize( w,h );
}

void
TrackOverview::resizeEvent( QResizeEvent* event )
{
   QWidget::resizeEvent( event );
}

void TrackOverview::paintEvent( QPaintEvent* event )
{
    int w = width();
    int h = height();
    if ( w > 0 && h > 0 )
    {
        QPainter dc( this );
        //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
        dc.fillRect( rect(), QColor( 230,200,255 ) );


        auto const & fm = dc.fontMetrics();
        dc.drawText( 4, h - 4 - fm.ascent(), "TrackView" );

        dc.drawImage( 50,0, m_imgDst );
    }

    //QWidget::paintEvent( event );
}


void
TrackOverview::focusInEvent( QFocusEvent* event )
{
   m_isFocused = true;
   update();
   QWidget::focusInEvent( event );
}

void
TrackOverview::focusOutEvent( QFocusEvent* event )
{
   m_isFocused = true;
   update();
   QWidget::focusOutEvent( event );
}

/*

void
TrackOverview::enterEvent( QEnterEvent* event )
{
   m_isHovered = true;
   QWidget::enterEvent( event );
}

void
TrackOverview::leaveEvent( QEvent* event )
{
   m_isHovered = false;
   QWidget::leaveEvent( event );
}


void
TrackOverview::mouseMoveEvent( QMouseEvent* event )
{
   int mx = event->x();
   int my = event->y();
   QWidget::mouseMoveEvent( event );
}

void
TrackOverview::mousePressEvent( QMouseEvent* event )
{
   QWidget::mousePressEvent( event );
}

void
TrackOverview::mouseReleaseEvent( QMouseEvent* event )
{
   //   int mx = event->x();
   //   int my = event->y();
   emit clicked( true );

   QWidget::mouseReleaseEvent( event );
}

void
TrackOverview::wheelEvent( QWheelEvent* event )
{
   //   int mx = event->x();
   //   int my = event->y();
   //   me.m_wheelY = event->angleDelta().y();
   QWidget::wheelEvent( event );
}

void
TrackOverview::keyPressEvent( QKeyEvent* event )
{
   QWidget::keyPressEvent( event );
}

void
TrackOverview::keyReleaseEvent( QKeyEvent* event )
{
   QWidget::keyReleaseEvent( event );
}

*/
