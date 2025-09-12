#include "GDeck.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace {

QColor toQColor( uint32_t color )
{
   int32_t r = dbRGBA_R(color);
   int32_t g = dbRGBA_G(color);
   int32_t b = dbRGBA_B(color);
   int32_t a = dbRGBA_A(color);
   return QColor( b,g,r,a );
}

}

GDeck::GDeck( QString title, QWidget* parent )
   : QWidget(parent)
   , m_inputSignal( nullptr )
   , m_body( nullptr )
//   , m_updateTimerId( 0 )
   , m_deckColor( 1,1,1,128 )
{
   setObjectName( title );
   setContentsMargins( 1,1,1,1 );
   setMinimumHeight(24);
   setMaximumHeight(24);

   m_title = new QImageWidget( this );
   m_title->setImageScaling( false );
   setTitleImageText( title );

   m_btnMinified = new QPushButton( "", this );
   m_btnMinified->setMinimumSize( 20,20 );
   m_btnMinified->setMaximumSize( 20,20 );
   //m_btnMinified->setToolTip( "This DSP element is fully visible now and (s)hown." );
   m_btnMinified->setCheckable( true );

   m_btnBypassed = new QPushButton( "", this );
   m_btnBypassed->setMinimumSize( 20,20 );
   m_btnBypassed->setMaximumSize( 20,20 );
   //m_btnBypassed->setToolTip("This DSP element is DISABLED (bypassed)");
   m_btnBypassed->setCheckable( true );

   m_btnExtraMore = new QPushButton( "", this );
   m_btnExtraMore->setMinimumSize( 20,20 );
   m_btnExtraMore->setMaximumSize( 20,20 );
   //m_btnExtraMore->setToolTip("All DSP options are visible now");
   m_btnExtraMore->setCheckable( true );

   m_levelL = new GLevelMeterH( this );
   m_levelR = new GLevelMeterH( this );

   auto v = new QVBoxLayout();
   v->setContentsMargins( 0,0,0,0 );
   v->setSpacing( 1 );
   v->addWidget( m_levelL );
   v->addWidget( m_levelR );
   setLayout( v );

   auto w = new QWidget( this );
   w->setContentsMargins( 0,0,0,0 );
   w->setLayout( v );

   auto h = new QHBoxLayout();
   h->setContentsMargins( 1,1,1,1 );
   h->setSpacing( 1 );
   h->addWidget( m_btnMinified );
   h->addWidget( m_btnBypassed );
   h->addWidget( m_btnExtraMore );
   h->addSpacing( 20 );
   h->addWidget( w );
   h->addSpacing( 20 );
   h->addWidget( m_title );
   h->addStretch( 1 );
   //h->addStretch( 1 );
   h->setAlignment( w, Qt::AlignVCenter );
   h->setAlignment( m_title, Qt::AlignVCenter );
   setLayout( h );

   m_ColorGradient.addStop( 0.0f, 0xFF000000 );
   m_ColorGradient.addStop( 0.1f, 0xFFFFFFFF );
   m_ColorGradient.addStop( 0.4f, 0xFF00FF00 );
   m_ColorGradient.addStop( 0.6f, 0xFF00FFFF );
   m_ColorGradient.addStop( 1.0f, 0xFF0000FF );
   m_ColorGradient.addStop( 1.1f, 0xFFFF00FF );

   // Feed LevelMeter
   m_Lmin = m_Lmax = m_Rmin = m_Rmax = 0.0f;

   // horizontal gradient filling
   QImage img( 128,5, QImage::Format_ARGB32 );
   float const fx = 1.2f / float( img.width() );
   for ( int32_t x = 0; x < img.width(); ++x )
   {
      uint32_t c = m_ColorGradient.getColor32( fx*x );
      int32_t r = dbRGBA_R( c );
      int32_t g = dbRGBA_G( c );
      int32_t b = dbRGBA_B( c );
      int32_t a = dbRGBA_A( c );
      QColor color( r,g,b,a );
      for ( int32_t y = 0; y < img.height(); ++y )
      {
         img.setPixelColor( x, y, color );
      }
   }

   m_levelL->setImage( img );
   m_levelR->setImage( img );

   connect( m_btnMinified, SIGNAL(toggled(bool)), this, SLOT(setMinified(bool)) );
   connect( m_btnBypassed, SIGNAL(toggled(bool)), this, SLOT(setBypassed(bool)) );
   connect( m_btnExtraMore, SIGNAL(toggled(bool)), this, SLOT(setExtraMore(bool)) );

   setBypassed( true );
   setExtraMore( true );
   setMinified( true );

   //startUpdateTimer();
}

