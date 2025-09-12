#include "GAudioRecorder.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDebug>
using namespace de::audio;

GAudioRecorder::GAudioRecorder( QWidget* parent )
    : QWidget(parent)
//    , m_updateTimerId( 0 )
    , m_inputSignal( nullptr )
{
   setObjectName( "GAudioRecorder" );
   setContentsMargins(0,0,0,0);
   m_btnRecStart = new QPushButton( "Record", this );
   m_btnRecPause = new QPushButton( "Pause", this );
   m_btnRecStop = new QPushButton( "Stop", this );
   m_btnSaveRaw = new QPushButton( "SaveWAV", this );
   m_btnSaveCompressed = new QPushButton( "SaveMP3", this );
   m_imgWaveform = new QImageWidget( this );
   m_btnRecClear = new QPushButton( "Clear", this );
   //m_inputBuffer.resize( 1024, 0.0f );

   auto h = new QHBoxLayout();
   h->setContentsMargins( 0,0,0,0 );
   h->setSpacing( 0 );
   h->addWidget( m_btnRecStart );
   h->addWidget( m_btnRecPause );
   h->addWidget( m_btnRecStop );
   h->addWidget( m_btnSaveRaw );
   h->addWidget( m_btnSaveCompressed );
   h->addWidget( m_imgWaveform,1 );
   h->addWidget( m_btnRecClear );
   setLayout( h );

   m_ColorGradient.addStop( 0.0f, 0xFFFFFFFF );
   m_ColorGradient.addStop( 0.1f, 0xFF000000 );
   m_ColorGradient.addStop( 0.5f, 0xFF00FF00 );
   m_ColorGradient.addStop( 0.6f, 0xFF002000 );
   m_ColorGradient.addStop( 0.8f, 0xFF00FFFF );
   m_ColorGradient.addStop( 1.0f, 0xFF0000FF );
   m_ColorGradient.addStop( 1.1f, 0xFFFF00FF );

   // Feed LevelMeter
   m_Lmin = m_Lmax = m_Rmin = m_Rmax = 0.0f;

   connect( m_btnRecClear, SIGNAL(clicked(bool)), this, SLOT(on_btnRecClear(bool)), Qt::QueuedConnection );
   connect( m_btnRecStart, SIGNAL(clicked(bool)), this, SLOT(on_btnRecStart(bool)), Qt::QueuedConnection );
   connect( m_btnRecPause, SIGNAL(clicked(bool)), this, SLOT(on_btnRecPause(bool)), Qt::QueuedConnection );
   connect( m_btnRecStop, SIGNAL(clicked(bool)), this, SLOT(on_btnRecStop(bool)), Qt::QueuedConnection );
   connect( m_btnSaveRaw, SIGNAL(clicked(bool)), this, SLOT(on_btnSaveRaw(bool)), Qt::QueuedConnection );
   connect( m_btnSaveCompressed, SIGNAL(clicked(bool)), this, SLOT(on_btnSaveCompressed(bool)), Qt::QueuedConnection );
}

GAudioRecorder::~GAudioRecorder()
{
   //stopUpdateTimer();
}


//void
//GAudioRecorder::startUpdateTimer()
//{
//   stopUpdateTimer();
//   m_updateTimerId = startTimer( 67, Qt::CoarseTimer );
//}

//void
//GAudioRecorder::stopUpdateTimer()
//{
//   if ( m_updateTimerId )
//   {
//      killTimer( m_updateTimerId );
//      m_updateTimerId = 0;
//   }
//}

//void
//GAudioRecorder::timerEvent( QTimerEvent* event )
//{
//   if ( event->timerId() == m_updateTimerId )
//   {
//      update();
//   }
//}


void
GAudioRecorder::on_btnRecClear( bool checked )
{

}
void
GAudioRecorder::on_btnRecStart( bool checked )
{

}
void
GAudioRecorder::on_btnRecPause( bool checked )
{

}
void
GAudioRecorder::on_btnRecStop( bool checked )
{

}
void
GAudioRecorder::on_btnSaveRaw( bool checked )
{
   // QApplication::applicationDirPath()
   QStringList files =
   QFileDialog::getOpenFileNames( this, "Choose WAV,MP3 save fileName", "../../" );
   if ( files.count() < 1 ) { qDebug() << "No files selected"; return; }

   on_btnRecStop(false);

//   std::string uri = files.at( 0 ).toStdString();
//   using namespace de::audio;
//   m_sampleBuffer.clear();
//   m_sampleBuffer.shrink_to_fit();
//   if ( BufferIO::load( m_sampleBuffer, uri, -1, ST_F32I ) )
//   {
//      m_state = Stopped;
//      m_edtUri->setText( QString::fromStdString( m_sampleBuffer.getUri() ) );
//      m_lblDuration->setText( QString::fromStdString( dbStrSeconds( m_sampleBuffer.getDuration() ) ) );
//   }
//   else
//   {
//      m_state = Unloaded;
//      m_edtUri->setText( "LoadError" );
//      m_lblDuration->setText( QString::fromStdString( dbStrSeconds( m_sampleBuffer.getDuration() ) ) );
//   }

//   m_timeSeek->setBuffer( &m_sampleBuffer );
//   m_sampleBuffer.setUri( uri );
}
void
GAudioRecorder::on_btnSaveCompressed(bool checked )
{

}

