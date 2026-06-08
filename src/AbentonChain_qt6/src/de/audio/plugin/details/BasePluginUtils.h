#pragma once
#include <DarkImage.h>
// We use openMP -fopenmp -O3 -march=native
#if 0
#include <vector>
#include <numeric>
#include <execution> // My clang/libc++ not compiled with par_unseq
#endif

namespace de {
namespace audio {

inline void fadeIn(float* __restrict__ buf, int blockSize)
{
    // if (!buf || blockSize <= 0) return;

    const float inv = 1.0f / float(blockSize - 1);
    for (int i = 0; i < blockSize; ++i)
    {
        const float g = i * inv; // 0 → 1
        buf[i] *= g;
    }
}

inline void fadeOut(float* __restrict__ buf, int blockSize)
{
    // if (!buf || blockSize <= 0) return;

    const float inv = 1.0f / float(blockSize - 1);
    for (int i = 0; i < blockSize; ++i)
    {
        const float g = 1.0f - (i * inv); // 1 → 0
        buf[i] *= g;
    }
}

//===============================
class NormalizedSumComputer
//===============================
{
    uint32_t m_blockSize = 0;
    float m_blockSizeInv = 1.0f;
public:
    float m_sumL = 0.0f;
    float m_sumR = 0.0f;

public:
    void calc(const float* __restrict__ L,
              const float* __restrict__ R,
              uint32_t blockSize);
};

//===============================
class SymbolLoader
//===============================
{
    uint64_t m_handle;
public:
    SymbolLoader();
    ~SymbolLoader();

    void open(std::string uri);
    void close();
    bool is_open() const;
    void* getSymbol(std::string symName);
};

//===============================
class PluginTimer
//===============================
{
public:
    PluginTimer();
    double now() const;

private:
    double m_freqInv;
};

//===============================
class PluginClock
//===============================
{
public:
    PluginClock();
    void restart();
    double now() const;

private:
    PluginTimer m_timer;
    double m_timeStart;
};

} // end namespace audio.
} // end namespace de.