GDeck::~GDeck()
{
   //stopUpdateTimer();
}

//void GDeck::stopUpdateTimer()
//{
//   if ( m_updateTimerId )
//   {
//      killTimer( m_updateTimerId );
//      m_updateTimerId = 0;
//   }
//}

//void GDeck::startUpdateTimer()
//{
//   stopUpdateTimer();
//   m_updateTimerId = startTimer( 101, Qt::CoarseTimer );
//}

//void
//GDeck::timerEvent( QTimerEvent* event )
//{
//   if ( event->timerId() == m_updateTimerId )
//   {
//      update();
//   }
//}

void GDeck::setBackgroundColor( QColor bgColor )
{
   m_deckColor = bgColor;
   update();
}
void GDeck::setBody(QWidget* body)
{
   m_body = body;
   if ( m_body )
   {
      m_body->setVisible( !isMinified() );
   }
}
void GDeck::setMinified(bool minified)
{
   //dbDebug("GDeck.setMinified(",minified,")")

   if ( minified )
   {
      m_btnMinified->blockSignals( true );
      m_btnMinified->setChecked(minified);
      m_btnMinified->setText("M");
      m_btnMinified->setToolTip("This GDeck is MINIFIED/COLLAPSED");
      m_btnMinified->blockSignals( false );

      m_btnExtraMore->blockSignals( true );
      m_btnExtraMore->setVisible( false );
      m_btnExtraMore->blockSignals( false );
      if ( m_body )
      {
         m_body->blockSignals( true );
         m_body->setVisible( false );
         m_body->blockSignals( false );
      }
   }
   else
   {
      m_btnMinified->blockSignals( true );
      m_btnMinified->setChecked(minified);
      m_btnMinified->setText("F");
      m_btnMinified->setToolTip("This GDeck is FULLY VISIBLE");
      m_btnMinified->blockSignals( false );

      m_btnExtraMore->blockSignals( true );
      m_btnExtraMore->setVisible( true );
      m_btnExtraMore->blockSignals( false );
      if ( m_body )
      {
         m_body->blockSignals( true );
         m_body->setVisible( true );
         m_body->blockSignals( false );
      }
   }
   emit toggledMinimized( minified );
}

void GDeck::setBypassed(bool bypassed)
{
   //dbDebug("GDeck.setBypassed(",bypassed,")")
   if ( bypassed )
   {
      m_btnBypassed->blockSignals( true );
      m_btnBypassed->setChecked( true );
      m_btnBypassed->setText("B");
      m_btnBypassed->setToolTip("This GDeck is DISABLED ( bypassed )");
      m_btnBypassed->blockSignals( false );
   }
   else
   {
      m_btnBypassed->blockSignals( true );
      m_btnBypassed->setChecked( false );
      m_btnBypassed->setText("E");
      m_btnBypassed->setToolTip("This GDeck is ENABLED ( throughpassed processing )");
      m_btnBypassed->blockSignals( false );
   }
   emit toggledBypass( bypassed );

}

void GDeck::setExtraMore(bool visible)
{
   //dbDebug("GDeck.setExtraMore(",visible,")")
   if ( visible )
   {
      m_btnExtraMore->blockSignals( true );
      m_btnExtraMore->setChecked( true );
      m_btnExtraMore->setText("-");
      m_btnExtraMore->setToolTip("Press to show less");
      m_btnExtraMore->blockSignals( false );
   }
   else
   {
      m_btnExtraMore->blockSignals( true );
      m_btnExtraMore->setChecked( false );
      m_btnExtraMore->setText("+");
      m_btnExtraMore->setToolTip("Press to show more");
      m_btnExtraMore->blockSignals( false );
   }
   emit toggledMore( visible );
}

void GDeck::setTitleImageText( QString title )
{
   auto ts = m_font5x8.getTextSize( title );
   QImage img( ts.width()+8, ts.height()+4, QImage::Format_ARGB32 );
   img.fill( QColor(255,255,255,255) );
   m_font5x8.drawText( img, 4,2, title, QColor(0,0,0) );
   m_title->setImage( img, true );
}

