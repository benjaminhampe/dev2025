#include <de/audio/dsp/DspSoundTouch.h>

namespace de {
namespace audio {

/*
namespace {

int powi( int base, int exponent )
{
   int k = base;
   for ( int i = 1; i < exponent; ++i )
   {
      k *= base;
   }
   return k;
}

} // end namespace
*/

// ============================================================================
SoundTouch::SoundTouch()
   : m_isBypassed( false )
   , m_inputSignal( nullptr )
   , m_channelCount( 2 )
   , m_srcRate( 44100 )
   , m_dstRate( 48000 )
   , m_srcFrames( 1024 )
   , m_dstFrames( 1024 )
   , m_callCounter( 0 )
   , m_putFrames( 0 )
   , m_popFrames( 0 )
{
   dbDebug("DEFAULT_AA_FILTER_LENGTH = ", m_soundTouch.getSetting(SETTING_AA_FILTER_LENGTH ) )
   dbDebug("DEFAULT_SEQUENCE_MS = ", m_soundTouch.getSetting(SETTING_SEQUENCE_MS ) )
   dbDebug("DEFAULT_SEEKWINDOW_MS = ", m_soundTouch.getSetting(SETTING_SEEKWINDOW_MS ) )
   dbDebug("DEFAULT_OVERLAP_MS = ", m_soundTouch.getSetting(SETTING_OVERLAP_MS ) )

   m_soundTouch.setSampleRate( 44100 );
   m_soundTouch.setChannels( 2 );
   m_soundTouch.setRate( 1.0f );
   m_soundTouch.setSetting(SETTING_USE_QUICKSEEK, 0 );
   m_soundTouch.setSetting(SETTING_USE_AA_FILTER, 1 );
   m_soundTouch.setSetting(SETTING_AA_FILTER_LENGTH,128 );
   m_soundTouch.setSetting(SETTING_SEQUENCE_MS,28 );
   m_soundTouch.setSetting(SETTING_SEEKWINDOW_MS, 28);
   m_soundTouch.setSetting(SETTING_OVERLAP_MS,80 );

   bool speechConfig = false;
   if (speechConfig)
   {
      // use settings for speech processing
      m_soundTouch.setSetting(SETTING_SEQUENCE_MS, 40);
      m_soundTouch.setSetting(SETTING_SEEKWINDOW_MS, 15);
      m_soundTouch.setSetting(SETTING_OVERLAP_MS, 8);
      dbDebug("Tune processing parameters for speech processing.")
   }


   dbDebug("Final Setting:" )
   dbDebug("SETTING_AA_FILTER_LENGTH = ", m_soundTouch.getSetting(SETTING_AA_FILTER_LENGTH ) )
   dbDebug("SETTING_SEQUENCE_MS = ", m_soundTouch.getSetting(SETTING_SEQUENCE_MS ) )
   dbDebug("SETTING_SEEKWINDOW_MS = ", m_soundTouch.getSetting(SETTING_SEEKWINDOW_MS ) )
   dbDebug("SETTING_OVERLAP_MS = ", m_soundTouch.getSetting(SETTING_OVERLAP_MS ) )

   // Read in Params first time.
   m_realTimeParams.quickSeek = (m_soundTouch.getSetting(SETTING_USE_QUICKSEEK ) != 0);
   m_realTimeParams.sequenceMs = m_soundTouch.getSetting(SETTING_SEQUENCE_MS);
   m_realTimeParams.seekWinMs = m_soundTouch.getSetting(SETTING_SEEKWINDOW_MS);
   m_realTimeParams.overlapMs = m_soundTouch.getSetting(SETTING_OVERLAP_MS);
   m_realTimeParams.aaFilterLen = m_soundTouch.getSetting(SETTING_AA_FILTER_LENGTH);
   if ( m_soundTouch.getSetting(SETTING_USE_AA_FILTER) == 0 )
   {
      m_realTimeParams.aaFilterLen = 0;
   }


   m_inputBuffer.resize( 8192, 0.0f );
   m_outputBuffer.resize( 8192, 0.0f );
}

SoundTouch::~SoundTouch()
{
}

void
SoundTouch::clearInputSignals()
{
   m_inputSignal = nullptr;
}
void
SoundTouch::setInputSignal( int, IDspChainElement* input )
{
   m_inputSignal = input;
}
void
SoundTouch::setInputSampleRate( uint32_t srcRate )
{
   m_srcRate = srcRate;
   m_soundTouch.setSampleRate( m_srcRate );
}
void
SoundTouch::setBypassed( bool bypassed )
{
   dbDebug("bypassed = ", bypassed)
   m_isBypassed = bypassed;
}
bool
SoundTouch::isBypassed() const
{
   return m_isBypassed;
}

void
SoundTouch::setupDspElement(
      uint32_t dstFrames, uint32_t dstChannels, double dstRate )
{
}

uint64_t
SoundTouch::readDspSamples(
   double pts, float* dst, uint32_t dstFrames,
            uint32_t dstChannels, double dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;
#if USE_BROKEN

//   DSP_RESIZE( m_inputBuffer, dstSamples );
//   DSP_FILLZERO( m_inputBuffer.data(), dstSamples );

   DSP_RESIZE( m_outputBuffer, dstSamples );
   DSP_FILLZERO( m_outputBuffer );

   if ( m_inputSignal )
   {
      m_inputSignal->readDspSamples( pts,
                        m_outputBuffer.data(),
                        dstFrames,
                        dstChannels,
                        dstRate ); // This is the bug. Should not be called here!
   }

   if ( m_isBypassed )
   {
      //DE_WARN("bypassed ",objectName().toStdString())
      //m_soundTouch.flush();
      m_soundTouch.clear();
      m_callCounter = 0;
      m_putFrames = 0;
      m_popFrames = 0;

      DSP_COPY( m_outputBuffer.data(), dst, dstSamples );
      return dstSamples;
   }
#else
   if ( !m_inputSignal )
   {
      // dbDebug("No inputSignal")
      DSP_FILLZERO( dst, dstSamples );
      return dstSamples;
   }

   if ( isBypassed() )
   {
      //DE_WARN("bypassed ",objectName().toStdString())
      //m_soundTouch.flush();
      m_soundTouch.clear();
      m_callCounter = 0;
      m_putFrames = 0;
      m_popFrames = 0;
      m_inputSignal->readDspSamples( pts, dst, dstFrames,
                                       dstChannels, dstRate );
      return dstSamples;
   }
#endif

   m_channelCount = dstChannels;
   m_dstRate = dstRate;

//=====================
// Update SoundTouch
//=====================
   m_soundTouch.setRate( m_realTimeParams.virtualRate );
   m_soundTouch.setTempo( m_realTimeParams.virtualTempo );
   m_soundTouch.setPitch( m_realTimeParams.virtualPitch );

   if ( m_realTimeParams.aaFilterLen > 0 )
   {
      auto enabled = (m_soundTouch.getSetting( SETTING_USE_AA_FILTER ) != 0);
      if ( !enabled )
      {
         m_soundTouch.setSetting( SETTING_USE_AA_FILTER, 1 );
      }

      auto old = m_soundTouch.getSetting( SETTING_AA_FILTER_LENGTH );
      if ( old != m_realTimeParams.aaFilterLen )
      {
         m_soundTouch.setSetting( SETTING_AA_FILTER_LENGTH, m_realTimeParams.aaFilterLen );
      }
   }
   else
   {
      auto enabled = (m_soundTouch.getSetting( SETTING_USE_AA_FILTER ) != 0);
      if ( enabled )
      {
         m_soundTouch.setSetting( SETTING_USE_AA_FILTER, 0 );
      }
   }

   auto sequenceMs = m_soundTouch.getSetting( SETTING_SEQUENCE_MS );
   if ( sequenceMs != m_realTimeParams.sequenceMs )
   {
      m_soundTouch.setSetting( SETTING_SEQUENCE_MS, m_realTimeParams.sequenceMs );
   }

   auto seekWinMs = m_soundTouch.getSetting( SETTING_SEEKWINDOW_MS );
   if ( seekWinMs != m_realTimeParams.seekWinMs )
   {
      m_soundTouch.setSetting( SETTING_SEEKWINDOW_MS, m_realTimeParams.seekWinMs );
   }

   auto overlapMs = m_soundTouch.getSetting( SETTING_OVERLAP_MS );
   if ( overlapMs != m_realTimeParams.overlapMs )
   {
      m_soundTouch.setSetting( SETTING_OVERLAP_MS, m_realTimeParams.overlapMs );
   }

//=====================
// Prepare buffers:
//=====================
   uint64_t srcFrames = 64;
   DSP_RESIZE( m_inputBuffer, srcFrames * dstChannels );
#if USE_BROKEN
#else
   DSP_RESIZE( m_outputBuffer, dstSamples );
   DSP_FILLZERO( m_outputBuffer );
#endif

//=====================
// Request 'dstFrames' frames from soundtouch to satisfy caller.
// feed more samples to soundtouch when we dont receive anything/enough, until done.
//=====================

   uint64_t loopCounter = 0;
   uint64_t dstCounter = dstFrames;

   auto pDst = dst;

   while ( dstCounter > 0 )
   {
      // [Receive]
      uint64_t popFrames = m_soundTouch.receiveSamples(
                              m_outputBuffer.data(), dstCounter );
      m_popFrames += popFrames;

      // [Received something]
      if ( popFrames > 0 )
      {
         // [Received too many, information loss since we dont store it]
         if ( popFrames > dstCounter )
         {
             //dbDebug("popFrames(",popFrames,") > dstCounter(",dstCounter,")")
//            DSP_COPY( m_outputBuffer.data(), pDst, dstCounter * dstChannels );
//            pDst += dstCounter * dstChannels;
         }

         // [Write as much as we can]
         uint64_t popSamples = std::min( popFrames, uint64_t(dstFrames) ) * dstChannels;
         DSP_COPY( m_outputBuffer.data(), pDst, popSamples );
         pDst += popSamples;
      }
      // [Received not enough, ergo we need to feed the dragon]
      else if ( popFrames < dstCounter )
      {
         // Read input, dont care if too few input samples, we cleared the buffer.
         DSP_FILLZERO( m_inputBuffer );
         // Read input...
         uint64_t gotSamples = m_inputSignal->readDspSamples( pts,
                     m_inputBuffer.data(), srcFrames, dstChannels, m_srcRate );
         uint64_t gotFrames = gotSamples / dstChannels;
         if ( gotFrames != srcFrames )
         {
            dbTrace("gotFrames(",gotFrames,") != srcFrames(",srcFrames,")")
         }

         m_soundTouch.putSamples( m_inputBuffer.data(), gotFrames );
         m_putFrames += gotFrames;
      }

      dstCounter -= popFrames;

#if 0
      DE_WARN("call(",m_callCounter,"), "
               "loop(",loopCounter,"), "
               "m_putFrames(",m_putFrames,"), "
               "m_popFrames(",m_popFrames,"), "
               "popFrames(",popFrames,"), "
               "dstCounter(",dstCounter,")")
#endif
      loopCounter++;
   }

   m_callCounter++;
   //dbDebug("pts(",pts,"), channels(",m_channelCount,"), loopCounter(",loopCounter,")")
//   dbDebug("dstRate(",m_dstRate," Hz), dstFrames(",dstFrames,")")
//   dbDebug("srcRate(",m_srcRate," Hz), srcFrames(",1024,")")
//   dbDebug("m_virtualRate", m_virtualRate)
//   dbDebug("m_rateChange",m_rateChange)
//   dbDebug("rateChange",rateChange)

   float volume = 0.0001f * float( m_realTimeParams.volume * m_realTimeParams.volume );
   DSP_MUL( dst, dstSamples, volume );

   return dstSamples;

}

} // end namespace audio
} // end namespace de
