#pragma once
#include <de/de_aligned_memory.h>
//#include <cstdint>
#include <cstring> // for memcpy
//#include <vector>
//#include <atomic>

namespace de {
namespace audio {

class AlignedAccumBuffer
{
private:
    u32 m_samples;
    u32 m_frames;
    u32 m_channels;
    u32 m_framesAvail;
	float* m_buffer;
    TAlignedVector<float> m_front; // AVX2 aligned 32 bytes.
	TAlignedVector<float> m_back; // AVX2 aligned 32 bytes.

public:
    AlignedAccumBuffer()
        : m_samples{ 0 }
        , m_frames{ 0 }
        , m_channels{ 0 }
        , m_framesAvail{ 0 }
		, m_buffer{ nullptr }
    {}

    // ~AlignedAccumBuffer() {}

    void resize(u32 frames, u32 channels)
    {
        if ((frames == m_frames) && (channels == m_channels))
        {
            return; // Nothing todo.
        }

        m_samples = static_cast<size_t>(frames) * channels;
        m_frames = frames;
        m_channels = channels;
        m_framesAvail = 0;
        m_front.resize(m_samples);
		m_back.resize(m_samples);
		m_buffer = m_front.data();
        DE_OK("frames(",m_frames,"), channels(",m_channels,")")
    }
    
	// How many frames can be written without overwriting unread data
    u32 getAvailFrames() const
    {
        return m_framesAvail;
    }

    // Produce interleaved frames: input samples = frames * channels
    void produce(const float* __restrict__ input, u32 frames)
    {
        if (frames < 1)
        {
            DE_ERROR("")
            return;
        }

		if (frames + m_framesAvail > m_frames)
		{
			DE_ERROR("")
			return;
		}

		std::memcpy(
            m_buffer + (size_t)m_framesAvail * m_channels,
            input,
            (size_t)frames * m_channels * sizeof(float));

		m_framesAvail += frames;

        // DE_TRACE("frames(",frames,"), framesAvail(",m_framesAvail,")")
    }

    // Consume interleaved frames: output samples = frames * channels
    void consume(float* __restrict__ output, u32 frames)
    {
        if (frames < 1)
            return;

		if (frames > m_frames)
		{
			DE_ERROR("")
			return;
		}

        if (frames > m_framesAvail)
		{
            DE_ERROR("frames(",frames,") >= framesAvail(",m_framesAvail,")")
			return;
		}
		
		size_t firstPart = frames * m_channels;
		size_t secondPart = (m_frames - frames) * m_channels;
		
        std::memcpy(output, m_buffer, firstPart * sizeof(float));
			
		if (m_buffer == m_front.data())
		{
			std::memcpy(m_back.data(), 
				m_front.data() + firstPart, 
				(size_t)secondPart * sizeof(float));
			
			m_buffer = m_back.data();
		}
		else
		{
			std::memcpy(m_front.data(), 
				m_back.data() + firstPart, 
				(size_t)secondPart * sizeof(float));
			
			m_buffer = m_front.data();
		}
		
		m_framesAvail -= frames;
    }
};

} // end namespace audio.
} // end namespace de.
