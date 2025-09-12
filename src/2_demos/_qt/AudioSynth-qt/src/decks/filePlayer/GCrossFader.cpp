#include "GCrossFader.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GCrossFader::GCrossFader( QWidget* parent )
   : QWidget(parent)
   , m_leftSignal( nullptr )
   , m_rightSignal( nullptr )
   , m_volumeL( 100 )
   , m_volumeR( 100 )
   , m_fader( 0 )
{
   setObjectName( "GCrossFader" );
   setContentsMargins(0,0,0,0);

   m_leftSlider = new GSliderV( "L", 100,0,100,"%",1, this );
   m_rightSlider = new GSliderV("R",100,0,100,"%",1, this );
   m_faderSlider = new QSlider( Qt::Horizontal, this );
   m_faderSlider->setMinimum( -100 );
   m_faderSlider->setMaximum( 100 );
   m_faderSlider->setValue( 0 );

   m_centerWidget = new QImageWidget( this );

   QImage f1;
   f1.load( "../../media/rack_ui/crossfader1.png");
   m_centerWidget->setImage( f1 );
   m_centerWidget->setImageScaling( false );
   m_centerWidget->setMinimumSize( f1.size() );
//   m_fadeSlider->setMinimumWidth( 64 );
//   m_leftSlider->setMinimumHeight( 64 );
//   m_rightSlider->setMinimumHeight( 64 );

   QLabel* m_title = new QLabel("CrossFader",this );

   QVBoxLayout* center = new QVBoxLayout();
   center->setContentsMargins(0,0,0,0);
   center->setSpacing( 3 );
   center->addWidget( m_title );
   center->addStretch( 1 );
   center->addWidget( m_centerWidget );
   center->addStretch( 1 );
   center->setAlignment( m_title, Qt::AlignHCenter );

   QHBoxLayout* h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 0 );
   h->addWidget( m_leftSlider );
   h->addLayout( center );
   h->addWidget( m_rightSlider );

   QVBoxLayout* v = new QVBoxLayout();
   v->setContentsMargins(0,0,0,0);
   v->setSpacing( 3 );
   v->addLayout( h );
   v->addWidget( m_faderSlider );

   setLayout( v );

   m_leftBuffer.resize( 2*1024, 0.0f );
   m_rightBuffer.resize( 2*1024, 0.0f );

   connect( m_leftSlider, SIGNAL(valueChanged(int)),
            this, SLOT(on_leftSlider(int)), Qt::QueuedConnection );

   connect( m_rightSlider, SIGNAL(valueChanged(int)),
            this, SLOT(on_rightSlider(int)), Qt::QueuedConnection );

   connect( m_faderSlider, SIGNAL(valueChanged(int)),
            this, SLOT(on_faderSlider(int)), Qt::QueuedConnection );
}

GCrossFader::~GCrossFader()
{
   // stopUpdateTimer();
}

void GCrossFader::clearInputSignals()
{
   m_leftSignal = nullptr;
   m_rightSignal = nullptr;
}

void
GCrossFader::setInputSignal( int i, de::audio::IDspChainElement* pSignal )
{
   if ( i == 1 )
   {
      m_rightSignal = pSignal;
   }
   else
   {
      m_leftSignal = pSignal;
   }
}

void
GCrossFader::setInputVolume( int i, int volume )
{
   if ( i == 1 )
   {
      //m_rightSignal = pSignal;
      m_rightSlider->setValue( std::clamp( volume, 0, 200 ) );
   }
   else
   {
      //m_leftSignal = pSignal;
      m_leftSlider->setValue( std::clamp( volume, 0, 200 ) );
   }
}

void
GCrossFader::setFader( float fader )
{
   m_faderSlider->setValue( std::clamp( int(fader * 100.0f), -100, 100 ) );
}

void
GCrossFader::on_leftSlider( int value )
{
   m_volumeL = std::clamp( value, 0, 100 );
   //m_labelB->setText( QString::number( m_volume ) + " %" );
   //DE_DEBUG("value(",value,"), m_volumeL = ",m_volumeL)
}

void
GCrossFader::on_rightSlider( int value )
{
   m_volumeR = std::clamp( value, 0, 100 );
   //m_labelB->setText( QString::number( m_volume ) + " %" );
   //DE_DEBUG("value(",value,"), m_volumeR = ",m_volumeR)
}

void
GCrossFader::on_faderSlider( int value )
{
   m_fader = std::clamp( 0.01f * value, -1.0f, 1.0f );
   //m_labelB->setText( QString::number( m_volume ) + " %" );
   DE_DEBUG("value(",value,"), m_iFadeAmount = ",m_fader)
}

uint64_t
GCrossFader::readDspSamples(
   double pts,
   float* dst,
   uint32_t dstFrames,
   uint32_t dstChannels,
   double dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;
   DSP_RESIZE( m_leftBuffer, dstSamples );
   DSP_FILLZERO( m_leftBuffer.data(), dstSamples );
   DSP_RESIZE( m_rightBuffer, dstSamples );
   DSP_FILLZERO( m_rightBuffer.data(), dstSamples );

   if ( m_leftSignal )
   {
      m_leftSignal->readDspSamples( pts, m_leftBuffer.data(),
         dstFrames, dstChannels, dstRate );
   }

   if ( m_rightSignal )
   {
      m_rightSignal->readDspSamples( pts, m_rightBuffer.data(),
         dstFrames, dstChannels, dstRate );
   }

   float fader = std::clamp( m_fader, -1.0f, 1.0f );
   float volumeL = 1.0f;
   float volumeR = 1.0f;
   if ( !almostEqual( fader, 0.0f, 1e-5f ) )
   {
      if ( fader < 0.0f )
      {
         volumeL = 1.0f;
         volumeR = 1.0f + fader;
      }
      else // if ( fader > 0.0f )
      {
         volumeL = 1.0f - fader;
         volumeR = 1.0f;
      }
   }

   volumeL *= 0.0001f * ( m_volumeL * m_volumeL );
   volumeR *= 0.0001f * ( m_volumeR * m_volumeR );

   auto srcL = m_leftBuffer.data();
   auto srcR = m_rightBuffer.data();
   auto pDst = dst;

   for ( uint64_t i = 0; i < dstFrames; ++i )
   {
      for ( size_t c = 0; c < dstChannels; ++c )
      {
         float sL = volumeL * (*srcL++);
         float sR = volumeR * (*srcR++);
         float mix = sL + sR;
         *pDst = mix;
         pDst++;
      }
   }

   return dstSamples;
}



/*
void
GCrossFader::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_updateTimerId )
   {
      update();
   }
}

// ============================================================================
void GCrossFader::paintEvent( QPaintEvent* event )
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
