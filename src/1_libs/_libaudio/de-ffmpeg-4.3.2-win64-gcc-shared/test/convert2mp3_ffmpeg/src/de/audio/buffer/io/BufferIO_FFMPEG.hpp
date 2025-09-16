#ifndef DE_AUDIO_SAMPLEBUFFER_MP3_HPP
#define DE_AUDIO_SAMPLEBUFFER_MP3_HPP

#include <de/audio/buffer/Buffer.hpp>

namespace de {
namespace audio {
namespace io {

struct BufferFFMPEG
{
   DE_CREATE_LOGGER("de.audio.BufferFFMPEG")

   static bool
   load( Buffer & dst,
         std::string uri,
         int audioStreamIndex = -1,
         ESampleType convertType = ST_Unknown,
         int sampleRate = 0 );

   static bool
   save( Buffer const & dst, std::string uri );
};

// This is heavy load, single threaded.
// Use it wise. I converted 800 MB mkv video 4hrs audio to 2,3 GB raw wav buffer.
// It took almost 30 seconds, so nothing for non-blocking realtime.
// But it can load an entire sound data base very easily for user.
// Any format that is supported by ffmpeg is supported,
// e.g. mp3, mkv, avi, m4a, ogg, flac, wav, webm, flv, 3gp, rm.
//
// If file has more than one audio stream u can select it explicitly.
// -> But you have to know file stream layout in advance for that.
// -> So u could open the file in advance with de::ffmpeg::File and inspect the streams before hand

// The BufferLoader always creates a ffmpeg instance that is independent -> thread safe.
// -> depends if ffmpeg can guarantee the same, but it should.
//
// This is the Demo class for converting any (ffmpeg) file into a benni audio buffer.
//
// It was meant to be somehow standalone, but uses de::ffmpeg::File as a serious dependency.
// So to not have to rewrite the mechanism again and again i rely on a shared tool.
//
// After that everybody can be king of multimedia. Image extraction is not wanted here.
// U can extract images again with the underlying used tool de::ffmpeg::File.

} // end namespace io
} // end namespace audio
} // end namespace de

#endif










