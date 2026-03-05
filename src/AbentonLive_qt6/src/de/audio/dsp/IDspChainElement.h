#pragma once
#include <DarkImage.h>
#include <de/audio/buffer/SampleType.h>
#include <functional>

namespace de {
namespace audio {

/*
        1
       /|\
      / | \
     /  |  ---  sustain
 ___/   |  |   \_________________ 0
    | a | d| r |

*/

// =========================================================
struct ADSR
// =========================================================
{
   ADSR()
      : m_isBypassed( false )
      , m_a(133.f)
      , m_d(400.f)
      , m_s(0.65f)
      , m_r(2000.f)
   {
      computeDuration();
   }

   void computeDuration() { m_sum = m_a + m_d + m_r; }
   float getDuration() const { return 0.001 * m_sum; } // ms to sec

   float computeSample( double td ) const
   {
      if ( m_isBypassed )
      {
         return 1.0f; // neutral element
      }

      float t = float( 1000.0 * td ); // seconds to milliseconds
      //t = fmodf( t, m_sum ); // seconds to milliseconds

      if ( t <= 0.0f || t >= m_sum )
      {
         return 0.0f;
      }

      if ( t <= m_a )
      {
         float m = 1.0f / m_a;
         float v = t*m;
         return v*v;
      }

      t -= m_a;

      if ( t > 0.0f && t <= m_d )
      {
         float m = (1.0f-m_s)/m_d;
         float v = 1.0f - t*m;
         return v*v;
      }

      t -= m_d;

      if ( t > 0.0f && t < m_r )
      {
         float m = m_s/m_r;
         float v = m_s - t*m;
         return v*v;
      }

      return 0.0f;
   }

   bool isBypassed() const { return m_isBypassed; }
   void setBypassed( bool bypassed ) { m_isBypassed = bypassed; }

   void set( float a, float d, float s, float r )
   {
      m_a = a;
      m_d = d;
      m_s = s;
      m_r = r;
      m_sum = m_a + m_d + m_r;
      DE_DEBUG( toString() )
   }

   void set( ADSR const & other ) { set( other.m_a, other.m_d, other.m_s, other.m_r ); }

   void setA( float ms ) { m_a = ms; computeDuration(); }
   void setD( float ms ) { m_d = ms; computeDuration(); }
   void setS( float pc ) { m_s = pc; }
   void setR( float ms ) { m_r = ms; computeDuration(); }

   std::string toString() const
   {
      std::stringstream s;
      s << "a:" << m_a << ", "
           "d:" << m_d << ", "
           "s:" << m_s << ", "
           "r:" << m_r << ", "
         "sum:" << m_sum;
      return s.str();
   }

   int A() const { return m_a; }
   int D() const { return m_d; }
   int S() const { return m_s; }
   int R() const { return m_r; }

public:
   DE_CREATE_LOGGER("de.audio.ADSR")
   bool m_isBypassed;
   float m_a; // Attack in [ms]
   float m_d; // Decay in [ms]
   float m_s; // Sustain in range [0,1]
   float m_r; // Release in [ms]
   float m_sum;
};

/*
                      1
                    --+--
                  --  |  ---
                --    |      ---
              --      |         ---+---------------+ sustain
            -- m = 1/a| m = (s-1)/d|  m = 0        |
          --   n = 0  | n = +1.0f  |  n = s        |
    0___--____________|____________|_______________|______________ 0
    ####| Attack time | Decay time | sustain level |#####################

float noteOn( double timeSinceNoteOnStart ) const
{
   //float sum = std::abs(a + d + r);
   float t = float( 1000.0 * timeSinceNoteOnStart ); // seconds to milliseconds
   //t = fmodf( t, sum ); // seconds to milliseconds

   if ( t < 0.0f )
   {
      return 0.0f;
   }
   else if ( t <= a )
   {
      float m = 1.0f / a;
      return t*m;
   }
   else if ( t > a && t <= a + d )
   {
      float m = (s - 1.0f) / d;
      return t*m + 1.0f;
   }
   else
   {
      return s; //
   }
}

// 1
// +--
//    --
//      --
//        --------------+  sustain
//                      |----
//                      |    ----
//                      |        ----
// __________________0,0|____________----_____________ t,0
//        ADS           |  ReleaseTime   |

float noteOff( double timeSinceNoteOffStart ) const
{
   //float sum = std::abs(a + d + r);
   float t = float( 1000.0 * timeSinceNoteOffStart ); // seconds to milliseconds
   //t = fmodf( t, sum ); // seconds to milliseconds

   if ( t < 0.0f )
   {
      return 0.0f;
   }
   else if ( t <= r )
   {
      float m = s/r;
      return s - t*m;
   }
   else // if ( t > sum )
   {
      return 0.0f;
   }
}

//        1
//       /|\
//      / | \
//     /  |  ---  sustain
// ___/   |  |   \_________________ 0
//    | a | d| r |
//
float computeValue( double td ) const
{
   //float sum = std::abs(a + d + r);
   float t = float( 1000.0 * td ); // seconds to milliseconds
   //t = fmodf( t, sum ); // seconds to milliseconds

   if ( t < 0.0f )
   {
      return 0.0f;
   }
   else if ( t <= a )
   {
      float m = 1.0f / a;
      return t*m;
   }
   else if ( t > a && t <= a + d )
   {
      float m = -(1.0f-s)/d;
      return (t-a)*m + 1.0f;
   }
   else if ( t > a + d && t <= a + d + r )
   {
      float m = -s/r;
      return (t-(a+d))*m + s;
   }
   else // if ( t > sum )
   {
      return 0.0f;
   }
}
*/
// ===========================================================================
// A note is transfered to the instrument ( RtAudioStream )
// The note commands several things in the instrument ( e.g. Piano/Organ instrument )
// like VoiceId, Volume ( velocity ), midiNote ( frequency ) and later other stuff.
// the note is sent thread safe ( there is a lock but its kept as small as possible )
// the audio thread locks voices and notes, plays them and makes cleanUp
// To reduce io with system ( too slow ) we use std::array not std::vector.
//
// This approach is kinda v3 of my tries and produces best audio so far,
// +++ you can play the same note again and again ( PC keyboard still not an instrument )
// Every note slot gets marked free if (t - m_timeStart > m_adsr.getDuration())
//
// The note is synced with the audio thread on first encounter by audio thread
// This way we always get positive time values t>=0.0. ( prevents bad effects when t < 0 ).
// Without this correction we would call sendNote() at t = 3.2 [s]
// and the audio thread later plays it first on t = 3.25 [s] which would result
// in negative time dt = -0.05 [s] and causes audible clicks at every note start.
// e.g. starting a hullcurve at sin(-0.22) would result in heavy audible cracks and clicks.
// =========================================================
struct Note
// =========================================================
{
   DE_CREATE_LOGGER("de.audio.synth.Note")
   int m_channel;    // default: -1 broadcast note to all voices.
   int m_midiNote;   // midi note in range {0,127}
   int m_velocity;   // midi velocity in range {0,127}
   bool m_noteOff;
   float m_detune;     // note detune in range {-1,1} for one semitone
   int m_pitchWheel; // midi pitch-wheel
   int m_modWheel;   // midi midi-wheel
   double m_timeStart;
   ADSR m_adsr;
   //   float m_a; // Attack in [ms]
   //   float m_d; // Decay in [ms]
   //   float m_s; // Sustain in range [0,1]
   //   float m_r; // Release in [ms]

   Note()
      : m_channel( 0 )
      , m_midiNote( 0 )
      , m_velocity( 90 )
      , m_noteOff( false )
      , m_detune( 0 )
      , m_pitchWheel( 0 )
      , m_modWheel( 0 )
      , m_timeStart( 0 )
//      , m_attackTime( 133.0f )
//      , m_decayTime( 400.0f )
//      , m_sustainLevel( 0.5f )
//      , m_sustainTime( 0.0f )
//      , m_releaseTime( 2000.0f )
   {}

   Note( int channel, int midiNote, int detuneInCents = 0, int velocity = 90, double pts = 0.0 )
      : m_channel( channel )
      , m_midiNote( midiNote )
      , m_velocity( velocity )
      , m_noteOff( false )
      , m_detune( detuneInCents )
      , m_pitchWheel( 0 )
      , m_modWheel( 0 )
      , m_timeStart( pts )
//      , m_attackTime( 133.0f )
//      , m_decayTime( 400.0f )
//      , m_sustainLevel( 0.5f )
//      , m_sustainTime( 0.0f )
//      , m_releaseTime( 2000.0f )
   {}

//   float getDuration() const { return 0.001f * (m_attackTime + m_decayTime + m_sustainTime + m_releaseTime); }

   //float getDuration() const { return m_a + m_d + m_r; }

   float getDuration() const { return m_adsr.getDuration(); }
};

// ===========================================================================
struct IMidiChainElement // The name is stupid but easy to read and find in code.
// ===========================================================================
{
   virtual ~IMidiChainElement() {}

   virtual uint64_t
   processMidiEvents( uint8_t const* beg, uint8_t const* end ) = 0;

//   virtual uint64_t
//   processVstMidiEvents( std::vector< VstMidiEvent > const & events ) = 0;
};

// ===========================================================================
struct IDspChainElement // The name is stupid but easy to read and find in code.
// ===========================================================================
{
   virtual ~IDspChainElement() {}

   /// @brief DSP element is a MIDI synthesizer
   //virtual uint32_t getVoiceCount() const = 0;
   //virtual IVoice* getVoice( uint32_t ) = 0;
   //virtual void setVoice( uint32_t slot, IVoice* voice ) = 0;
   //virtual de::audio::ADSR const* getADSR() const = 0;
   //virtual de::audio::ADSR* getADSR() = 0;
   //virtual void noteOn( int voiceId, int midiNote, int velocity = 90 ) = 0;
   //virtual void noteOff( int voiceId, int midiNote ) = 0;
   virtual bool isSynth() const { return false; }

   // --- interface ---
   virtual void sendMidi( uint8_t byte1, uint8_t data1, uint8_t data2 )
   {
      //std::cout << "sendMidi() - not implemented" << std::endl;
   }

   // --- quick helper ---
   virtual void allNotesOff() { sendMidi( 0xB0, 0x7B, 0x00 ); } // ch 0

   // --- quick helper ---
   virtual void sendNote( de::audio::Note const & note )
   {
      uint8_t c = note.m_channel;
      uint8_t m = note.m_midiNote;
      uint8_t v = note.m_velocity;

      // VST Plugins mostly use explicit 0x80 for note off, not 0x90 (note-on) with velocity = 0.
      // MIDI files use the (running status) trick to save some hdd space.
      //bool polyMode = true;

      if ( note.m_noteOff )
      {
         sendMidi( 0x80 | c, m, v );
      }
      else
      {
         sendMidi( 0x90 | c, m, v );
      }

      /*
      if ( polyMode )
      {
         if ( v < 1 )
         {
            sendMidi( 0x80 | c, m, 0 );
         }
         else
         {
            sendMidi( 0x90 | c, m, v );
         }
      }
      else
      {
         allNotesOff();
         if ( v > 0 )
         {
            sendMidi( 0x90 | c, m, v );
         }
      }
      */
   }

   /// @brief Caller presents a buffer to be filled with samples to build backward DSP chains.
   /// @param pts The presentation time stamp in [s]
   /// @param dst The destination buffer to be filled with dstSampleCount = dstFrames * dstChannels.
   /// @return Should return the num expected samples in 'dst' buffer = dstFrames * dstChannels.
   ///         If returns less then caller has to react, fade out, fill with zeroes or so.
   ///         But the source is expected to deliver zeroes or something else on its own in error cases.
   ///         So normally we expect the callee to fill everything and we dont have to care in most cases.
   virtual uint64_t
   readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) = 0;

