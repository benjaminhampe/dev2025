#pragma once
#include <QImageWidget.hpp>
#include <QPushButton>
#include <QFont5x8.hpp>
#include <DarkImage.h>

// ============================================================================
class GLevelMeterH : public QWidget
// ============================================================================
{
   Q_OBJECT
   int m_updateTimerId;
   float m_value;
   QColor m_bgColor;
   QImage m_img;

public:
   GLevelMeterH( QWidget* parent = 0 )
      : QWidget(parent)
      , m_updateTimerId( 0 )
      , m_value( 0.0f )
      , m_bgColor( 1,1,1 )
   {
      setObjectName( "GLevelMeterH" );
      setContentsMargins( 0,0,0,0 );
      setMinimumSize( 64, 5 );
      startUpdateTimer();
   }

   ~GLevelMeterH() override
   {
      stopUpdateTimer();
   }

signals:
public slots:
   void setValue( float t )
   {
      m_value = t;
      update();
   }

   void setImage( QImage const & img )
   {
      m_img = img;
      setMinimumSize(m_img.size());
      setMaximumSize(m_img.size());
   }

protected:

   void stopUpdateTimer()
   {
      if ( m_updateTimerId )
      {
         killTimer( m_updateTimerId );
         m_updateTimerId = 0;
      }
   }

   void startUpdateTimer()
   {
      stopUpdateTimer();
      m_updateTimerId = startTimer( 101, Qt::CoarseTimer );
   }

   void timerEvent( QTimerEvent* event ) override
   {
      if ( event->timerId() == m_updateTimerId )
      {
         repaint();
      }
   }

   void paintEvent( QPaintEvent* event ) override
   {
      int w = width();
      int h = height();

      QPainter dc(this);
      // dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

      // Draw Image
      dc.drawImage( 0,0, m_img, 0,0,-1,-1, Qt::NoOpaqueDetection );

      // Draw Cover Rect in Background color
      int px = int( m_value * w );
      int dx = w - px;

      //dc.setPen( QPen( QColor( 255,255,255,255 ) ) );
      dc.setPen( Qt::NoPen );
      dc.setBrush( QBrush( m_bgColor ) );
      dc.drawRect( QRect(px,0,dx,h) );
      //QWidget::paintEvent( event );
   }
};
