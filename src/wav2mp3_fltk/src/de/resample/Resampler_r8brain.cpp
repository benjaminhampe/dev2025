#include <de/resample/Resampler_r8brain.h>

#include <de/sound/SoundUtil.h>

#include "CDSPResampler.h"

namespace de {
namespace sound {

struct Resampler_r8brain::Impl
{
    int32_t m_channels = 0;
    int32_t m_srcRate = 0;
    int32_t m_dstRate = 0;
    int32_t m_chunkSize = 0;

    std::vector<std::unique_ptr<r8b::CDSPResampler>> m_resamplers;

    Sound m_inBuffer1;  // Has src chunk of ST_SRC + interleaved, after copy.
    Sound m_inBuffer2;  // Has src chunk of ST_F64 + interleaved, after copy + convert.
    Sound m_inBuffer3;  // Has src chunk of ST_F64 + planar, after copy + convert + deinterleave.
    Sound m_outBuffer1; // Has out chunk of ST_F64 planar, after resampler.
    Sound m_outBuffer2; // Has out chunk of ST_SRC planar, after resampler + convert.
    Sound m_outBuffer3; // Has out chunk of ST_SRC interleaved, after resampler + convert + interleave.

    bool setup(int32_t channels, int32_t srcRate, int32_t dstRate, int32_t chunkSize)
    {
        m_channels = channels;
        m_srcRate = srcRate;
        m_dstRate = dstRate;
        m_chunkSize = chunkSize;

        m_resamplers.clear();
        m_resamplers.reserve(m_channels);

        for (int ch = 0; ch < m_channels; ch++)
        {
            m_resamplers.emplace_back(
                std::make_unique<r8b::CDSPResampler>(
                    m_srcRate,
                    m_dstRate,
                    m_chunkSize,      // maxBlockSize == chunkSize
                    2.0,
                    206.91,
                    r8b::fprLinearPhase
                )
            );
        }

        return true;
    }

    bool resample(const Sound & src, Sound & dst, int32_t dstRate)
    {
        setup(  src.m_channels,
                src.m_sampleRate,
                dstRate,
                1024);

        dst.m_channels   = src.m_channels;
        dst.m_sampleType = src.m_sampleType;
        dst.m_sampleRate = dstRate;
        dst.m_samples.clear();

        const double ratio       = double(m_dstRate) / double(m_srcRate);
        const int64_t inFramesTotal = src.m_frames;
        int64_t outFramesTotal = 0;

        int64_t pos = 0;
        while (pos < inFramesTotal)
        {
            int64_t inFrames = std::min(int64_t(m_chunkSize), inFramesTotal - pos);

            // Copy: ... chunk of inFrames
            int64_t in1 = SoundUtil::copy(src, m_inBuffer1, inFrames, pos);

            // Convert: ... ST_F64
            int64_t in2 = SoundUtil::convert(m_inBuffer1, m_inBuffer2, SampleType::F64);

            // Deinterleave: ... Make planar
            int64_t in3 = SoundUtil::deinterleave(m_inBuffer2, m_inBuffer3);

            // Resample: ... Each channel has own resampler
            int64_t outFrames = 0;

            for (int32_t ch = 0; ch < m_channels; ch++)
            {
                // Move to first planar sample byte in m_inBuffer2.
                double* __restrict__ inPtr =
                    reinterpret_cast<double*>(m_inBuffer3.data() + inFrames * ch * int32_t(sizeof(double)));

                double* outPtr = nullptr;

                outFrames = m_resamplers[ch]->process(
                    inPtr,      // double*
                    inFrames,   // <= maxBlockSize
                    outPtr      // double*& (r8brain sets this)
                );

                if (outFrames > 0)
                {
                    m_outBuffer1.m_flags = 1; // Planar flag.
                    m_outBuffer1.m_sampleType = SampleType::F64;
                    m_outBuffer1.m_sampleRate = src.m_sampleRate;
                    m_outBuffer1.m_channels = m_channels;
                    m_outBuffer1.allocFrames( outFrames );
                    //m_outBuffer1.resize( outFrames * m_channels * sizeof(double) );

                    // Move to first planar sample byte in m_outBuffer1.
                    double* __restrict__ outPtr1 =
                       reinterpret_cast<double*>(m_outBuffer1.data() + outFrames * ch * int32_t(sizeof(double)));

                    // Copy planar channel to planar multichannel
                    for (int i = 0; i < outFrames; i++)
                    {
                        outPtr1[i] = outPtr[i];
                    }
                }
            }

            // Deinterleave + Convert to dstSampleType
            if (outFrames > 0)
            {
                // Interleave
                auto o1 = SoundUtil::interleave( m_outBuffer1, m_outBuffer2 );

                // Convert ST_SRC
                auto o2 = SoundUtil::convert( m_outBuffer2, m_outBuffer3, src.sampleType() );

                // Append to dst
                dst.append( m_outBuffer3 );

                outFramesTotal += outFrames;
            }

            pos += inFrames;
        }

        // dst.m_frames = outFramesTotal;
        dst.validate();
        return true;
    }

};

Resampler_r8brain::Resampler_r8brain()
    : _d( new Resampler_r8brain::Impl )
{
}

Resampler_r8brain::~Resampler_r8brain()
{
    delete _d;
}

bool Resampler_r8brain::resample(const Sound & src,
                                    Sound & dst, int32_t dstRate)
{
    return _d->resample(src,dst,dstRate);
}

} // end namespace sound.
} // end namespace de.