   /// @brief Caller presents a buffer to be filled with samples to build backward DSP chains.
   /// @param pts The presentation time stamp in [s]
   /// @param dst The destination buffer to be filled with dstSampleCount = dstFrames * dstChannels.
   virtual void
   aboutToStart( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) {}

   virtual void
   aboutToStop() {}

   // Not really used
   virtual void
   clearInputSignals() {}

   // Most important setter for all effect DSP elements, empty for synths and players.
   virtual void
   setInputSignal( int i, IDspChainElement* input ) {}

   // No input means a synth or player. With input(s) means a relay/effect/processor/display/mixer.
   virtual uint32_t
   getInputSignalCount() const { return 0; }

   /// @brief Used in mixer
   virtual IDspChainElement*
   getInputSignal( uint32_t i ) const { return nullptr; }

   // All DSP elements need atleast one output. (must be >=1)
   // The AudioEndpoint (RtAudioStream) is special and has no output
   // since it is the end of the DSP chain. Therefore Endpoint is no real DSP element.
   //virtual uint32_t
   //getOutputSignalCount() const { return 1; }

   virtual uint32_t
   getOutputSampleRate() const { return 48000; }

   virtual bool
   isBypassed() const { return false; }
   virtual void
   setBypassed( bool bypassed ) {}

