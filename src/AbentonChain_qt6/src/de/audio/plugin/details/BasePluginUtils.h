#pragma once
#include <cstdint>
#include <sstream>

// We use openMP -fopenmp -O3 -march=native
#if 0
#include <vector>
#include <numeric>
#include <execution> // My clang/libc++ not compiled with par_unseq
#endif

namespace de {
namespace audio {

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
