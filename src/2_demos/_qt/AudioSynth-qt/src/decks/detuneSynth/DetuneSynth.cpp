#include "DetuneSynth.hpp"

namespace de {
namespace audio {

DetuneSynth::DetuneSynth()
   : m_isBypassed( false )
{
   m_lfoPhase.setBypassed( true );
   m_lfoPulseWidth.setBypassed( true );
   m_lfoDetune.setBypassed( true );
   m_lfoPhase.setBypassValue( 0.0f );
   m_lfoPulseWidth.setBypassValue( 0.5f );
   m_lfoDetune.setBypassValue( 0.0f );

   //DSP_RESIZE( m_inputBuffer, 2*1024 );
   //DSP_RESIZE( m_mixerBuffer, 2*1024 );

   for ( auto & note : m_NoteSlots )
   {
      note.m_isPlaying = false;
      note.m_wasPlaying = false;
   }

}

DetuneSynth::~DetuneSynth()
{

}

bool
DetuneSynth::isBypassed() const { return m_isBypassed; }

void
DetuneSynth::setBypassed( bool bypass )
{
   m_isBypassed = bypass;
//   if ( !m_isBypassed )
//   {
//      for ( auto & noteSlot : m_NoteSlots )
//      {
//         noteSlot.m_isPlaying = false;
//         noteSlot.m_wasPlaying = false;
//      }
//   }
}

void
DetuneSynth::sendNote( Note const & note )
{
   auto emplaceNote = [&] ( Note const & enote )
   {
      for ( size_t i = 0; i < m_NoteSlots.size(); ++i )
      {
         auto & slot = m_NoteSlots[ i ];
         if ( !slot.m_isPlaying )
         {
            slot.m_note = enote;
            slot.m_note.m_adsr = m_adsr;
            slot.m_osc = m_osc;
            slot.m_lfoPhase = m_lfoPhase;
            slot.m_lfoPulseWidth = m_lfoPulseWidth;
            slot.m_lfoDetune = m_lfoDetune;
            slot.m_wasPlaying = false;
            slot.m_isPlaying = true;
            break;
         }
      }
   };

   {
      //std::lock_guard< std::mutex > lg( m_NoteSlotsMutex );
      emplaceNote( note );
   }

   //DE_DEBUG("NoteOn(",note.m_midiNote,"), freq(",note.m_midiFreq,"), slot(", slotIndex,")" )
}

uint64_t
DetuneSynth::readDspSamples(
   double pts,
   float* dst,
   uint32_t dstFrames,
   uint32_t dstChannels,
   double dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;
   //DSP_RESIZE( m_inputBuffer, dstSamples );
   //DSP_RESIZE( m_mixerBuffer, dstSamples );
   //DSP_FILLZERO( m_mixerBuffer );
   DSP_FILLZERO( dst, dstSamples );
   if ( m_isBypassed )
   {
      return dstSamples;
   }

   double dt = 1.0 / double( dstRate );

   size_t noteCount = 0;
   {
      //std::lock_guard< std::mutex > lg( m_NoteSlotsMutex );

//      // Count active notes
//      for ( auto & note : m_NoteSlots )
//      {
//         if ( note.m_isPlaying ) noteCount++;
//      }

      // Add notes to mixer
      for ( size_t i = 0; i < m_NoteSlots.size(); ++i )
      {
         auto & slot = m_NoteSlots[ i ];
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

            // Play the OSC:
//            m_adsr.set( slot.m_note.m_attackTime,
//                        slot.m_note.m_decayTime,
//                        slot.m_note.m_sustainLevel,
//                        slot.m_note.m_releaseTime );

            float* pDst = dst;

            //DE_DEBUG( slot.m_lfoDetune.toString() )

            for ( size_t f = 0; f < dstFrames; f++ )
            {
               float midiNote = float(slot.m_note.m_midiNote)
                            + slot.m_note.m_detune
                            + slot.m_lfoDetune.computeSample( t );

               float freq = getFrequencyFromMidiNote( midiNote );

               //DE_DEBUG( "i[",f,"] midiNote = ", midiNote,", freq = ", freq )

//               int midiNote = slot.m_note.m_midiNote;
//               float detune = slot.m_note.m_detune
//                            + slot.m_lfoDetune.computeSample( t );
//               float freq = getFrequencyFromMidiNote( midiNote, detune );
               float phase = slot.m_lfoPhase.computeSample( t );
               float pw = slot.m_lfoPulseWidth.computeSample( t );

               float adsr = slot.m_note.m_adsr.computeSample( t );
               slot.m_osc.setFrequency( freq );
               slot.m_osc.setPhase( phase );
               slot.m_osc.setPulseWidth( pw );
               float sample = slot.m_osc.computeSample( t ) * adsr;
               for ( size_t c = 0; c < dstChannels; c++ )
               {
                  *pDst += sample;
                  pDst++;
               }
               t += dt;
            }
            //glm::smoothstep()
            //DSP_ADD( m_inputBuffer.data(), m_mixerBuffer.data(), dstSamples );
         }
      }
   }

   //DSP_COPY( m_mixerBuffer.data(), dst, dstSamples );
   return dstSamples;
}



} // end namespace audio
} // end namespace de