   virtual int32_t
   getVolume() const { return 100; }

   virtual void
   setVolume( int32_t vol_in_pc ) {}


   // Only needed for TimeStretch like plugins
   //virtual uint32_t
   //getInputSampleRate() const { return 0; }

   // Only needed for TimeStretch like plugins
   //virtual uint32_t
   //getOutputSampleRate() const { return 0; }

   //virtual uint32_t
   //getInputFrameCount() const { return 0; }

   //virtual uint32_t
   //getOutputFrameCount() const { return 0; }


};


template < typename T >
inline void
DSP_RESIZE( std::vector< T > & dst, uint64_t dstSamples )
{
   if ( dstSamples != dst.size() ) dst.resize( dstSamples );
}

template < typename T >
inline void
DSP_FILLZERO( std::vector< T > & dst )
{
   auto p = dst.data();
   for ( size_t i = 0; i < dst.size(); ++i ) { *p = T(0); p++; }
}

inline void
DSP_FILLZERO( std::vector< float > & dst, uint64_t sampleCount )
{
   auto p = dst.data();
   sampleCount = std::min( sampleCount, dst.size() );
   for ( size_t i = 0; i < sampleCount; ++i ) { *p = 0.0f; p++; }
}

inline void
DSP_FILLZERO( float* dst, uint64_t sampleCount )
{
   if ( !dst ) return;
   if ( sampleCount < 1 ) return;
   for ( size_t i = 0; i < sampleCount; ++i ) { *dst = 0.0f; dst++; }
}

inline void
DSP_FILLZERO( double* dst, uint64_t sampleCount )
{
   if ( !dst ) return;
   if ( sampleCount < 1 ) return;
   for ( size_t i = 0; i < sampleCount; ++i ) { *dst = 0.0; dst++; }
}

// Reads one channel with sampleCount sc = fc * cc from src to dst.
inline void
DSP_GET_CHANNEL(
   float* dst,          // Destination mono channel buffer
   uint64_t dstFrames,  // Destination mono channel buffer frame count
   float const* src,    // Source multi channel buffer start,
   uint64_t srcFrames,  // Source multi channel buffer frame count 'fc'.
   uint32_t srcChannel, // The channel you want to extract 'ci'
   uint32_t srcChannels)// The number of channels the source has 'cc'.
{
   src += srcChannel; // Advance to first sample of desired channel.
   auto n = std::min( srcFrames, dstFrames );
   for ( size_t i = 0; i < n; i++ )
   {
      *dst = (*src);
      dst++;
      src += srcChannels; // jump to next sample of srcChannel.
   }
}

inline void
DSP_FUSE_STEREO_TO_MONO( float* mono,           // the destination single channel buffer
                         float const* src,      // the source buffer with any num of channels
                         uint64_t srcFrames,
                         uint32_t srcChannels )
{
   if ( srcChannels < 1 )
   {
      return; // Nothing todo, no data.
   }
   else if ( srcChannels == 1 )  // Mono Rail
   {
      for ( size_t i = 0; i < srcFrames; i++ )
      {
         *mono++ = *src++; // directly read and write L.
      }
   }
   else if ( srcChannels == 2 ) // Optimized for stereo ( only one add! op. less )
   {
      for ( size_t i = 0; i < srcFrames; i++ )
      {
         float L = *src++;       // read iL
         float R = *src++;       // read iR
         *mono = 0.5f * (L+R);   // write oL = (iL+iR)/2
         mono++;                 // Advance to next sample oL
      }
   }
   else // if ( srcChannels > 2 )
   {
      auto skipAmount = srcChannels - 2;
      for ( size_t i = 0; i < srcFrames; i++ )
      {
         float L = *src++;       // read iL
         float R = *src++;       // read iR
         src += skipAmount;      // Advance to next sample iL
         *mono = 0.5f * (L+R);   // write oL = (iL+iR)/2
         mono++;                 // Advance to next sample oL
      }
   }

}

inline void
DSP_COPY( float const* src, float* dst, uint64_t sampleCount )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst = *src;
      dst++;
      src++;
   }
}

