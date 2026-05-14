#pragma once
#include <de/audio/file/FileInfo.h>

namespace de {
namespace audio {

// ===========================================================================
class IFileStreamer : public IDspChainElement
// ===========================================================================
{
public:
    virtual ~IFileStreamer() = default;


    virtual void openStream( std::string uri ) = 0;

    virtual void closeStream() = 0;

    virtual bool isStreamOpen() const = 0;


    virtual const FileInfo& getFileInfo() const = 0;

    // virtual void play() const = 0;

    // virtual void stop() const = 0;

    // virtual bool isPlaying() const = 0;

    // virtual void pause() const = 0;

    // virtual void resume() const = 0;


    virtual u64 tellFrame() const = 0;

    virtual bool seekFrame( u64 frame ) const = 0;



    // virtual u64 readFrames( u64 frames, f32* __restrict__ L, f32* __restrict__ R ) = 0;
};


} // end namespace audio.
} // end namespace de.
