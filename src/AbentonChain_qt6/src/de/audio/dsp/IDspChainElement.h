#pragma once
#include <de/audio/dsp/DspUtils.h>

namespace de {
namespace audio {

/*
 *  TODO: Remove virtual in HOT-PATH:
 *
 *
struct IDspChainElement
{
    using InitFn =
        void(*)(IDspChainElement*, u64, u32, u32);

    using ReadFn =
        void(*)(IDspChainElement*, f64, u32, u32, f32*, f32*);

    InitFn init_fn = nullptr;
    ReadFn read_fn = nullptr;

    // HOTPATH → nicht virtuell
    void dsp_init(u64 frames, u32 channels, u32 sampleRate)
    {
        init_fn(this, frames, channels, sampleRate);
    }

    // HOTPATH → nicht virtuell
    void dsp_read(f64 pts, u32 frames, u32 sampleRate,
                  f32* L, f32* R)
    {
        read_fn(this, pts, frames, sampleRate, L, R);
    }

    virtual ~IDspChainElement() {}

    // Graph bleibt virtuell
    virtual std::string dsp_name() const = 0;
    virtual u32 dsp_getInputSignalCount() const = 0;
    virtual IDspChainElement* dsp_getInputSignal(int i = 0) = 0;
    virtual void dsp_setInputSignal(IDspChainElement* input, int i = 0) = 0;
    virtual void dsp_clearInputSignals() = 0;
};

🧱 Derived‑Klasse mit statischen Memberfunktionen
cpp

struct Gain : IDspChainElement
{
    float gain = 1.0f;

    // statische Implementierungen
    static void dsp_init_impl(IDspChainElement* self,
                              u64 frames, u32 channels, u32 sampleRate)
    {
        auto* g = static_cast<Gain*>(self);
        g->gain = 0.5f;
    }

    static void dsp_read_impl(IDspChainElement* self,
                              f64 pts, u32 frames, u32 sampleRate,
                              f32* L, f32* R)
    {
        auto* g = static_cast<Gain*>(self);

        for (u32 i = 0; i < frames; ++i)
        {
            L[i] *= g->gain;
            R[i] *= g->gain;
        }
    }

    Gain()
    {
        init_fn = &Gain::dsp_init_impl;
        read_fn = &Gain::dsp_read_impl;
    }

    std::string dsp_name() const override { return "Gain"; }

    u32 dsp_getInputSignalCount() const override { return 0; }
    IDspChainElement* dsp_getInputSignal(int) override { return nullptr; }
    void dsp_setInputSignal(IDspChainElement*, int) override {}
    void dsp_clearInputSignals() override {}
};

*/


// ============================
struct DspInitParams // 16 bytes
// ============================
{
    u64 frames = 0;
    u32 channels = 0;
    u32 sampleRate = 0;
};

// ============================
struct AutomationPoint
// ============================
{
    int64_t framePos = 0;
    double value = 0.0;
};

typedef uint32_t ParamID;

// ============================
struct AutomationCurve
// ============================
{
    ParamID paramId;
    std::vector<AutomationPoint> points;

    explicit AutomationCurve( ParamID id ) : paramId(id)
    {
    }
};

// ============================
struct AutomationQueue
// ============================
{
    std::vector<AutomationCurve> curves;

    AutomationQueue()
    {
        curves.reserve(32);
    }

    AutomationCurve& get(ParamID paramId)
    {
        auto it = std::find_if(curves.begin(),curves.end(),[&](const AutomationCurve& curve)
            { return curve.paramId == paramId; });

        if (it == curves.end())
        {
            curves.emplace_back(paramId);
            return curves.back();
        }

        return *it;
    }

    // std::unique_lock< std::mutex >
    // lock() const
    // {
    //     return std::unique_lock<std::mutex>(m_mutex);
    // }

private:
    // std::mutex mutable m_mutex;

};

// ============================
enum class eTransport : uint8_t
// ============================
{
    None = 0,
    Playing = 1,
    Recording = 2,
    Looping = 4,
    Reserved0 = 8,
    Reserved1 = 16,
};

// ============================
struct DspReadParams // 64 bytes
// ============================
{
    // --- 16 Bytes ---
    uint64_t framePos;
    uint32_t blockSize;
    uint32_t sampleRate;
    // f64 pts = double(framePos) / double(sampleRate);

    // --- 16 Bytes --- TransportFlags / Tempo / Beat / PPQ ---
    uint8_t channels;   // 2...8
    uint8_t flags;      // eTransport::<Playing|Looping|Recording>
    uint8_t timeSigNum; // 4
    uint8_t timeSigDen; // 4
    float bpm;          // 120.0f etc.

    // --- 16 Bytes
    uint64_t loopBeg;   // loop start frame.
    uint64_t loopEnd;   // loop end frame.

    // --- 16 Bytes
    double ppqPosition;         // fractional beat position
    double ppqPerSample;        // (bpm / 60) / sampleRateOut

    // --- Automation (sample-genau)
    //AutomationQueue* automation; // pointer auf host queue

    // --- Globale Parameter
    //double globalTempoFactor;   // z.B. für Time-Stretch
    //double globalPitch;         // semitones

    //f32* outputs[8]; // 32 bytes
    //f32* __restrict__ L;
    //f32* __restrict__ R;
};

// ============================
struct IDspChainElement
// ============================
{
    virtual ~IDspChainElement() {}

    virtual std::string dsp_name() const = 0;

    virtual void dsp_init(u64 frames, u32 channels, u32 sampleRate) = 0;

    virtual void dsp_read(f64 pts, u32 frames, u32 sampleRate,
                          f32* __restrict__ L,
                          f32* __restrict__ R ) = 0;

    virtual u32 dsp_getInputSignalCount() const = 0;

    virtual IDspChainElement* dsp_getInputSignal(int i = 0) = 0;

    virtual void dsp_setInputSignal(IDspChainElement* input, int i = 0) = 0;

    virtual void dsp_clearInputSignals() = 0;
};

// ============================
struct SampleMinMax
// ============================
{
/*
    // ==============================================
    // Feed (L+R) AudioLevelMeter
    // ==============================================
    m_Lmin = std::numeric_limits< float >::max();
    m_Lmax = std::numeric_limits< float >::lowest();
    m_Rmin = std::numeric_limits< float >::max();
    m_Rmax = std::numeric_limits< float >::lowest();
    if ( dstChannels == 2 )   // Fast interleaved stereo path O(1) = one loop over samples
    {
        float const* pSrc = dst;
        for ( uint64_t i = 0; i < dstFrames; ++i )
        {
            float L = *pSrc++;
            float R = *pSrc++;
            m_Lmin = std::min( m_Lmin, L );
            m_Lmax = std::max( m_Lmax, R );
            m_Rmin = std::min( m_Rmin, L );
            m_Rmax = std::max( m_Rmax, R );
        }
    }
    else   // Slower path O(N) = one loop for each channel of N channels.
    {
        float const* pSrc = dst;
        for ( uint64_t i = 0; i < dstFrames; ++i )
        {
            float sample = *pSrc;
            m_Lmin = std::min( m_Lmin, sample );
            m_Lmax = std::max( m_Lmax, sample );
            pSrc += dstChannels;
        }

        if ( dstChannels > 1 )
        {
            pSrc = dst + 1;
            for ( uint64_t i = 0; i < dstFrames; ++i )
            {
                float sample = *pSrc;
                m_Rmin = std::min( m_Rmin, sample );
                m_Rmax = std::max( m_Rmax, sample );
                pSrc += dstChannels;
            }
        }
    }
*/
};

} // end namespace audio.
} // end namespace de.
