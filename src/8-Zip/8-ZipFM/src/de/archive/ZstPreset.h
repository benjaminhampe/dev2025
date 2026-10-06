#pragma once
#include <de/Core.h>

struct ZstUtil
{
    static std::string cpStr(int param);

    static std::string cpHelpStrII(int param);

    static std::string cpHelpStr(int param);

    static int32_t cpMin(int param);

    static int32_t cpMax(int param);
};

/*
    std::optional<int32_t> compressionLevel;
    std::optional<int32_t> windowLog;
    std::optional<int32_t> hashLog;
    std::optional<int32_t> chainLog;
    std::optional<int32_t> searchLog;
    std::optional<int32_t> minMatch;
    std::optional<int32_t> targetLength;
    std::optional<int32_t> strategy;
    std::optional<int32_t> targetCBlockSize;
    std::optional<int32_t> enableLongDistanceMatching;
    std::optional<int32_t> ldmHashLog;
    std::optional<int32_t> ldmMinMatch;
    std::optional<int32_t> ldmBucketSizeLog;
    std::optional<int32_t> ldmHashRateLog;
    std::optional<int32_t> contentSizeFlag;
    std::optional<int32_t> checksumFlag;
    std::optional<int32_t> dictIDFlag;
    std::optional<int32_t> nbWorkers;
    std::optional<int32_t> jobSize;
    std::optional<int32_t> overlapLog;
*/

// =====================================================
struct ZstPreset
// =====================================================
{
    // int id;
    std::string name;
    std::unordered_map<int32_t, int32_t> data;

    ZstPreset() = default;

    std::string str() const;

    static ZstPreset createDefault(int compressLevel = 19, std::string prefix = "Default");
};

// =====================================================
class ZstPresets
// =====================================================
{
    std::vector<ZstPreset> m_presets;
public:
    ZstPresets();

    int
    getDefaultIndex() const;

    int
    getPresetCount() const;

    const ZstPreset&
    getPreset(int i) const;

    static std::shared_ptr<ZstPresets>
    getInstance();
};

void applyCCtxPreset(void* cctx, int iPreset = -1);

#if 0
inline void ZstPresetDefaultOld(ZSTD_CCtx* cctx)
{
    int maxThreads = std::thread::hardware_concurrency();
    int compressionLevel = 19;
    int numberOfThreads = std::max<int>(1, maxThreads - 1);
    int jobSize = 2 * 1024 * 1024;
    int windowLog = 26; // 2^26 = 64MB window
    int longDistanceMatching = 1;

    // 1. Drop from level 22 to 19 (19 is the highest standard level)
    // Level 19 natively allows multi-threading without a master thread bottleneck.
    auto e = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, compressionLevel);
    if (ZSTD_isError(e))
    {
        DE_ERROR("ZSTD_c_compressionLevel: ", ZSTD_getErrorName(e))
    }

    // 2. Enable your 8 worker threads
    e = ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, numberOfThreads);
    if (ZSTD_isError(e))
    {
        DE_ERROR("ZSTD_c_nbWorkers: ", ZSTD_getErrorName(e))
    }

    // 3. FORCE smaller job sizes (e.g., 2MB chunks)
    // This overrides the massive default 19-level block and forces data
    // to be distributed to all 8 threads instantly.
    e = ZSTD_CCtx_setParameter(cctx, ZSTD_c_jobSize, jobSize);
    if (ZSTD_isError(e))
    {
        DE_ERROR("ZSTD_c_jobSize: ", ZSTD_getErrorName(e))
    }

    // 4. Force a matching window size (e.g., 64MB or 128MB)
    // This acts as a replacement for LDM, ensuring high compression ratios.
    e = ZSTD_CCtx_setParameter(cctx, ZSTD_c_windowLog, windowLog); // 2^26 = 64MB window
    if (ZSTD_isError(e))
    {
        DE_ERROR("ZSTD_c_windowLog: ", ZSTD_getErrorName(e))
    }

    // 5. Set maximum ultra compression level
    e = ZSTD_CCtx_setParameter(cctx, ZSTD_c_enableLongDistanceMatching, longDistanceMatching);
    if (ZSTD_isError(e))
    {
        DE_ERROR("ZSTD_c_enableLongDistanceMatching: ", ZSTD_getErrorName(e))
    }

    // 6. Set the LDM window size to maintain high compression ratio (e.g., 128MB)
    // e = ZSTD_CCtx_setParameter(cctx, ZSTD_c_ldmWindowLog, 27);
    // if (ZSTD_isError(e))
    // {
    //     DE_ERROR("ZSTD_c_compressionLevel: ", ZSTD_getErrorName(e))
    // }

    DE_TRACE("maxThreads = ", maxThreads)
    DE_TRACE("ZSTD_c_nbWorkers = ", numberOfThreads)
    DE_TRACE("ZSTD_c_compressionLevel = ", compressionLevel)
    DE_TRACE("ZSTD_c_jobSize = ", jobSize)
    DE_TRACE("ZSTD_c_windowLog = ", windowLog)
    DE_TRACE("ZSTD_c_enableLongDistanceMatching = ", longDistanceMatching)
}

