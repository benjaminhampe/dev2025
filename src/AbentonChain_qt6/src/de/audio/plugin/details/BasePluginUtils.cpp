#include <de/audio/plugin/details/BasePluginUtils.h>

namespace de {
namespace audio {

//===============================
void NormalizedSumComputer::calc(
            const float* __restrict__ L,
            const float* __restrict__ R,
            uint32_t blockSize)
//===============================
{
    if (blockSize < 1)
    {
        m_sumL = 0.0f;
        m_sumR = 0.0f;
        return;
    }

    if (m_blockSize != blockSize)
    {
        m_blockSize = blockSize;
        m_blockSizeInv = 1.0f / float(blockSize);
    }

#if 1

    float sumL = 0.0f;

    // #pragma omp parallel for reduction(+:sumL)
    for (size_t i = 0; i < blockSize; ++i)
    {
        sumL += ::de::absf(L[i]);
    }

    m_sumL = sumL * m_blockSizeInv; // Normalized

    float sumR = 0.0f;

    // #pragma omp parallel for reduction(+:sumR)
    for (size_t i = 0; i < blockSize; ++i)
    {
        sumR += ::de::absf(R[i]);
    }

    m_sumR = sumR * m_blockSizeInv; // Normalized

    //DE_TRACE("sumL = ", m_sumL)
    //DE_TRACE("sumR = ", m_sumR)

#else

    const float sumL = std::reduce(
                    std::execution::par_unseq,
                    L,
                    L + blockSize,
                    0.0f);

    m_sumL = sumL * m_blockSizeInv; // Normalized

    const float sumR = std::reduce(
                    std::execution::par_unseq,
                    R,
                    R + blockSize,
                    0.0f);

    m_sumR = sumR * m_blockSizeInv; // Normalized

#endif
}

} // end namespace audio.
} // end namespace de.

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace de {
namespace audio {

//===============================
SymbolLoader::SymbolLoader() : m_handle(0) {}

SymbolLoader::~SymbolLoader() { close(); }

bool SymbolLoader::is_open() const { return m_handle != 0; }

void SymbolLoader::open(std::string uri)
{
    if (m_handle)
    {
        DE_ERROR("Already open")
        return;
    }

    DE_TRACE("Open dll ",uri)
    #ifdef _WIN32
    HMODULE hDLL = LoadLibraryExA(
        uri.c_str(),
        NULL,
        LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
    );

    //HMODULE hDLL = LoadLibraryA( uri.c_str() );
    if ( hDLL )
    {
        DE_OK("Loaded module ",uri)
        m_handle = uint64_t(hDLL);
        DE_OK("Loaded module handle ",m_handle)
    }
    else
    {
        DE_ERROR("No HMODULE ",uri)
    }
    #else
    void* hSO = dlopen(uri.c_str(), RTLD_NOW);
    if ( !hSO )
    {
        DE_ERROR("No hSO ",uri)
        return;
    }
    m_handle = uint64_t(hSO);
    #endif
}

void SymbolLoader::close()
{
    if (!m_handle)
    {
        DE_ERROR("Module already closed.")
        return;
    }

    DE_OK("Close module handle ",m_handle)
    #ifdef _WIN32
    FreeLibrary((HMODULE)m_handle);
    #else
    dlclose((void*)m_handle);
    #endif
    m_handle = 0;
}

void* SymbolLoader::getSymbol(std::string symName)
{
    if (!m_handle)
    {
        DE_ERROR("Not open")
        return nullptr;
    }

    void* symAddr = nullptr;
    #ifdef _WIN32
    symAddr = (void*)GetProcAddress((HMODULE)m_handle, symName.c_str());
    #else
    symAddr = (void*)dlsym((void*)m_handle, symName.c_str());
    #endif
    if (!symAddr)
    {
        DE_ERROR("Not symbol ", symName)
        return nullptr;
    }

    return symAddr;
};

// =========================================
PluginTimer::PluginTimer()
// =========================================
{
    #ifdef _WIN32
    LARGE_INTEGER m_freq;
    QueryPerformanceFrequency(&m_freq);
    m_freqInv = 1.0 / (double)m_freq.QuadPart;
    #else
    DE_ERROR("Not implemented.")
    m_freqInv = 1.0;
    #endif
}

double PluginTimer::now() const
{
    #ifdef _WIN32
    LARGE_INTEGER a;
    QueryPerformanceCounter(&a);
    double seconds = double(a.QuadPart) * m_freqInv;
    #else
    DE_ERROR("Not implemented.")
    double seconds = 0.0f;
    #endif
    return seconds;
}

// =========================================
PluginClock::PluginClock()
// =========================================
{
    m_timeStart = m_timer.now();
}

void PluginClock::restart()
{
    m_timeStart = m_timer.now();
}

double PluginClock::now() const
{
    return m_timer.now() - m_timeStart;
}

} // end namespace audio.
} // end namespace de.