/*

class Resampler_r8brain
{
public:
    Resampler_r8brain();
    ~Resampler_r8brain();
    bool resample(const Sound & src, Sound & dst, int32_t dstRate);

private:
    struct Impl;
    Impl* _d = nullptr;
};

struct Resampler_r8brain::Impl
{
    int32_t m_channels = 0;
    int32_t m_srcRate = 0;
    int32_t m_dstRate = 0;
    int32_t m_chunkSize = 0;

    std::vector<std::unique_ptr<r8b::CDSPResampler>> m_resamplers;

    TAlignedVector<uint8_t> m_inBuffer1; // src any interleaved -> src f64 interleaved
    TAlignedVector<uint8_t> m_inBuffer2;  // src f64 interleaved -> src f64 planar
    TAlignedVector<uint8_t> m_outBuffer1; // resampler f64 planar multi-channel output
    TAlignedVector<uint8_t> m_outBuffer2; // resampler any planar multi-channel output
    TAlignedVector<uint8_t> m_outBuffer3; // resampler any interleaved multi-channel output

    bool setup(int32_t channels, int32_t srcRate, int32_t dstRate, int32_t chunkSize)
    {
        m_channels = channels;
        m_srcRate = srcRate;
        m_dstRate = dstRate;
        m_chunkSize = chunkSize;

        m_resamplers.clear();
        m_resamplers.reserve(m_channels);

        for (int ch = 0; ch < m_channels; ch++)
        {
            m_resamplers.emplace_back(
                std::make_unique<r8b::CDSPResampler>(
                    m_srcRate,
                    m_dstRate,
                    m_chunkSize,      // maxBlockSize == chunkSize
                    2.0,
                    206.91,
                    r8b::fprLinearPhase
                )
            );
        }

        return true;
    }

    bool resample(const Sound & src, Sound & dst, int32_t dstRate)
    {
        setup(  src.m_channels,
                src.m_sampleRate,
                dstRate,
                1024);

        dst.m_channels   = src.m_channels;
        dst.m_sampleType = src.m_sampleType;
        dst.m_sampleRate = dstRate;
        dst.m_samples.clear();

        const double ratio       = double(m_dstRate) / double(m_srcRate);
        const int64_t inFramesTotal = src.m_frames;
        int64_t outFramesTotal = 0;

        int64_t pos = 0;
        while (pos < inFramesTotal)
        {
            int64_t inFrames = std::min(int64_t(m_chunkSize), inFramesTotal - pos);

            // std::vector<std::vector<double>> inCh;
            // deinterleave_float_to_double(in, pos, frames, inCh);

            // Convert to double + deinterleave
            Sound::deinterleave(
                src.m_channels,
                src.m_sampleType,
                src.m_samples,
                m_inBuffer1,
                SampleType::F64,
                m_inBuffer2,
                inFrames,
                pos);

            // int outCap = int(std::ceil(frames * ratio)) + 16;
            // std::vector<std::vector<double>> outCh(chs, std::vector<double>(outCap));

            int64_t outFrames = 0;

            for (int32_t ch = 0; ch < m_channels; ch++)
            {
                // Move to first planar sample byte in m_inBuffer2.
                double* __restrict__ inPtr =
                    reinterpret_cast<double*>(m_inBuffer2.data() + inFrames * ch * int32_t(sizeof(double)));

                double* outPtr = nullptr;

                outFrames = m_resamplers[ch]->process(
                    inPtr,      // double*
                    inFrames,   // <= maxBlockSize
                    outPtr      // double*& (r8brain sets this)
                );

                if (outFrames > 0)
                {
                    m_outBuffer1.resize( outFrames * m_channels * sizeof(double) );

                    // Move to first planar sample byte in m_outBuffer1.
                    double* __restrict__ outPtr1 =
                       reinterpret_cast<double*>(m_outBuffer1.data() + outFrames * ch * int32_t(sizeof(double)));

                    // Copy planar channel to planar multichannel
                    for (int i = 0; i < outFrames; i++)
                    {
                        outPtr1[i] = outPtr[i];
                    }
                }
            }

            // Deinterleave + Convert to dstSampleType
            if (outFrames > 0)
            {
                Sound::deinterleave(
                    src.m_channels,
                    SampleType::F64,
                    m_outBuffer1,
                    m_outBuffer2,
                    src.m_sampleType,
                    m_outBuffer3,
                    outFrames,
                    0);

                // Append to dst
                dst.m_samples.insert(
                    dst.m_samples.end(),
                    m_outBuffer3.data(),
                    m_outBuffer3.data() + m_outBuffer3.size());

                outFramesTotal += outFrames;
            }

            pos += inFrames;
        }

        dst.m_frames = outFramesTotal;
        dst.validate();
        return true;
    }

};

Resampler_r8brain::Resampler_r8brain()
    : _d( new Resampler_r8brain::Impl )
{
}

Resampler_r8brain::~Resampler_r8brain()
{
    delete _d;
}

bool Resampler_r8brain::resample(const Sound & src,
                                    Sound & dst, int32_t dstRate)
{
    return _d->resample(src,dst,dstRate);
}

*/
