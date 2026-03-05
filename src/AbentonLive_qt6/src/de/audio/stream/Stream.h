#pragma once
#include <de/audio/dsp/IDspChainElement.h>

namespace de {
namespace audio {

// ===========================================================================
struct Stream
// ===========================================================================
{
   Stream();
   ~Stream();

   void clearInputSignals();
   void setInputSignal( int i, IDspChainElement* input );

   int getVolume() const;
   void setVolume( int vol_in_percent );
   bool isPlaying() const;

   void play();
   void stop();

protected:
   DE_CREATE_LOGGER("de.audio.Stream")
   void* m_impl;
};

} // end namespace gpu.
} // end namespace de.

