#pragma once
#include <de/audio/dsp/ADSR.hpp>

namespace de {
namespace audio {

// IStereoSignal
//template < typename T >
//struct IDSP
//{
//   /// @brief Caller presents a buffer to be filled with samples to build backward DSP chains.
//   /// @param pts The presentation time stamp in [s]
//   /// @param dst The destination buffer to be filled with dstSampleCount = dstFrames * dstChannels.
//   /// @return Should return the num expected samples in 'dst' buffer = dstFrames * dstChannels.
//   ///         If returns less then caller has to react, fade out, fill with zeroes or so.
//   ///         But the source is expected to deliver zeroes or something else on its own in error cases.
//   ///         So normally we expect the callee to fill everything and we dont have to care in most cases.
//   uint64_t
//   readSamples( T pts, T* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate );

//   /// @brief Caller presents a buffer to be filled with samples to build backward DSP chains.
//   /// @param pts The presentation time stamp in [s]
//   /// @param dst The destination buffer to be filled with dstSampleCount = dstFrames * dstChannels.
//   void
//   initDsp( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate );

//   bool
//   isBypassed() const;
//   void
//   setBypassed( bool enabled );

//   void
//   clearInputSignals();

//   void
//   setInputSignal( int i, IDSP* input );

//   uint32_t
//   getSampleRate() const;
//};

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

/*
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

// =======================
struct DspBuffer
// =======================
{
   std::vector< uint8_t > bytes;
};

// =======================
struct DspBufferRef
// =======================
{
   float* refPtr = nullptr;
   uint32_t frames = 0;
   uint32_t channels = 0;
   uint32_t rate = 48000;
};

*/

typedef struct std::vector< uint8_t >        DspMidiEvent;
typedef struct std::vector< DspMidiEvent >   DspMidiEvents;


// =======================
struct DspMidiEventsRef
// =======================
{
   DspMidiEvents* refPtr = nullptr;
};


// ===========================================================================
struct IDspChainElement // The name is stupid but easy to search for.
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
   virtual void sendNote( Note const & note ) {}

   virtual int receiveDspMidi( DspMidiEvents* midiEvents )
   {
      return 1; // Success
   }

   virtual int sendDspMidi( DspMidiEvents* midiEventsPtr )
   {
      if ( !midiEventsPtr ) return 1;// Success

      DspMidiEvents & midiEvents = * midiEventsPtr;

      size_t n = midiEvents.size();

      DE_ERROR("MidiEvents = ",midiEvents.size() )
      for ( size_t i = 0; i < n; ++i )
      {
         DE_ERROR("MidiEvent[",i,"] = ",midiEvents[i].size()," Bytes" )
      }
      return 1; // Success
   }

   /// @brief Caller presents a buffer to be filled with samples to build backward DSP chains.
   /// @param pts The presentation time stamp in [s]
   /// @param dst The destination buffer to be filled with dstSampleCount = dstFrames * dstChannels.
   /// @return Should return the num expected samples in 'dst' buffer = dstFrames * dstChannels.
   ///         If returns less then caller has to react, fade out, fill with zeroes or so.
   ///         But the source is expected to deliver zeroes or something else on its own in error cases.
   ///         So normally we expect the callee to fill everything and we dont have to care in most cases.
   virtual uint64_t
   readDspSamples( double pts,
                   float* dst,
                   uint32_t frameCount,
                   uint32_t channelCount,
                   double sampleRate ) = 0;

   /// @brief Caller presents a buffer to be filled with samples to build backward DSP chains.
   /// @param pts The presentation time stamp in [s]
   /// @param dst The destination buffer to be filled with dstSampleCount = dstFrames * dstChannels.
   virtual void
   setupDspElement( uint32_t frameCount,
                    uint32_t channelCount,
                    double sampleRate ) {}

   // Most important setter for all effect DSP elements, empty for synths and players.
   virtual void
   setInputSignal( int i, IDspChainElement* input ) {}

   // No input means a synth or player. With input(s) means a relay/effect/processor/display/mixer.
   virtual uint32_t
   getInputSignalCount() const { return 0; }

   /// @brief Used in mixer
   virtual IDspChainElement*
   getInputSignal( uint32_t i ) const { return nullptr; }

   // Not really used
   virtual void
   clearInputSignals() {}

   // All DSP elements need atleast one output. (must be >=1)
   // The AudioEndpoint (RtAudioStream) is special and has no output
   // since it is the end of the DSP chain. Therefore Endpoint is no real DSP element.
   //virtual uint32_t
   //getOutputSignalCount() const { return 1; }

   virtual uint32_t
   getOutputSampleRate() const { return 48000; }

//   virtual bool
//   isBypassed() const { return false; }
//   virtual void
//   setBypassed( bool enabled ) {}

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
*/

} // end namespace audio
} // end namespace de