inline void
DSP_ADD( float const* src, float* dst, uint64_t sampleCount )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst += *src;
      dst++;
      src++;
   }
}

inline void
DSP_MUL( float* dst, uint64_t sampleCount, float scalar )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst *= scalar;
      dst++;
   }
}

inline void
DSP_MUL_LIMITED( float* dst, uint64_t sampleCount, float scalar )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst = std::clamp( (*dst) * scalar, -1.0f, 1.0f ); // Limiter.
      dst++;
   }
}

inline void
DSP_COPY_SCALED( float const* src, float* dst, uint64_t sampleCount, float scale )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst = (*src) * scale;
      dst++;
      src++;
   }
}

inline void
DSP_COPY_SCALED_LIMITED( float const* src, float* dst, uint64_t sampleCount, float scale )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst = std::clamp( (*src) * scale, -1.0f, 1.0f ); // Limiter.
      dst++;
      src++;
   }
}

//inline void
//DSP_PROC( float const* src, float* dst, uint32_t sampleCount, std::function<float(float,float)> const & manipSample )
//{
//   for ( size_t i = 0; i < sampleCount; ++i )
//   {
//      *dst = manipSample(*src,*dst);
//      dst++;
//      src++;
//   }
//}

// RMS - root means square:
inline float
DSP_RMSf( uint32_t channel, float const* src, uint64_t srcFrames, uint32_t srcChannels )
{
   float rms = 0.0f;

   src += channel; // Advance to first sample of 'channel'

   for ( size_t i = 0; i < srcFrames; ++i )
   {
      float sample = *src;
      src += srcChannels;
      rms += sample * sample;
   }

   rms /= float( srcFrames );
   return sqrtf( rms );
}

