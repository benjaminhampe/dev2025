#pragma once
#include <de/audio/dsp/IDspChainElement.h>

#if 0

namespace de {
namespace audio {

class FileReader_MP3_Impl;

// ===========================================================================
class FileReader_MP3 : public IFileStreamer
// ===========================================================================
{
public:
    FileReader_MP3();
    ~FileReader_MP3() override;

    void openStream( std::string uri ) override;

    void closeStream() override;

    bool isStreamOpen() const override;


    std::string getUri() const override;

    FileInfo getFileInfo() const override;

    // void play() const override;

    // void stop() const override;

    // bool isPlaying() const override;

    // void pause() const override;

    // void resume() const override;

    u64 tellFrame() const override;

    bool seekFrame( u64 frame ) const override;

    // u64 readFrames( u64 frames, f32* __restrict__ L, f32* __restrict__ R ) override;


    void dsp_init(u64 frames, u32 channels, u32 sampleRate) override;

    void dsp_read(f64 pts, u32 frames, u32 sampleRate,
                          f32* __restrict__ L,
                  f32* __restrict__ R ) override;

    u32 dsp_getInputSignalCount() const = 0;

    IDspChainElement* dsp_getInputSignal(int i = 0) = 0;

    void dsp_setInputSignal(IDspChainElement* input, int i = 0) = 0;

    void dsp_clearInputSignals() = 0;

private:
    FileReader_MP3_Impl* _d;
};

} // end namespace audio.
} // end namespace de.

#endif