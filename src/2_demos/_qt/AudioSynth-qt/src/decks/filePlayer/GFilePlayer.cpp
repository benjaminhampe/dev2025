#include "GFilePlayer.hpp"

#include <de/audio/GetCoverArt.hpp>
#include <de/audio/buffer/BufferIO.hpp>
#include <QString>


namespace de {
namespace audio {

BufferPlayer::BufferPlayer()
   : m_sampleBuffer( nullptr )
   , m_sampleRate( 0 )
   , m_frameCount( 0 )
   , m_frameIndex( 0 )
   , m_isLooping( false )
   , m_loopIndex( 0 )
   , m_loopCount( 0 )
{
}

BufferPlayer::~BufferPlayer()
{

}

uint64_t
BufferPlayer::readDspSamples(
   double pts,
   float* dst,
   uint32_t frames,
   uint32_t channels,
   double rate )
{
// using namespace de::audio;

//   DE_DEBUG( "m_frameIndex = ",m_frameIndex,", "
//            "m_frameCount = ",m_frameCount,", "
//            "frames = ",frames,", "
//            "channels = ",channels,", "
//            "rate = ",rate,".")

   size_t dstSamples = size_t( frames ) * channels;
   DSP_FILLZERO( dst, dstSamples );

   if ( !m_sampleBuffer )
   {
      return dstSamples;
   }

   if ( m_sampleBuffer->getSampleType() != de::audio::ST_F32I )
   {
      DE_ERROR("No ST_F32I")
      return dstSamples;
   }

//   size_t k = m_sampleBuffer.read(
//                  m_frameIndex, frames, channels, dst, ST_F32I );

   auto & fi = m_frameIndex;
   auto fc = m_sampleBuffer->getFrameCount();

   if ( fi + frames >= fc )
   {
      DE_DEBUG("AboutToStop fi(",fi,") + n(",frames,") >= fc(",fc,")" )
      //m_state = Stopped;
      m_frameIndex = 0; // rewind dirty hack
   }

   size_t k = m_sampleBuffer->readF32I( dst, frames, channels, m_frameIndex );
   //DE_DEBUG( "m_frameIndex = ",m_frameIndex,", k = ",k )
   m_frameIndex += k;

/*
   if ( m_frameIndex >= m_sampleBuffer->getFrameCount() )
   {
      //DE_DEBUG( "m_FrameIndex(",m_FrameIndex,") + n(",n,") >= m(",m,")" )
      //m_state = Stopped;
      m_frameIndex = 0; // rewind dirty hack

   }
*/
   return dstSamples;
}

} // end namespace audio.
} // end namespace de.



