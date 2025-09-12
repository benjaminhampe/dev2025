#pragma once
#include <de/Core.h>

extern "C"
{
   #include <lame/lame.h>
}

namespace de {
namespace audio {
namespace encoder {

struct EncoderLAME
{
   DE_CREATE_LOGGER("de.audio.EncoderLAME")
   bool m_isOpen;
   bool m_isDebug;
   lame_global_flags* m_lame;

   uint32_t m_kilobitRate;  // kbit/sec, use 64,96,128,160,[192],256 or 320
   uint32_t m_channelCount; // max 2 channels?
   uint32_t m_sampleRate;   // does not resample.

   int64_t m_frameCount;
   int64_t m_byteCount;
   int64_t m_byteCount2;
   int64_t m_chunkCount;
   int64_t m_chunkFrames;
   int64_t m_chunkSamples;
   int64_t m_chunkBytes;
   FILE* m_file;

   std::string m_uri;

   //Buffer convBuf( ST_F32I, 2, src.getSampleRate(), chunkFrames );
   std::vector< uint8_t > m_fileBuf;

   EncoderLAME();
   ~EncoderLAME();
   void close();
   bool open( std::string uri, uint32_t srcChannels, uint32_t srcRate, uint32_t kiloBitRate = 192 );
   bool writeSamples( float const* src, uint64_t srcFrames );

};

} // end namespace encoder
} // end namespace audio
} // end namespace de

