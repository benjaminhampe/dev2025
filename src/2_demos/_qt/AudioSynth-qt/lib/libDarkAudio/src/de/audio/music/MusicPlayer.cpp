#include <de/audio/music/MusicPlayer.hpp>

namespace de {
namespace audio {

MusicPlayer::MusicPlayer()
   : m_Uri("")
//   , m_IsOpen( false )
   , m_IsPlaying( false )
   , m_IsLooping( false )
   , m_Volume( 80 )
   //, m_Bpm( 90.f )
   , m_StereoMode( 0 )
{
   DE_DEBUG("Constructor")
   m_mixBuffer.resize( 8*1024 );
}

MusicPlayer::~MusicPlayer()
{
   DE_DEBUG("Destructor")
   close();
}

uint64_t
MusicPlayer::readDspSamples( double pts,
                            float* dst,
                            uint32_t dstFrames,
                            uint32_t dstChannels,
                            double dstRate )
{
   size_t dstSamples = dstFrames * dstChannels;
   if ( m_mixBuffer.size() < dstSamples )
   {
      m_mixBuffer.resize( dstSamples );
      DE_WARN("Need resize!")
   }

   DSP_FILLZERO( dst, dstSamples );
   if ( !m_FileStream )
   {
      //DE_WARN("No FileThread!")
      return dstSamples;
   }

   size_t retSamples = m_FileStream->m_BufferQueue.pop_front( m_mixBuffer.data(), pts, dstFrames, dstChannels, dstRate );
   if ( retSamples < dstSamples )
   {
      DE_WARN("EOF retSamples(",retSamples,") < dstSamples(",dstSamples,")")
      m_IsPlaying = false;
      return dstSamples; // not enough data, eof.
   }

   if ( !m_IsPlaying )
   {
      DE_WARN("!m_IsPlaying")
      return dstSamples; // not enough data, eof.
   }

   // Write RTAUDIO_FLOAT32
   float vol = 0.0001f * (m_Volume*m_Volume);
   DSP_COPY_SCALED( m_mixBuffer.data(), dst, dstSamples, vol );
   /*
   for ( size_t i = 0; i < dstFrames; i++ )
   {
      for ( size_t j = 0; j < dstChannels; j++ )
      {
         float sample = 0.0f;
         if ( m_mixBuffer.getSample( i,j, &sample ) )
         {
            *dst = vol * sample;
         }
         else
         {
            DE_WARN("i(",i,"), j(",j,") no sample")
            *dst = 0.f;
         }

         dst++;
      }
   }
   */
   return 0;
}



void
MusicPlayer::setVolume( int vol_in_percent )
{
   vol_in_percent = dbClampi( vol_in_percent, 0, 200 );
   m_Volume = vol_in_percent;
   DE_DEBUG("setVolume(",vol_in_percent,"%)")
}

int
MusicPlayer::getVolume() const { return m_Volume; }

void
MusicPlayer::close()
{
   stop();

   if ( m_FileStream.get() )
   {
      DE_DEBUG("Closing")
      m_FileStream.reset();
   }
}

bool
MusicPlayer::open( std::string uri, int streamIndex )
{
   //close();
   if ( m_FileStream.get() )
   {
      DE_ERROR("Already open")
      return true;
   }

   if ( uri.empty() )
   {
      DE_ERROR("Empty uri")
      return false; // Prevent dead locks.
   }
   //dbStrReplace( uri, "\\", "/" );

   m_FileStream.reset( new MusicFileStream() );

   bool ok = m_FileStream->open( uri, streamIndex );
   if ( !ok )
   {
      DE_ERROR("Cant open uri ",uri )
      m_FileStream.reset();
   }

   return ok;
}

bool
MusicPlayer::is_open() const { return m_FileStream.get() != nullptr; }

bool
MusicPlayer::is_playing() const { return m_IsPlaying; }

void
MusicPlayer::play()
{
   stop();
   m_TimeStart = dbTimeInSeconds();
   m_Time = 0.0;
   m_IsPlaying = true;
   DE_DEBUG("Play")
}

void
MusicPlayer::stop()
{
   if ( m_IsPlaying )
   {
      DE_DEBUG("Stop")
      m_IsPlaying = false;
   }
}


} // end namespace audio
} // end namespace de

