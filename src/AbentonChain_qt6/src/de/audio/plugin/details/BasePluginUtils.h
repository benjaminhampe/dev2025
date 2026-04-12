#pragma once
#include <cstdint>
#include <sstream>

namespace de {
namespace audio {

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