// RMS - root means square:
inline double
DSP_RMSd( uint32_t channel, float const* src, uint64_t srcFrames, uint32_t srcChannels )
{
   double rms = 0.0;

   src += channel; // Advance to first sample of 'channel'

   for ( size_t i = 0; i < srcFrames; ++i )
   {
      double sample = *src;
      src += srcChannels;
      rms += sample * sample;
   }

   rms /= double( srcFrames );
   return sqrt( rms );
}


} // end namespace audio
} // end namespace de

/*
   struct IMultiChannelFrequencySignal
   {
      virtual ~IMultiChannelFrequencySignal() {}

      /// @brief Caller presents a buffer to be filled with samples to build backward DSP chains.
      /// @param pts The presentation time stamp in [s]
      /// @param dst The destination buffer to be filled with dstSampleCount = dstFrames * dstChannels.
      virtual uint64_t
      readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) = 0;

   };

   /// @brief Is the source playing? Asked by caller like the SoundCard
   virtual bool
   is_playing() const { return true; }

   /// @brief Starts from begin if was stopped or resumes at last pos when was paused.
   virtual void
   dsp_play() {}

   /// @brief Wishes to stop with fade out or at next beat grid border?
   virtual void
   dsp_stop() {}

   /// @brief Is the source paused? Not asked by SoundCard. Only some info bar?
   virtual bool
   dsp_is_paused() const { return false; }

   /// @brief Wishes to stop with fade out and staying at same position.
   ///        The source advances while fade out a bit, so you dont resume at the same pos you pressed the button
   virtual void
   dsp_pause() {}

   /// @brief Is the source looping?
   virtual bool
   dsp_is_looping() const { return false; }

   virtual int32_t
   dsp_loops() const { return 0; }

   virtual void
   dsp_loop( int32_t loops ) {}

   virtual void
   dsp_set_loops( int32_t loops ) {}

// IStereoSignal
template < typename T >
struct IDSP
{
   /// @brief Caller presents a buffer to be filled with samples to build backward DSP chains.
   /// @param pts The presentation time stamp in [s]
   /// @param dst The destination buffer to be filled with dstSampleCount = dstFrames * dstChannels.
   /// @return Should return the num expected samples in 'dst' buffer = dstFrames * dstChannels.
   ///         If returns less then caller has to react, fade out, fill with zeroes or so.
   ///         But the source is expected to deliver zeroes or something else on its own in error cases.
   ///         So normally we expect the callee to fill everything and we dont have to care in most cases.
   uint64_t
   readSamples( T pts, T* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate );

   /// @brief Caller presents a buffer to be filled with samples to build backward DSP chains.
   /// @param pts The presentation time stamp in [s]
   /// @param dst The destination buffer to be filled with dstSampleCount = dstFrames * dstChannels.
   void
   initDsp( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate );

   bool
   isBypassed() const;
   void
   setBypassed( bool enabled );

   void
   clearInputSignals();

   void
   setInputSignal( int i, IDSP* input );

   uint32_t
   getSampleRate() const;
};

//typedef IDSP< float > IDSP32;
//typedef IDSP< double > IDSP64;

// IAudioSource
// IAudioProducer
// ISoundProducer
// ISoundProcessor
// IFunElement
// IMusiker
// IKrachProducer
// ITierschreck
// IHundepfeife
// IDogWhistler
// IOhrenReiniger
// INeighborVisitProducer
// IBeeingMurderedEnhancer
// IDanceEnhancer
// IKaraoke
// IEntarteteKunst
// ISonicPollution
// IEarStress
// IEarPain
// IMultiChannelAudioSignalChainElement
// IBackwardDspChainElement
// IDSP,
// IDspElem
// IAudioSignal,
// ITimeSignal ( compared to complex frequency signals after fft )
// IMultiChannelAudioTimeSignal
// IStereoSignal

// ===========================================================================
struct ISignalChainElement
// ===========================================================================
{
   virtual ~ISignalChainElement() {}

   virtual float
   computeSignal( double pts ) = 0;

   //virtual void
   //setupDspElement( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) {}
};

// ===========================================================================
struct IVoice : public ISignalChainElement
// ===========================================================================
{
   virtual ~IVoice() = default;

   enum eParam
   {
      eP_Unknown = 0,
      eP_Frequency,
      eP_MidiNote,
      eP_Detune,
      eP_FineDetune,
      eP_AttackTime,
      eP_DecayTime,
      eP_SustainLevel,
      eP_ReleaseTime,
   };

   virtual uint32_t paramCount() const { return 0; }
   virtual float paramf( uint32_t paramId ) const { return 0.0f; }

   virtual eParam paramType( uint32_t param ) const { return eP_Unknown; }
   virtual std::string paramName( uint32_t param ) const { return "Unknown"; }
   virtual std::string paramInfo( uint32_t param ) const { return "Unknown"; }

   virtual float valuef( uint32_t param ) const { return 0.0f; }
   virtual int valuei( uint32_t param ) const { return 0; }
   virtual uint32_t valueu( uint32_t param ) const { return 0; }

   virtual float minf( uint32_t param ) const { return 0.0f; }
   virtual int mini( uint32_t param ) const { return 0; }
   virtual uint32_t minu( uint32_t param ) const { return 0; }

   virtual float maxf( uint32_t param ) const { return 0.0f; }
   virtual int maxi( uint32_t param ) const { return 0; }
   virtual uint32_t maxu( uint32_t param ) const { return 0; }

   virtual float stepf( uint32_t param ) const { return 0.0f; }
   virtual int stepi( uint32_t param ) const { return 0; }
   virtual uint32_t stepu( uint32_t param ) const { return 0; }
};


// ===========================================================================
struct PianoStringVoice : public IVoice
// ===========================================================================
{
   PianoStringVoice()
   {

   }

   ~PianoStringVoice()
   {}

   de::audio::ADSR m_adsr;
   de::audio::OscSine< float > m_oscSine1;

   de::audio::OscSine< float > m_oscSine2;
   de::audio::OscSine< float > m_oscSine3;

   de::audio::OscSine< float > m_oscSine4;
   de::audio::OscSine< float > m_oscSine5;
   de::audio::OscTriangle< float > m_oscTriangle;
   de::audio::OscRect< float > m_oscRect;
   de::audio::OscSaw< float > m_oscSaw;
   de::audio::OscNoise< float > m_oscNoise;
   std::array< de::audio::IOSC<float>*, 9 > m_oscs;
};
*/
