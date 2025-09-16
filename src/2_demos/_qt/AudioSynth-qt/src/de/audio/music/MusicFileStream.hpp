#pragma once
#include "AudioQueue.hpp"
#include <de/audio/buffer/io/DecoderFFMPEG.hpp>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace de {
namespace audio {

// Uses libAVCodec/ffmpeg to decode any known file format to normal humans.
// Creates a background thread opening the file solely to load audio from it.
// It blocks until data is cached for playing and then signals the condition_var.
// All but one audio stream is enabled, all other packets are discarded.
// So far not seekable but also works for files with broken index.
// So it works for any audio/movie file

struct MusicFileStream
{
   DE_CREATE_LOGGER("de.audio.MusicFileStream")
   std::string m_Uri;
   std::thread* m_ThreadPtr;
   mutable std::condition_variable m_RunFence;
   std::atomic< bool > m_IsRunning;
   bool m_DidRun;
   int m_StreamIndex;
   std::unique_ptr< decoder::DecoderFFMPEG > m_File;
   AudioQueue m_BufferQueue;

   MusicFileStream();
   ~MusicFileStream();
   uint64_t getFrameCount() const;
   uint64_t getFrameIndex() const;
   bool isPlaying() const;
   bool isRunning() const;
   void close();
   bool open( std::string uri, int streamIndex = -1 );
   void run();
};

} // end namespace audio
} // end namespace de