/*
ZSTD_c_compressionLevel	            22	                Enables Ultra level 22, the maximum native preset.
ZSTD_c_enableLongDistanceMatching	1	Turn on LDM to catch repetitive patterns over immense spans.
ZSTD_c_windowLog	                31	Sets the history window to 2³¹ (2 GB), maximizing back-references.
ZSTD_c_hashLog	                    30	Main hash table size (2³⁰ entries).
ZSTD_c_chainLog	                    30	Match chain size (2³⁰ entries).
ZSTD_c_searchLog	                29	Search step capacity limit (2²⁹ operations).
ZSTD_c_targetLength	                9999	Forces deep block matches instead of fast cut-offs.
ZSTD_c_strategy	                    ZSTD_btultra2	Utilizes the most thorough algorithmic parse engine.
ZSTD_c_nbWorkers	                14	Spawns exactly 14 worker threads.
ZSTD_c_overlapLog	                9	Forces a full window size overlap from previous jobs.
*/

inline void setParam(ZSTD_CCtx* cctx, std::string name, int param, int value)
{
    int def_val = 0;
    ZSTD_CCtx_getParameter(cctx, (ZSTD_cParameter)param, &def_val);

    // Limits abfragen
    ZSTD_bounds bounds = ZSTD_cParam_getBounds((ZSTD_cParameter)param);

    // Immer zuerst auf Fehler prüfen!
    if (ZSTD_isError(bounds.error))
    {
        DE_ERROR("[",name,"] No bounds: ", ZSTD_getErrorName(bounds.error));
    }

    DE_BENNI("[",name,"] "
        "Value(",value,"), "
        "Default(",def_val,"), "
        "Min(",bounds.lowerBound,"),"
        "Max(",bounds.upperBound,")")

    // 1. Drop from level 22 to 19 (19 is the highest standard level)
    // Level 19 natively allows multi-threading without a master thread bottleneck.
    auto e = ZSTD_CCtx_setParameter(cctx, (ZSTD_cParameter)param, value);
    if (ZSTD_isError(e))
    {
        DE_ERROR("[",name,"] No setParameter: ", ZSTD_getErrorName(e))
    }
};

inline void ZstPresetUltraBad(ZSTD_CCtx* cctx)
{
    setParam(cctx,"ZSTD_c_compressionLevel",ZSTD_c_compressionLevel,22); //Enables Ultra level 22, the maximum native preset.
    setParam(cctx,"ZSTD_c_enableLongDistanceMatching",ZSTD_c_enableLongDistanceMatching,1); //Turn on LDM to catch repetitive patterns over immense spans.
    setParam(cctx,"ZSTD_c_windowLog",ZSTD_c_windowLog,31); //Sets the history window to 2³¹ (2 GB), maximizing back-references.
    setParam(cctx,"ZSTD_c_hashLog",ZSTD_c_hashLog,30); //Main hash table size (2³⁰ entries).
    setParam(cctx,"ZSTD_c_chainLog",ZSTD_c_chainLog,30); //Match chain size (2³⁰ entries).
    setParam(cctx,"ZSTD_c_searchLog",ZSTD_c_searchLog,29); //Search step capacity limit (2²⁹ operations).
    setParam(cctx,"ZSTD_c_targetLength",ZSTD_c_targetLength,9999); //Forces deep block matches instead of fast cut-offs.
    setParam(cctx,"ZSTD_c_strategy",ZSTD_c_strategy,ZSTD_btultra2); //Utilizes the most thorough algorithmic parse engine.
    setParam(cctx,"ZSTD_c_nbWorkers",ZSTD_c_nbWorkers,12); //Spawns exactly 14 worker threads.
    setParam(cctx,"ZSTD_c_overlapLog",ZSTD_c_overlapLog,9); //Forces a full window size overlap from previous jobs.
}

