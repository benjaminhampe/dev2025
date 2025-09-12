#include "BetterSynth.hpp"

namespace de {
namespace audio {

BetterSynth::BetterSynth()
   : m_isBypassed( true )
{
   //DSP_RESIZE( m_inputBuffer, 2*1024 );
   //DSP_RESIZE( m_mixerBuffer, 2*1024 );
   for ( auto & note : m_notes )
   {
      note.m_isPlaying = false;
      note.m_wasPlaying = false;
   }

   for ( auto & voice : m_voices )
   {
      voice.setBypassed( true );
   }

   m_voices[0].setBypassed( false );
}

BetterSynth::~BetterSynth()
{

}

bool
BetterSynth::isBypassed() const { return m_isBypassed; }

void
BetterSynth::setBypassed( bool bypassed )
{
   m_isBypassed = bypassed;

   // if ( !m_isBypassed )
   // {
      // for ( auto & noteSlot : m_notes )
      // {
         // noteSlot.m_isPlaying = false;
         // noteSlot.m_wasPlaying = false;
      // }
   // }

   DE_DEBUG("BetterSynth.setBypassed(",bypassed,")")
}

void
BetterSynth::sendNote( Note const & note )
{
   if ( m_isBypassed ) return;

   auto emplaceNote = [&] ( Note const & enote )
   {
      for ( size_t i = 0; i < m_notes.size(); ++i )
      {
         auto & slot = m_notes[ i ];
         if ( !slot.m_isPlaying )
         {
            slot.m_note = enote;
            slot.m_voices = m_voices;
            slot.m_wasPlaying = false;
            slot.m_isPlaying = true;
            slot.computeDuration();
            break;
         }
      }
   };

   {
      std::lock_guard< std::mutex > lg( m_mutex );
      emplaceNote( note );
   }

   //DE_DEBUG("NoteOn(",note.m_midiNote,"), freq(",note.m_midiFreq,"), slot(", slotIndex,")" )
}

uint64_t
BetterSynth::readDspSamples( double pts,
                              float* dst,
                              uint32_t dstFrames,
                              uint32_t dstChannels,
                              double dstRate )
{
   ///! Compute num samples that needs to be processed by this DSP
   uint64_t dstSamples = dstFrames * dstChannels;

   //DSP_RESIZE( m_inputBuffer, dstSamples );

   ///! Safe clear output buffer ( 100% silence )
   DSP_FILLZERO( dst, dstSamples );

   if ( m_isBypassed )
   {
      return dstSamples;
   }

   ///! Compute time step
   double dt = 1.0 / double( dstRate );

   ///! Loop all pressed MIDI/Keyboard notes
   size_t noteCount = 0;
   {
      std::lock_guard< std::mutex > lg( m_mutex );

//      // Count active notes
//      for ( auto & note : m_notes )
//      {
//         if ( note.m_isPlaying ) noteCount++;
//      }

      ///! Add notes to mixer
      for ( size_t i = 0; i < m_notes.size(); ++i )
      {
         auto & slot = m_notes[ i ];
         if ( slot.m_isPlaying )
         {
            // Start playing
            if ( !slot.m_wasPlaying )
            {
               slot.m_note.m_timeStart = pts; // This prevents dropouts for unsync start times < 0.0
               slot.m_wasPlaying = true;
            }

            // Stop when ADSR duration exceeded.
            double t = pts - slot.m_note.m_timeStart;
            if ( t >= slot.getDuration() )
            {
               slot.m_isPlaying = false;
               slot.m_wasPlaying = false;
               continue;
            }

            // Play the voices:
            for ( size_t v = 0; v < slot.m_voices.size(); ++v )
            {
               auto & voice = slot.m_voices[ v ];
               voice.addSamples(
                        slot.m_note.m_midiNote,
                        slot.m_note.m_velocity,
                        t,
                        dt,
                        dst, dstFrames, dstChannels, dstRate );
            }
         }
      }
   }

   return dstSamples;
}



} // end namespace audio
} // end namespace de
