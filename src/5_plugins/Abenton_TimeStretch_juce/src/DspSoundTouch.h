#pragma once
#include "DspUtil.h"
#include <SoundTouch_241/SoundTouch.h>
#include <SoundTouch_241/BPMDetect.h>

namespace de {
namespace audio {

// ============================================================================
class DspSoundTouch
// ============================================================================
{
public:
    DspSoundTouch();
    ~DspSoundTouch();

    void
    setup( uint32_t frames, uint32_t channels, uint32_t rate );

    void
    process( const float* __restrict__ src, float* __restrict__ pL, float* __restrict__ pR );

    void
    setInputSampleRate( uint32_t srcRate );

    // void
    // clearInputSignals();
    // void
    // setInputSignal( int, IDspChainElement* input ) override;

    bool
    isBypassed() const;
    void
    setBypassed( bool bypassed );

    struct RealTimeParams
    {
        bool quickSeek = false;    // Seems unstable ( produces too many raptors )
        float virtualRate = 1.0f;  // these param must never be zero
        float virtualTempo = 1.0f; // or raptors will come out of your screen and eat you.
        float virtualPitch = 0.5f; // 2^(semitones/12) -> -12 semitones = 2^-1 = 0.5f
        int aaFilterLen = 128;     // tapcount, can be 0 = disabled, default:64, range:{0,128}, experimental
        int sequenceMs = 10;       // range:{0,?}, default:0, speech:40ms, experimental
        int seekWinMs = 10;        // range:{0,?}, default:0, speech:15ms, experimental
        int overlapMs = 8;         // range:{0,?}, default:8, speech: 8ms, experimental
        int volume = 100;         // range:{0,?}, default:8, speech: 8ms, experimental
    };

    RealTimeParams m_realTimeParams;

    bool m_isBypassed;
    //IDspChainElement* m_inputSignal;
    uint32_t m_channelCount;
    uint32_t m_blockSize;
    AudioFifo m_fifo;
    // uint32_t m_srcRate;
    // uint32_t m_dstRate;
    // uint32_t m_srcFrames;
    // uint32_t m_dstFrames;

    // uint64_t m_callCounter;
    // uint64_t m_putFrames;
    // uint64_t m_popFrames;

    // std::vector< float > m_inputBuffer;
    // std::vector< float > m_outputBuffer;

    TAlignedVector< float > m_popBuffer;

    soundtouch::SoundTouch m_soundTouch;



protected:
    void updateParams();
};

} // end namespace audio
} // end namespace de





/*
4. sequenceMs

Default: 40 ms
Valid range: 10–100 ms

Meaning:
Length of each processing “sequence” block before overlap‑add.

    Short (10–20 ms):

        Better transient preservation
        – More CPU
        – More artifacts at extreme stretch ratios

    Long (60–100 ms):

        Smoother for steady‑state signals
        – Smears transients
        – More “phasiness” on drums

Typical: 40 ms (SoundTouch default)
5. seekWindowMs

Default: 30 ms
Valid range: 10–100 ms

Meaning:
Search window for finding the best overlap point (cross‑correlation).

    Small (10–20 ms):

        Lower CPU
        – Worse alignment → more flanging

    Large (40–100 ms):

        Better alignment
        – Higher CPU
        – Can cause “pumping” if too large

Rule of thumb:
seekWindowMs should be ≤ sequenceMs.
6. overlapMs

Default: 8 ms
Valid range: 5–50 ms

Meaning:
Length of the crossfade between sequences.

    Small (5–10 ms):

        Crisp transients
        – More clicks if alignment is bad

    Large (20–50 ms):

        Smooth transitions
        – Smearing / loss of attack

Typical: 8–12 ms
🎚️ Anti‑Alias Filter
7. aaFilterLen

This is the FIR low‑pass filter length used when rate changes (not tempo/pitch).

Default: depends on SoundTouch build
Typical: 32–128 taps

Meaning:
Controls the steepness of the anti‑alias filter when resampling.

    Short (16–32 taps):

        Low CPU
        – More aliasing at high rate changes

    Medium (48–64 taps):

        Good compromise
        – Moderate CPU

    Long (96–128 taps):

        Very clean
        – High CPU
        – More latency

Important:
aaFilterLen only matters when virtualRate ≠ 1.0.
🔧 Recommended Practical Ranges (Real‑World)
Parameter       Safe Range	Notes
virtualRate     0.5–2.0     Affects pitch + tempo
virtualTempo	0.5–2.0     Time‑stretch only
virtualPitch	0.5–2.0     Pitch‑shift only
sequenceMs      30–60       40 is standard
seekWindowMs	20–40       Must be ≤ sequenceMs
overlapMs       6–12        8 is standard
aaFilterLen     32–64       48 is a good default


🔧 Preset Table
Preset          sequenceMs	seekWindowMs	overlapMs	aaFilterLen
Speech          30          20              8           48
Drums           20          15              6           32
Full Mix        40          30              8–10        48–64
Extreme Stretch	60–80       40–60           12–20       64–96
Low Latency     15–20       10–15           5–6         16–32


🎛️ SoundTouch Recommended Presets (All Values in ms)
🗣️ Speech Preset

Optimized for intelligibility, minimal smearing, stable formants.
Param	Value
sequenceMs	30 ms
seekWindowMs	20 ms
overlapMs	8 ms
aaFilterLen	48

Why:
Short sequences preserve consonants; moderate overlap avoids clicks; seek window small enough to avoid “robotic” artifacts.
🥁 Drums / Percussion Preset

Optimized for transients, minimal smearing, minimal flanging.
Param	Value
sequenceMs	20 ms
seekWindowMs	15 ms
overlapMs	6 ms
aaFilterLen	32

Why:
Short blocks + short overlap = crisp attacks.
Small seek window reduces phase‑wandering on kicks/snares.
🎧 Full Mix / Music Preset

Balanced for vocals + instruments, smooth but not smeared.
Param	Value
sequenceMs	40 ms
seekWindowMs	30 ms
overlapMs	8–10 ms
aaFilterLen	48–64

Why:
40/30/8 is the classic SoundTouch default for a reason:
Good compromise between transient clarity and smoothness.
🌀 Extreme Stretch Preset

For slow‑downs > 2×, ambient effects, time‑warp.
Param	Value
sequenceMs	60–80 ms
seekWindowMs	40–60 ms
overlapMs	12–20 ms
aaFilterLen	64–96

Why:
Long sequences reduce “granular” artifacts.
Large overlap smooths transitions.
Large seek window improves correlation at extreme ratios.
⚡ Low‑Latency Preset

For real‑time processing (live input, monitoring).
Param	Value
sequenceMs	15–20 ms
seekWindowMs	10–15 ms
overlapMs	5–6 ms
aaFilterLen	16–32

Why:
Short blocks = low latency.
Small overlap = minimal buffering.
Lower filter length reduces FIR delay.
🔧 Summary Table (All Presets)
Preset          sequenceMs	seekWindowMs	overlapMs	aaFilterLen
Speech          30          20              8           48
Drums           20          15              6           32
Full Mix        40          30              8–10        48–64
Extreme Stretch	60–80       40–60           12–20       64–96
Low Latency     15–20       10–15           5–6         16–32

*/









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