GFilePlayer::GFilePlayer( bool bRight, QWidget* parent )
   : QWidget(parent)
   , m_soundTouch( nullptr )
   , m_state( Unloaded )
   //, m_frameIndex( 0 )
   //, m_isLooping( false )
   //, m_loopIndex( 0 )
   //, m_loopCount( 0 )
   , m_updateTimerID( 0 )
   , m_sampleBuffer( de::audio::ST_F32I, 2, 44100 )
{
   m_bufferPlayer.setSampleBuffer( &m_sampleBuffer );

   setObjectName( "GFilePlayer" );

   m_fileInfo = new QWidget( this );
   m_edtUri = new QLineEdit( "...", this );
   m_edtState = new QLineEdit( getStateStr(), this );
   m_btnLoad = new QPushButton( "+", this );
   m_btnPlay = new QPushButton( "Play", this );
   m_btnStop = new QPushButton( "Stop", this );
   m_lblPosition = new QLabel( "-:-", this );
   m_sldPosition = new QSlider( Qt::Horizontal, this );
   m_sldPosition->setMinimum( 0 );
   m_sldPosition->setMaximum( 1255 );
   m_sldPosition->setValue( 0 );
   m_lblDuration = new QLabel( "-:-", this );

   m_coverArt = new QImageWidget( this );
   m_coverArt->setMinimumSize(200,200);
   m_coverArt->setMaximumSize(200,200);
   m_coverArt->setVisible( false );

   m_timeSeek = new GTimeSeek( this );
   m_timeSeek->setBuffer( &m_sampleBuffer );

   m_lpFilter = new GLowPass3( this );
   m_soundTouch = new GSoundTouch( this );
   m_levelMeter = new GLevelMeter( this );

   auto h1 = new QHBoxLayout();
   h1->setContentsMargins(0,0,0,0);
   h1->setSpacing( 5 );
   h1->addWidget( m_btnLoad );
   h1->addWidget( m_edtUri );
   h1->addWidget( m_edtState );

   auto h2 = new QHBoxLayout();
   h2->setContentsMargins(0,0,0,0);
   h2->setSpacing( 5 );
   h2->addWidget( m_btnPlay );
   h2->addWidget( m_btnStop );

   auto h3 = new QHBoxLayout();
   h3->setContentsMargins(0,0,0,0);
   h3->setSpacing( 5 );
   h3->addWidget( m_lblPosition );
   h3->addWidget( m_sldPosition );
   h3->addWidget( m_lblDuration );

   auto v0 = new QVBoxLayout();
   v0->setContentsMargins(0,0,0,0);
   v0->setSpacing( 5 );
   v0->addLayout( h1 );
   v0->addLayout( h2 );
   v0->addLayout( h3 );

   auto h4 = new QHBoxLayout();
   h4->setContentsMargins(0,0,0,0);
   h4->setSpacing( 5 );
   if ( bRight )
   {
      h4->addLayout( v0 );
      h4->addWidget( m_coverArt );
   }
   else
   {
      h4->addWidget( m_coverArt );
      h4->addLayout( v0 );
   }

   m_fileInfo->setLayout( h4 );


   auto v = new QVBoxLayout();
   v->setContentsMargins(0,0,0,0);
   v->setSpacing( 5 );
   v->addWidget( m_fileInfo );
   v->addWidget( m_timeSeek );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 5 );
   h->addLayout( v );
   h->addWidget( m_lpFilter );
   h->addWidget( m_soundTouch );
   h->addWidget( m_levelMeter );

   setLayout( h );

   // m_soundtouch->setInputSignal( m_mixer );

   m_lpFilter->dsp()->setInputSignal( 0, &m_bufferPlayer );
   m_soundTouch->dsp()->setInputSignal( 0, m_lpFilter->dsp() );
   m_levelMeter->dsp()->setInputSignal( 0, m_soundTouch->dsp() );
   //m_levelMeter->dsp()->setInputSignal( 0, m_lpFilter->dsp() );

   m_lpFilter->setBypassed( true );
   m_lpFilter->setMinified( true );

   m_soundTouch->setBypassed( true );
   m_soundTouch->setMinified( true );

   connect( m_btnLoad, SIGNAL(clicked(bool)),
            this, SLOT(on_clicked_load(bool)) );
   connect( m_btnPlay, SIGNAL(clicked(bool)),
            this, SLOT(on_clicked_play(bool)) );
   connect( m_btnStop, SIGNAL(clicked(bool)),
            this, SLOT(on_clicked_stop(bool)) );
   connect( m_sldPosition, SIGNAL(valueChanged(int)),
            this, SLOT(on_slider_pos(int)) ); //, Qt::QueuedConnection
   connect( m_timeSeek, SIGNAL(positionChanged(float)),
            this, SLOT(on_time_seek(float)) ); //, Qt::QueuedConnection

}

GFilePlayer::~GFilePlayer()
{

}

void
GFilePlayer::updateSlider()
{
}

void
GFilePlayer::updateLabels()
{
   typedef double T;
   int v = m_sldPosition->value();
   int m = m_sldPosition->minimum();
   int n = m_sldPosition->maximum();
   T t = T( v - m ) / T( n - m );

   auto fi = m_bufferPlayer.getFrameIndex();
   auto fc = m_bufferPlayer.getFrameCount();
   auto sr = m_bufferPlayer.getSampleRate();

   T pos = T(0);
   T dur = T(0);

   if ( int(sr) > 0 )
   {
      pos = T(fi) / T(sr);
      dur = T(fc) / T(sr);

   }

   m_lblPosition->blockSignals( true );
   m_lblPosition->setText( QString::fromStdString( dbStrSeconds( pos ) ) );
   m_lblPosition->blockSignals( false );

   m_lblDuration->blockSignals( true );
   m_lblDuration->setText( QString::fromStdString( dbStrSeconds( dur ) ) );
   m_lblDuration->blockSignals( false );

   m_edtState->blockSignals( true );
   m_edtState->setText( getStateStr() );
   m_edtState->blockSignals( false );
}