void
GAudioRecorder::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void
GAudioRecorder::setInputSignal( int i, de::audio::IDspChainElement* input )
{
   m_inputSignal = input;
}

uint64_t
GAudioRecorder::readDspSamples( double pts,
   float* dst, uint32_t dstFrames,
   uint32_t dstChannels, double dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;

   m_Lmin = m_Lmax = m_Rmin = m_Rmax = 0.0f;

   if ( m_inputSignal )
   {
      auto ret = m_inputSignal->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
      if ( ret != dstSamples )
      {
         DE_ERROR("retSamples != dstSamples, need zero filling")
      }

      // Feed LevelMeter
      m_Lmin = std::numeric_limits< float >::max();
      m_Lmax = std::numeric_limits< float >::lowest();
      m_Rmin = std::numeric_limits< float >::max();
      m_Rmax = std::numeric_limits< float >::lowest();

      // Fast interleaved stereo path O(1) = one loop over samples
      if ( dstChannels == 2 )
      {
         float const* pSrc = dst;
         for ( uint64_t i = 0; i < dstFrames; ++i )
         {
            float L = *pSrc++;
            float R = *pSrc++;
            m_Lmin = std::min( m_Lmin, L );
            m_Lmax = std::max( m_Lmax, R );
            m_Rmin = std::min( m_Rmin, L );
            m_Rmax = std::max( m_Rmax, R );
         }
      }
      // Slower path O(N) = one loop for each channel of N channels.
      else
      {
         float const* pSrc = dst;
         for ( uint64_t i = 0; i < dstFrames; ++i )
         {
            float sample = *pSrc;
            m_Lmin = std::min( m_Lmin, sample );
            m_Lmax = std::max( m_Lmax, sample );
            pSrc += dstChannels;
         }

         if ( dstChannels > 1 )
         {
            pSrc = dst + 1;
            for ( uint64_t i = 0; i < dstFrames; ++i )
            {
               float sample = *pSrc;
               m_Rmin = std::min( m_Rmin, sample );
               m_Rmax = std::max( m_Rmax, sample );
               pSrc += dstChannels;
            }
         }
      }
   }

   // Main audio bypass ( oszilloskop shall not change data )
   //de::audio::DSP_COPY( m_inputBuffer.data(), dst, dstSamples );
   return dstSamples;
}

/*
namespace {

QColor toQColor( uint32_t color )
{
   int r = de::RGBA_R( color );
   int g = de::RGBA_G( color );
   int b = de::RGBA_B( color );
   int a = de::RGBA_A( color );
   return QColor( r,g,b,a );
}

} // end namespace

// ============================================================================
void GAudioRecorder::paintEvent( QPaintEvent* event )
// ============================================================================
{
   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   //dc.setBrush( Qt::NoBrush );
   dc.fillRect( rect(), QColor( 10,10,10 ) );

   int w = width();
   int h = height();

   //dc.setCompositionMode( QPainter::CompositionMode_SourceOver );

   //DE_DEBUG("m_Lmax = ", m_Lmax )
//   int yL = int( (1.0f - m_Lmax) * h );
//   int yR = int( (1.0f - m_Rmax) * h );
//   QColor colorL = toQColor( m_ColorGradient.getColor32( m_Lmax ) );
//   QColor colorR = toQColor( m_ColorGradient.getColor32( m_Rmax ) );
//   QColor colorBg( 25,25,25 );
   //QColor color = toQColor( 0xFF10BF20 );

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

//   m_font5x8.drawText( dc, w/2, 1, QString("m_Lmin(%1), m_Lmax(%2), m_Rmin(%3), m_Rmax(%4)")
//      .arg( m_Lmin).arg( m_Lmax ).arg( m_Rmin).arg( m_Rmax ), 0xFFFFFFFF, de::Align::CenterTop );

}
*/