inline void ZstPresetUltra(ZSTD_CCtx* cctx)
{
    setParam(cctx,"ZSTD_c_compressionLevel",ZSTD_c_compressionLevel,22); //Enables Ultra level 22, the maximum native preset.
    //setParam(cctx,"ZSTD_c_strategy",ZSTD_c_strategy,ZSTD_btultra2); //Utilizes the most thorough algorithmic parse engine.
    setParam(cctx,"ZSTD_c_enableLongDistanceMatching",ZSTD_c_enableLongDistanceMatching,2); //Turn on LDM to catch repetitive patterns over immense spans.
    setParam(cctx,"ZSTD_c_windowLog",ZSTD_c_windowLog,30); //Sets the history window to 2³¹ (2 GB), maximizing back-references.
    setParam(cctx,"ZSTD_c_hashLog",ZSTD_c_hashLog,25); //Main hash table size (2³⁰ entries).
    setParam(cctx,"ZSTD_c_chainLog",ZSTD_c_chainLog,25); //Match chain size (2³⁰ entries).
    setParam(cctx,"ZSTD_c_searchLog",ZSTD_c_searchLog,30); //Search step capacity limit (2²⁹ operations).
    setParam(cctx,"ZSTD_c_targetLength",ZSTD_c_targetLength,131072); //Forces deep block matches instead of fast cut-offs.
    setParam(cctx,"ZSTD_c_nbWorkers",ZSTD_c_nbWorkers,14); //Spawns exactly 14 worker threads.
    setParam(cctx,"ZSTD_c_overlapLog",ZSTD_c_overlapLog,9); //Forces a full window size overlap from previous jobs.
    setParam(cctx,"ZSTD_c_jobSize",ZSTD_c_jobSize,2 * 1024 * 1024);
}

inline void ZstPresetDefault(ZSTD_CCtx* cctx)
{
    setParam(cctx,"ZSTD_c_compressionLevel",ZSTD_c_compressionLevel,19); //Enables Ultra level 22, the maximum native preset.
    setParam(cctx,"ZSTD_c_nbWorkers",ZSTD_c_nbWorkers,13); //Spawns exactly 14 worker threads.
    setParam(cctx,"ZSTD_c_jobSize",ZSTD_c_jobSize,2 * 1024 * 1024);
    setParam(cctx,"ZSTD_c_windowLog",ZSTD_c_windowLog,26); //Sets the history window to 2³¹ (2 GB), maximizing back-references.
    setParam(cctx,"ZSTD_c_enableLongDistanceMatching",ZSTD_c_enableLongDistanceMatching,1); //Turn on LDM to catch repetitive patterns over immense spans.

    //setParam(cctx,"ZSTD_c_strategy",ZSTD_c_strategy,ZSTD_btultra2); //Utilizes the most thorough algorithmic parse engine.
    // setParam(cctx,"ZSTD_c_hashLog",ZSTD_c_hashLog,25); //Main hash table size (2³⁰ entries).
    // setParam(cctx,"ZSTD_c_chainLog",ZSTD_c_chainLog,25); //Match chain size (2³⁰ entries).
    // setParam(cctx,"ZSTD_c_searchLog",ZSTD_c_searchLog,30); //Search step capacity limit (2²⁹ operations).
    // setParam(cctx,"ZSTD_c_targetLength",ZSTD_c_targetLength,131072); //Forces deep block matches instead of fast cut-offs.
    // setParam(cctx,"ZSTD_c_overlapLog",ZSTD_c_overlapLog,9); //Forces a full window size overlap from previous jobs.
}