void
GFilePlayer::startUpdateTimer()
{
   if (m_updateTimerID != 0) return;
   m_updateTimerID = startTimer( 80 );
}

void
GFilePlayer::stopUpdateTimer()
{
   if (m_updateTimerID == 0) return;
   killTimer( m_updateTimerID );
   m_updateTimerID = 0;
}

void
GFilePlayer::timerEvent( QTimerEvent* event )
{
   if ( m_updateTimerID == event->timerId() )
   {
      if ( getState() == Playing )
      {
         updateLabels();
      }
   }

   //QWidget::timerEvent( event );
   event->accept();
}

uint32_t
GFilePlayer::getState() const { return m_state; }

QString
GFilePlayer::getStateStr() const
{
   switch ( m_state )
   {
      case Unloaded: return "Unloaded";
      case Stopped: return "Stopped";
      case Playing: return "Playing";
      case Paused: return "Paused";
      case Looping: return "Looping";
      default: return "Unknown";
   }
}

double
GFilePlayer::getDuration() const
{
   return m_sampleBuffer.getDuration();
}

double
GFilePlayer::getPosition() const
{
   return m_bufferPlayer.getPosition();
}

void
GFilePlayer::on_time_seek( float percent )
{
   typedef double T;
   auto fc = m_sampleBuffer.getFrameCount();
   auto fi = uint64_t( T(percent / 100.0f) * T(fc) );
   m_bufferPlayer.setFrameIndex( fi );

   updateLabels();
}

void
GFilePlayer::on_slider_pos( int value )
{
   typedef double T;
   int v = value; // m_sldPosition->value();
   int m = m_sldPosition->minimum();
   int n = m_sldPosition->maximum();
   T t = T( v - m ) / T( n - m );

   auto fi = uint64_t( t * double(m_sampleBuffer.getFrameCount()) );

   m_bufferPlayer.setFrameIndex( fi );

   updateLabels();
}

void
GFilePlayer::on_clicked_load(bool checked)
{
   load();
   updateLabels();
}

void
GFilePlayer::on_clicked_play(bool checked)
{
   play();
   updateLabels();
   //m_edtState->setText( getStateStr() );
}

void
GFilePlayer::on_clicked_stop(bool checked)
{
   stop();
   updateLabels();
   //m_edtState->setText( getStateStr() );
}

void
GFilePlayer::load()
{
   // QApplication::applicationDirPath()
   auto files = QFileDialog::getOpenFileNames( this, "Open audio file", "../../" );
   if ( files.count() < 1 )
   {
      qDebug() << "No files selected";
      return;
   }

   if ( m_state > Stopped )
   {
      stop();
   }

   QString uri = files.at( 0 );
   //auto wri = uri.toStdWString();

   DE_DEBUG(uri.toStdString())

   stop();

   //m_coverArt->setImage( QImage(), true );

   DE_DEBUG(uri.toStdString())
   de::audio::CoverArt coverArt;
   de::audio::CoverArt::getCoverArt( coverArt, uri.toStdWString() );
   m_coverArt->setImage( toQImage( coverArt.img ) );

   m_sampleBuffer.clear();
   m_sampleBuffer.shrink_to_fit();
   if ( de::audio::BufferIO::load( m_sampleBuffer, uri.toStdString(), -1, de::audio::ST_F32I ) )
   {
      m_state = Stopped;
      //m_edtUri->blockSignals( true );
      m_edtUri->setText( uri );
      m_edtUri->setText( QString::fromStdString( m_sampleBuffer.getUri() ) );
      //m_edtUri->blockSignals( false );
      m_lblDuration->setText( QString::fromStdString( dbStrSeconds( m_sampleBuffer.getDuration() ) ) );
   }
   else
   {
      m_state = Unloaded;
      //m_edtUri->blockSignals( true );
      m_edtUri->setText( "LoadError" );
      //m_edtUri->blockSignals( false );
      m_lblDuration->setText( QString::fromStdString( dbStrSeconds( m_sampleBuffer.getDuration() ) ) );
   }

   DE_DEBUG(m_sampleBuffer.toString())

//   DE_DEBUG("Loaded SampleBuffer ", uri )
//   DE_FLUSH

   m_timeSeek->setBuffer( &m_sampleBuffer );
   //m_sampleBuffer.setUri( uri );

   m_soundTouch->dsp()->setInputSampleRate( m_sampleBuffer.getSampleRate() );

   m_bufferPlayer.setSampleBuffer( &m_sampleBuffer );
   m_bufferPlayer.rewind();

   DE_DEBUG("FrameIndex = ", m_bufferPlayer.getFrameIndex() )
   DE_DEBUG("FrameCount = ", m_bufferPlayer.getFrameCount() )
}

