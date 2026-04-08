#pragma once
#include <de/audio/dsp/IDspChainElement.h>
#include <cstdint>
#include <functional>

namespace de {
namespace audio {

class EndPoint_RtAudio_Private;

// =======================================================
class EndPoint_RtAudio
// =======================================================
{
public:
    EndPoint_RtAudio();
    ~EndPoint_RtAudio();

    int getSampleRate() const;
    int getBufferSize() const;
    int getChannelCount() const;

    void setInputSignal( IDspChainElement* inputSignal );


    void start();
    void stop();
	
public:
    EndPoint_RtAudio_Private* m_impl;	
};

} // end namespace audio.
} // end namespace de.
