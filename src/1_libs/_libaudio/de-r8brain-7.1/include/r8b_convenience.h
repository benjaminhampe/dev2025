#pragma once
#include "r8bbase.h"
#include "CDSPResampler.h"
#include <vector>
#include <cmath>

namespace r8b {

enum class ResampleQuality {
    Low,
    Medium,
    High,
    Ultra
};

struct QualityParams {
    double transBand;
    double atten;
    EDSPFilterPhaseResponse phase;
};

inline QualityParams getQualityParams(ResampleQuality q) {
    switch (q) {
        case ResampleQuality::Low:
            return { 8.0, 80.0, fprMinimumPhase };
        case ResampleQuality::Medium:
            return { 4.0, 120.0, fprLinearPhase };
        case ResampleQuality::High:
            return { 2.0, 206.91, fprLinearPhase };
        case ResampleQuality::Ultra:
            return { 1.0, 240.0, fprLinearPhase };
    }
    return { 2.0, 206.91, fprLinearPhase };
}

class Resampler {
public:
    Resampler(int channels,
              double srcRate,
              double dstRate,
              ResampleQuality quality,
              int maxBlockSize)
        : m_channels(channels),
          m_srcRate(srcRate),
          m_dstRate(dstRate),
          m_ratio(dstRate / srcRate)
    {
        auto qp = getQualityParams(quality);

        m_resamplers.reserve(channels);
        for (int ch = 0; ch < channels; ch++) {
            m_resamplers.emplace_back(
                srcRate,
                dstRate,
                maxBlockSize,
                qp.transBand,
                qp.atten,
                qp.phase
            );
        }
    }

    int process(const float* const* in, int inFrames, float** out) {
        int maxOutFrames = int(std::ceil(inFrames * m_ratio)) + 16;

        for (int ch = 0; ch < m_channels; ch++) {
            int written = m_resamplers[ch].process(
                in[ch],
                inFrames,
                out[ch]
            );
            m_lastOutFrames = written;
        }

        return m_lastOutFrames;
    }

    int getLastOutFrames() const { return m_lastOutFrames; }

private:
    int m_channels;
    double m_srcRate;
    double m_dstRate;
    double m_ratio;

    int m_lastOutFrames = 0;

    std::vector<CDSPResampler> m_resamplers;
};

} // namespace r8b