void
GFilePlayer::play()
{
   if ( m_state == Unloaded )
   {
      load();
      DE_DEBUG("Loaded")
   }

   if ( m_state == Stopped )
   {
      startUpdateTimer();
      m_state = Playing;
      DE_DEBUG("Playing")
   }
   else if ( m_state == Playing )
   {
      stopUpdateTimer();
      m_state = Paused;
      DE_DEBUG("Paused")
   }
   else if ( m_state == Paused )
   {
      startUpdateTimer();
      m_state = Playing;
      DE_DEBUG("Resume")
   }
}

void
GFilePlayer::stop()
{
   if ( m_state != Stopped )
   {
      startUpdateTimer();
      m_state = Stopped;
      DE_DEBUG("Stopped")
      m_bufferPlayer.rewind();
      m_lblPosition->setText( "00:00" );
      m_timeSeek->setPosition( 0.0f );
   }
}

void
GFilePlayer::pause()
{
   if ( m_state == Playing )
   {
      m_state = Paused;
      DE_DEBUG("Paused")
   }
}

uint64_t
GFilePlayer::readDspSamples(
   double pts,
   float* dst,
   uint32_t frames,
   uint32_t channels,
   double rate )
{
   size_t dstSamples = size_t( frames ) * channels;

   if ( m_state != Playing )
   {
      de::audio::DSP_FILLZERO( dst, dstSamples );
      return dstSamples;
   }

//   if ( m_sampleBuffer.getSampleType() != de::audio::ST_F32I )
//   {
//      DE_ERROR("No ST_F32I")
//      return dstSamples;
//   }

//   size_t k = m_sampleBuffer.read(
//                  m_frameIndex, frames, channels, dst, ST_F32I );


   //m_bufferPlayer.getFrameIndex()
/*
   auto fi = m_bufferPlayer.getFrameIndex();
   auto fc = m_bufferPlayer.getFrameCount();
   if ( fi + frames >= fc )
   {
      DE_DEBUG("fi(",fi,") >= fc(",fc,")" )
      m_state = Stopped;
      //m_frameIndex = 0; // rewind dirty hack
   }
*/
//   m_bufferPlayer.readDspSamples( pts, dst,
//      frames, channels, rate );

   m_levelMeter->dsp()->readDspSamples( pts, dst,
      frames, channels, rate );

//   size_t k = m_sampleBuffer.readF32I( dst, frames, channels, m_frameIndex );
//   //DE_DEBUG( "k = ",k,", dst.getChannelCount() = ",dst.getChannelCount() )
//   m_frameIndex += k;

   //m_bufferPlayer.getFrameIndex()
   auto fi = m_bufferPlayer.getFrameIndex();
   auto fc = m_bufferPlayer.getFrameCount();
   if ( fi >= fc )
   {
      DE_DEBUG("fi(",fi,") >= fc(",fc,")" )
      m_state = Stopped;
      //m_frameIndex = 0; // rewind dirty hack
   }

   return dstSamples;
}