void GDeck::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void GDeck::setInputSignal( int i, de::audio::IDspChainElement* input )
{
   m_inputSignal = input;
}

uint64_t
GDeck::readDspSamples( double pts,
                       float* dst,
                       uint32_t frames,
                       uint32_t channels,
                       double rate )
{
   uint64_t dstSamples = frames * channels;
//   ///! Safe clear output buffer ( 100% silence )

   if ( m_inputSignal )
   {
      auto ret = m_inputSignal->readDspSamples( pts, dst,
                                 frames, channels, rate );
      if ( ret != dstSamples )
      {
         DE_ERROR("retSamples != dstSamples, need zero filling")
      }
   }
   else
   {
      de::audio::DSP_FILLZERO( dst, dstSamples );
   }

   if ( isBypassed() )
   {
      return dstSamples;
   }

   m_Lmin = m_Lmax = m_Rmin = m_Rmax = 0.0f;

   // Feed LevelMeter
   m_Lmin = std::numeric_limits< float >::max();
   m_Lmax = std::numeric_limits< float >::lowest();
   m_Rmin = std::numeric_limits< float >::max();
   m_Rmax = std::numeric_limits< float >::lowest();

   // Fast interleaved stereo path O(1) = one loop over samples
   if ( channels == 2 )
   {
      float const* pSrc = dst;
      for ( uint64_t i = 0; i < frames; ++i )
      {
         float L = *pSrc++;
         float R = *pSrc++;
         m_Lmin = std::min( m_Lmin, L );
         m_Lmax = std::max( m_Lmax, R );
         m_Rmin = std::min( m_Rmin, L );
         m_Rmax = std::max( m_Rmax, R );
      }

      m_levelL->setValue( m_Lmax );
      m_levelR->setValue( m_Rmax );

   }
   // Slower path O(N) = one loop for each channel of N channels.
   else
   {
      DE_ERROR("")
//         float const* pSrc = dst;
//         for ( uint64_t i = 0; i < frames; ++i )
//         {
//            float sample = *pSrc;
//            m_Lmin = std::min( m_Lmin, sample );
//            m_Lmax = std::max( m_Lmax, sample );
//            pSrc += channels;
//         }

//         if ( channels > 1 )
//         {
//            pSrc = dst + 1;
//            for ( uint64_t i = 0; i < frames; ++i )
//            {
//               float sample = *pSrc;
//               m_Rmin = std::min( m_Rmin, sample );
//               m_Rmax = std::max( m_Rmax, sample );
//               pSrc += channels;
//            }
//         }
   }

   // Main audio bypass ( oszilloskop shall not change data )
   //de::audio::DSP_COPY( m_inputBuffer.data(), dst, dstSamples );
   return dstSamples;
}

void
GDeck::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();

   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

   // Draw deck background rectangle
   dc.setPen( QPen( QColor( 255,255,255,255 ) ) );
   dc.setBrush( QBrush( m_deckColor ) );
   dc.drawRect( QRect(0,0,w-1,h-1) );

//   // Draw Color Gradient
//   int lvx = w/2;
//   int lvx = w/2;

//   dc.drawImage( w/,0, m_imgColorGradient );

//   dc.drawImage( 0,0, m_imgColorGradient );

//   //
//   int xL = int( m_Lmax * w );
//   int xR = int( m_Rmax * h );
//   int dL = w - 1 - xL;
//   int dR = w - 1 - xR;

//   QColor colorL = toQColor( m_ColorGradient.getColor32( m_Lmax ) );
//   QColor colorR = toQColor( m_ColorGradient.getColor32( m_Rmax ) );
//   QColor colorBg( 25,25,25 );
//   //QColor color = toQColor( 0xFF10BF20 );

//   dc.setPen( Qt::NoPen );
//   dc.setBrush( QBrush( colorBg ) );
//   dc.drawRect( QRect(0,0,w/2-1,yL) );

//   dc.setBrush( QBrush( colorL ) );
//   dc.drawRect( QRect(0,yL,w/2-1,h-1-yL) );


//   dc.setPen( Qt::NoPen );
//   dc.setBrush( QBrush( colorBg ) );
//   dc.drawRect( QRect(w/2,0,w/2-1,yR) );

//   dc.setBrush( QBrush( colorR ) );
//   dc.drawRect( QRect(w/2,yR,w/2-1,h-1-yR) );


   QWidget::paintEvent( event );
}

