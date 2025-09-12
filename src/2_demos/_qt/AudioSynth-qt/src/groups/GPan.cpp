#include "GPan.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GPan::GPan( QWidget* parent )
   : QWidget(parent)
{
   setObjectName( "GPan" );
   setContentsMargins(0,0,0,0);

   m_menu = new GGroupV( "PAN", this );
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );

   m_panSlider = new GSliderV( "Pan", 0,0,100,"%",1, this );

   auto hBody = new QHBoxLayout();
   hBody->setContentsMargins(0,0,0,0);
   hBody->setSpacing( 0 );
   hBody->addWidget( m_panSlider );
   m_body->setLayout( hBody );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 0 );
   h->addWidget( m_menu );
   h->addWidget( m_body );
   setLayout( h );

   connect( m_menu, SIGNAL(toggledBypass(bool)), this, SLOT(on_menuButton_bypass(bool)), Qt::QueuedConnection );
   connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_menuButton_more(bool)), Qt::QueuedConnection );
   connect( m_menu, SIGNAL(toggledHideAll(bool)), this, SLOT(on_menuButton_hideAll(bool)), Qt::QueuedConnection );
   connect( m_panSlider, SIGNAL(valueChanged(int)), this, SLOT(on_panSlider(int)), Qt::QueuedConnection );
}

GPan::~GPan()
{
   // stopUpdateTimer();
}

void
GPan::on_menuButton_hideAll(bool checked)
{
   //m_panSlider->setVisible( !checked );
}

void
GPan::on_menuButton_bypass(bool checked)
{
   m_pan.setBypassed( checked );
}

void
GPan::on_menuButton_more(bool checked)
{

}

void
GPan::on_panSlider( int value )
{
   m_pan.setFader( 0.01f * value );
}

void
GPan::setFader( float fader )
{
   m_panSlider->setValue( int(fader * 100.0f) );
}

/*
void
GPan::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_updateTimerId )
   {
      update();
   }
}

// ============================================================================
void GPan::paintEvent( QPaintEvent* event )
// ============================================================================
{
   QPainter dc(this);
   dc.fillRect( rect(), QColor( 25,25,25 ) );

   int w = width();
   int h = height();
   int h2 = height() / 2;

   uint64_t srcSamples = m_shiftBuffer.size();
   uint64_t srcFrames = srcSamples / m_channelCount;
   uint64_t srcChannels = m_channelCount;
   if (srcFrames < 2) { return; }

   float const dx = float(w) / float(srcFrames);
   float const dy = float(h) * 0.5f;

   dc.setBrush( Qt::NoBrush );
   dc.setCompositionMode( QPainter::CompositionMode_SourceOver );

   for ( uint64_t c = 0; c < srcChannels; ++c )
   {
      if ( c == 0 )
         dc.setPen( QPen( QColor(255,55,55,63) ) );
      else if ( c == 1 )
         dc.setPen( QPen( QColor(255,225,55,63) ) );
      else if ( c == 2 )
         dc.setPen( QPen( QColor(55,200,55,63) ) );
      else
         dc.setPen( QPen( QColor(255,255,255,63) ) );

      for ( uint64_t i = 1; i < srcFrames; ++i )
      {
         const float sample1 = m_shiftBuffer[ srcChannels*(i-1) + c ];
         const float sample2 = m_shiftBuffer[ srcChannels*(i) + c ];
         const int x1 = dx * (i-1);
         const int x2 = dx * (i);
         const int y1 = dy - dy * sample1;
         const int y2 = dy - dy * sample2;
         dc.drawLine( x1,y1,x2,y2 );
      }
   }

   m_font5x8.drawText( dc, w/2, 1, QString("L_min(%1), L_max(%2), R_min(%3), R_max(%4)")
      .arg( L_min).arg( L_max ).arg( R_min).arg( R_max ), 0xFFFFFFFF, de::Align::CenterTop );
}
*/
