#pragma once
#include "BetterSynthVoice.hpp"
#include <array>
#include <vector>
#include <mutex>

namespace de {
namespace audio {

// ============================================================================
struct BetterSynth : public IDspChainElement
// ============================================================================
{
   BetterSynth();
   ~BetterSynth() override;

   uint64_t
   readDspSamples( double pts, float* dst,
                  uint32_t dstFrames,
                  uint32_t dstChannels,
                  double dstRate ) override;

   bool isSynth() const override { return true; }
   void sendNote( Note const & note ) override;

   bool isBypassed() const;
   void setBypassed( bool bypass );

public:

   // Each command triggers a note ( no release event needed, yet, more like single shots )
   // =========================================================
   struct NoteSlot
   // =========================================================
   {
      bool m_isPlaying; // default: true, so it auto starts playing
      bool m_wasPlaying;// default: false, used for sync
      float m_duration;
      Note m_note;
      std::array< BetterSynthVoice, 8 > m_voices;

      NoteSlot()
         : m_isPlaying( true )
         , m_wasPlaying( false ) // false triggers a sync with audio thread stream time,
         , m_duration( 0 )
      {}

      float getDuration() const { return m_duration; }

      void computeDuration()
      {
         m_duration = 0.0f;
         for ( auto & voice : m_voices )
         {
            if ( voice.isBypassed() ) continue;
            m_duration = std::max( m_duration, voice.m_adsr.getDuration() );
         }
      }
   };

   DE_CREATE_LOGGER("BetterSynth")
   bool m_isBypassed;
   //int m_volume;
   //std::vector< float > m_inputBuffer;
   //std::vector< float > m_mixerBuffer;
   std::array< BetterSynthVoice, 8 > m_voices;
   std::array< NoteSlot, 32 > m_notes;
   std::mutex m_mutex; // std::atomic< bool >
};

} // end namespace audio
} // end namespace de
