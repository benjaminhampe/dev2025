#pragma once
#include <de/audio/dsp/IDspChainElement.hpp>
#include <SoundTouch/SoundTouch.h>
#include <SoundTouch/BPMDetect.h>

namespace de {
namespace audio {

// ============================================================================
class SoundTouch : public IDspChainElement
// ============================================================================
{
public:
   SoundTouch();
   ~SoundTouch();

   void
   setupDspElement(
      uint32_t frames, uint32_t channels, double rate ) override;

   uint64_t
   readDspSamples( double pts, float* dst,
      uint32_t frames, uint32_t channels, double rate ) override;

   void
   clearInputSignals() override;
   void
   setInputSignal( int, IDspChainElement* input ) override;
   void
   setInputSampleRate( uint32_t srcRate );

   bool
   isBypassed() const;
   void
   setBypassed( bool bypassed );


   struct RealTimeParams
   {
      bool quickSeek = false;    // Seems unstable ( produces too many raptors )
      float virtualRate = 1.0f;  // these param must never be zero
      float virtualTempo = 1.0f; // or raptors will come out of your
      float virtualPitch = 1.0f; // screen and eat you.
      int aaFilterLen = 128;     // tapcount, can be 0 = disabled, default:64, range:{0,128}, experimental
      int sequenceMs = 10;       // range:{0,?}, default:0, speech:40ms, experimental
      int seekWinMs = 10;        // range:{0,?}, default:0, speech:15ms, experimental
      int overlapMs = 8;         // range:{0,?}, default:8, speech: 8ms, experimental
      int volume = 100;         // range:{0,?}, default:8, speech: 8ms, experimental
   };

   RealTimeParams m_realTimeParams;

   bool m_isBypassed;
   IDspChainElement* m_inputSignal;
   uint32_t m_channelCount;
   uint32_t m_srcRate;
   uint32_t m_dstRate;
   uint32_t m_srcFrames;
   uint32_t m_dstFrames;

   uint64_t m_callCounter;
   uint64_t m_putFrames;
   uint64_t m_popFrames;

   std::vector< float > m_inputBuffer;
   std::vector< float > m_outputBuffer;

   soundtouch::SoundTouch m_soundTouch;
};

} // end namespace audio
} // end namespace de

/*

uint32_t
receiveSamples( soundtouch::SoundTouch & m_soundTouch, float* dst, float dstFrames )
{
      // Now the input file is processed, yet 'flush' few last samples that are
   // hiding in the SoundTouch's internal processing pipeline.
   m_SoundTouch.flush();
   do
   {
      recvFrames = m_SoundTouch.receiveSamples( reinterpret_cast< float* >( chunk.data() ), chunk.getFrameCount() );
      m_FrameIndex += recvFrames;
      if (recvFrames < in.getFrameCount() )
      {
         dbDebug("After Flush: recvFrames(",recvFrames,") < in.getFrameCount(",chunk.getFrameCount(),")")
         chunk.m_FrameCount = recvFrames;
      }
      liste.append( chunk );
   }
   while (recvFrames != 0);
}


// Processes the sound
static void
resample( SampleBuffer const & in, SampleBuffer & out, bool speechConfig = false )
{
   if ( in.getSampleType() != ST_F32I )
   {
      DE_ERROR("No ST_F32I in")
      //return;
   }

   if ( out.getSampleType() != ST_F32I )
   {
      DE_ERROR("No ST_F32I out")
      //return;
   }

   float rateA = in.getSampleRate();
   float rateB = out.getSampleRate();
   float newRate = rateB / rateA;

   soundtouch::SoundTouch m_SoundTouch;
   m_SoundTouch.setSampleRate( in.getSampleRate() );
   m_SoundTouch.setChannels( out.getChannelCount() );
   m_SoundTouch.setRate( newRate );
   //m_SoundTouch.setTempoChange(params->tempoDelta);
   //m_SoundTouch.setPitchSemiTones(params->pitchDelta);
   m_SoundTouch.setSetting(SETTING_USE_QUICKSEEK, true );
   m_SoundTouch.setSetting(SETTING_USE_AA_FILTER, true );

   if (speechConfig)
   {
      // use settings for speech processing
      m_SoundTouch.setSetting(SETTING_SEQUENCE_MS, 40);
      m_SoundTouch.setSetting(SETTING_SEEKWINDOW_MS, 15);
      m_SoundTouch.setSetting(SETTING_OVERLAP_MS, 8);
      fprintf(stderr, "Tune processing parameters for speech processing.\n");
   }

   if ( in.getSampleType() != ST_F32I )
   {
      DE_DEBUG("Convert to ST_F32I")
      SampleBuffer tmp;
      in.convert( tmp, ST_F32I );
      DE_DEBUG("I. putSamples(",(void*)tmp.data(),"), frameCount(", tmp.getFrameCount(), ")")
      DE_FLUSH
      m_SoundTouch.putSamples( reinterpret_cast< float const* >( tmp.data() ), tmp.getFrameCount() );
   }
   else
   {
      DE_DEBUG("II. putSamples(",(void*)in.data(),"), frameCount(", in.getFrameCount(), ")")
      DE_FLUSH
      m_SoundTouch.putSamples( reinterpret_cast< float const* >( in.data() ), in.getFrameCount() );
   }

   //in.getSampleType(), in.getChannelCount(), in.getSampleRate(),
   //out.resize( uint64_t( double( in.getFrameCount() ) * newRate ) + 1 );

   DE_DEBUG("Resized to ",out.getFrameCount()," frames")
   uint64_t m_FrameIndex = 0;

   SampleBufferList liste;
   // Read ready samples from SoundTouch processor & write them output file.
   // NOTES:
   // - 'receiveSamples' doesn't necessarily return any samples at all
   //   during some rounds!
   // - On the other hand, during some round 'receiveSamples' may have more
   //   ready samples than would fit into 'sampleBuffer', and for this reason
   //   the 'receiveSamples' call is iterated for as many times as it
   //   outputs samples.
   SampleBuffer chunk( ST_F32I, out.getChannelCount(), in.getSampleRate(), 1024 );

   //outFile->write(sampleBuffer, nSamples * nChannels); TODO: Are samples = frames?

   int recvFrames = 0;
   do
   {
      recvFrames = m_SoundTouch.receiveSamples( reinterpret_cast< float* >( chunk.data() ), chunk.getFrameCount() );
      m_FrameIndex += recvFrames;
      if (recvFrames < in.getFrameCount() )
      {
         DE_DEBUG("Before Flush: recvFrames(",recvFrames,") < in.getFrameCount(",chunk.getFrameCount(),")")
         chunk.m_FrameCount = recvFrames;
      }
      liste.append( chunk );
   }
   while (recvFrames != 0);

   // Now the input file is processed, yet 'flush' few last samples that are
   // hiding in the SoundTouch's internal processing pipeline.
   m_SoundTouch.flush();
   do
   {
      recvFrames = m_SoundTouch.receiveSamples( reinterpret_cast< float* >( chunk.data() ), chunk.getFrameCount() );
      m_FrameIndex += recvFrames;
      if (recvFrames < in.getFrameCount() )
      {
         DE_DEBUG("After Flush: recvFrames(",recvFrames,") < in.getFrameCount(",chunk.getFrameCount(),")")
         chunk.m_FrameCount = recvFrames;
      }
      liste.append( chunk );
   }
   while (recvFrames != 0);

   if ( !liste.join( out ) )
   {
      DE_ERROR("No join")
   }
}
*/