inline void ZstPresetExtreme(ZSTD_CCtx* cctx)
{
    setParam(cctx,"ZSTD_c_compressionLevel",ZSTD_c_compressionLevel,19); //Enables Ultra level 22, the maximum native preset.
    setParam(cctx,"ZSTD_c_nbWorkers",ZSTD_c_nbWorkers,13); //Spawns exactly 14 worker threads.
    setParam(cctx,"ZSTD_c_jobSize",ZSTD_c_jobSize,2 * 1024 * 1024);
    setParam(cctx,"ZSTD_c_windowLog",ZSTD_c_windowLog,26); //Sets the history window to 2³¹ (2 GB), maximizing back-references.
    setParam(cctx,"ZSTD_c_enableLongDistanceMatching",ZSTD_c_enableLongDistanceMatching,1); //Turn on LDM to catch repetitive patterns over immense spans.

    //setParam(cctx,"ZSTD_c_strategy",ZSTD_c_strategy,ZSTD_btultra2); //Utilizes the most thorough algorithmic parse engine.
    // setParam(cctx,"ZSTD_c_hashLog",ZSTD_c_hashLog,25); //Main hash table size (2³⁰ entries).
    // setParam(cctx,"ZSTD_c_chainLog",ZSTD_c_chainLog,25); //Match chain size (2³⁰ entries).
    // setParam(cctx,"ZSTD_c_searchLog",ZSTD_c_searchLog,30); //Search step capacity limit (2²⁹ operations).
    // setParam(cctx,"ZSTD_c_targetLength",ZSTD_c_targetLength,131072); //Forces deep block matches instead of fast cut-offs.
    // setParam(cctx,"ZSTD_c_overlapLog",ZSTD_c_overlapLog,9); //Forces a full window size overlap from previous jobs.
}

// =====================================================
struct ZstPreset
// =====================================================
{
    int algo;
    int level;

    std::string name;

    ZstPreset()
        : algo{ 0 }
        , level{ 3 }
        , name{ "Default" }
    {}

    ZstPreset(int algorithm, int compressLevel, std::string dispName)
        : algo{ algorithm }
        , level{ compressLevel }
        , name{ dispName }
    {}
};

// =====================================================
struct ZstPresets
// =====================================================
{
    static int
    getDefault() { return 4; }

    static const std::array<ZstPreset,36>
    get()
    {
        static const std::array<ZstPreset,36> presets
        {{
            { 0, 0, "0 - No compression"},
            // { ZSTD_fast, 0, "Fast -1000 (highest throughput)"},
            // { ZSTD_fast, 0, "Fast -500 (ultra throughput)"},
            // { ZSTD_fast, 0, "Fast -400 (ultra throughput)"},
            // { ZSTD_fast, 0, "Fast -300 (ultra throughput)"},
            // { ZSTD_fast, 0, "Fast -200 (higher throughput)"},
            // { ZSTD_fast, 0, "Fast -100 (high throughput)"},
            // { ZSTD_fast, 0, "Fast -50 (logs/telemetry)"},
            // { ZSTD_fast, 0, "Fast -30 (super fast)"},
            // { ZSTD_fast, 0, "Fast -20 (extremely fast)"},
            // { ZSTD_fast, 0, "Fast -10 (very fast)"},
            // { ZSTD_fast, 0, "Fast -5 (fast)"},
            // { ZSTD_fast, 0, "Fast -3 (Standard‑Fast)"},
            // { ZSTD_fast, 0, "Fast -1 (a little faster)"},
            { 0, 1, "1 - very fast    - ZSTD_fast"},
            { 0, 2, "2 - fast         - ZSTD_fast"},
            { 0, 3, "3 - (default)    - ZSTD_dfast"},
            { 0, 4, "4 - better ratio - ZSTD_dfast"},
            { 0, 5, "5 - medium ratio - ZSTD_greedy"},
            { 0, 6, "6 - higher ratio - ZSTD_lazy"},
            { 0, 7, "7 - higher ratio - ZSTD_lazy"},
            { 0, 8, "8 - high ratio   - ZSTD_lazy2"},
            { 0, 9, "9 - high ratio   - ZSTD_lazy2"},
            { 0, 10, "10 - very high ratio - ZSTD_lazy2"},
            { 0, 11, "11 - very high ratio - ZSTD_lazy2"},
            { 0, 12, "12 - very high ratio - ZSTD_lazy2"},
            { 0, 13, "13 - super high ratio - ZSTD_btlazy2"},
            { 0, 14, "14 - super high ratio - ZSTD_btlazy2"},
            { 0, 15, "15 - super high ratio - ZSTD_btlazy2"},
            { 0, 16, "16 - maximal - ZSTD_btopt"},
            { 0, 17, "17 - maximal - ZSTD_btopt"},
            { 0, 18, "18 - maximal - ZSTD_btopt"},
            { 0, 19, "19 - maximal - ZSTD_btopt"},
            { 0, 20, "20 - ultra - ZSTD_btultra"},
            { 0, 21, "21 - ultra - ZSTD_btultra"},
            { 0, 22, "22 - ultra - ZSTD_btultra"}
        }};

        return presets;
    }

};

#endif
