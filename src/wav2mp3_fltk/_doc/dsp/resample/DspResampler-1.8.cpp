#include <de/audio/dsp/DspResampler-1.8.h>
#include <libresample.h>

namespace de {
namespace audio {

// ===================================================================
DspResampler::DspResampler()
    : m_inputSignal{ nullptr }
    , m_handleL{ nullptr }
    , m_handleR{ nullptr }
    , m_blockSizeIn{ 0 }
    , m_blockSizeOut{ 0 }
    , m_sampleRateIn{ 44100 }
    , m_sampleRateOut{ 48000 }
    , m_minRatio{ 0.1 }
    , m_maxRatio{ 10.0 }
    , m_ratio{ 1.0 }
    , m_userFactor{ 1.0 }
    , m_fifo(4096)
{
    DE_TRACE("")
    /* 0 = high quality,
     * 1 = medium,
     * 2 = low */
    int quality = 0;

    m_handleL = resample_open(quality, m_minRatio, m_maxRatio);
    m_handleR = resample_open(quality, m_minRatio, m_maxRatio);
}

// ===================================================================
DspResampler::~DspResampler()
{
    // DE_TRACE("")
    resample_close(m_handleL);
    resample_close(m_handleR);
}

void DspResampler::setSampleRateIn(u32 sampleRate)
{
    m_sampleRateIn = sampleRate;
    // m_ratio =
}

void
DspResampler::dsp_setInputSignal( IDspChainElement* inputSignal, int i )
{
    m_inputSignal = inputSignal;
}

void
DspResampler::dsp_init( u64 frames, u32 channels, u32 sampleRate )
{
    if (frames != m_blockSizeOut)
    {
        DE_TRACE("frames(",frames,"), channels(",channels,"), sampleRate(",sampleRate,")")

        if (m_sampleRateIn < 1)
        {
            DE_ERROR("No imput sampleRate")
            m_sampleRateIn = 44100;
        }

        m_sampleRateOut = sampleRate;
        m_blockSizeOut = frames;

        // 1. Compute effective ratio
        m_ratio = std::clamp( double(m_sampleRateOut) / double(m_sampleRateIn), m_minRatio, m_maxRatio);

        // 2. Output frames are known
        double blockSizeIn = double(m_blockSizeOut) / m_ratio;

        constexpr u32 FILTER_DELAY = 512;

        // 3. Compute input frames with safety margin
        m_blockSizeIn = static_cast<u32>(std::floor(blockSizeIn));

        m_L.resize(m_blockSizeIn);
        m_R.resize(m_blockSizeIn);

        DE_TRACE("m_sampleRateIn = ",m_sampleRateIn)
        DE_TRACE("m_sampleRateOut = ",m_sampleRateOut)
        DE_TRACE("m_blockSizeIn = ",m_blockSizeIn)
        DE_TRACE("m_blockSizeOut = ",m_blockSizeOut)
        DE_TRACE("m_ratio = ",m_ratio)
    }

    if (m_inputSignal)
    {
        if (m_blockSizeIn < 8)
        {
            DE_ERROR("m_blockSizeIn = ", m_blockSizeIn)
        }
        m_inputSignal->dsp_init(m_blockSizeIn, channels, m_sampleRateIn);
    }
}

void DspResampler::dsp_read(double pts, uint32_t frames, uint32_t sampleRate,
                            float* L, float* R)
{
    dsp_init(frames, 2, sampleRate);

    if (!m_inputSignal)
        return;

    int totalOut = 0;

    while (totalOut < (int)frames)
    {
        // 1) Wenn FIFO fast leer → neuen Block vom Source holen
        if (m_fifo.available() < m_blockSizeIn)
        {
            m_inputSignal->dsp_read(
                pts, m_blockSizeIn, m_sampleRateIn,
                m_L.data(), m_R.data()
                );

            // Quelle liefert manchmal 0 → dann Stille pushen
            m_fifo.push(m_L.data(), m_R.data(), m_blockSizeIn);
        }

        // 2) FIFO → Resampler Input
        m_fifo.pop(m_L.data(), m_R.data(), m_blockSizeIn);

        int ninL = 0, ninR = 0;

        int outL = resample_process(
            m_handleL, m_ratio,
            m_L.data(), m_blockSizeIn,
            0, &ninL,
            L + totalOut, frames - totalOut
            );

        int outR = resample_process(
            m_handleR, m_ratio,
            m_R.data(), m_blockSizeIn,
            0, &ninR,
            R + totalOut, frames - totalOut
            );

        if (outL == 0 && outR == 0)
            break;

        totalOut += outL;
    }

    // Rest auffüllen
    if (totalOut < (int)frames)
    {
        memset(L + totalOut, 0, (frames - totalOut) * sizeof(float));
        memset(R + totalOut, 0, (frames - totalOut) * sizeof(float));
    }
}


/*
void
DspResampler::dsp_read(double pts, uint32_t frames, uint32_t sampleRate,
                       float* __restrict__ L, float* __restrict__ R)
{
    // Ziel: genau `frames` Out‑Frames bei `sampleRate` liefern.
    // m_sampleRateIn = Samplerate der Quelle
    // m_ratio        = (double)sampleRate / (double)m_sampleRateIn

    dsp_init(frames, 2, sampleRate);

    if (!m_inputSignal)
    {
        DE_ERROR("Must have inputsignal");
        std::memset(L, 0, frames * sizeof(float));
        std::memset(R, 0, frames * sizeof(float));
        return;
    }

    int totalOut = 0;

    // Solange laufen, bis wir genug Output haben
    while (totalOut < (int)frames)
    {
        // 1) Von der Quelle lesen (immer STEREO gemeinsam!)
        //    m_blockSizeIn = gewünschte Inputblockgröße für den Resampler
        m_inputSignal->dsp_read(
            pts,                      // PTS der Quelle (kannst du auch inkrementell führen)
            m_blockSizeIn,            // gewünschte Anzahl Inputframes
            m_sampleRateIn,           // Samplerate der Quelle
            m_L.data(),               // Temp‑Buffer L
            m_R.data()                // Temp‑Buffer R
            );

        // Wenn die Quelle nichts liefert → Rest mit 0 auffüllen und raus
        // (hier kannst du auch ein "Ende erreicht"‑Flag aus der Quelle auswerten,
        //  falls du so etwas hast)
        bool sourceSilent = true;
        for (size_t i = 0; i < m_blockSizeIn; ++i)
        {
            if (m_L[i] != 0.0f || m_R[i] != 0.0f)
            {
                sourceSilent = false;
                break;
            }
        }

        if (sourceSilent && totalOut == 0)
        {
            // Komplett nichts mehr da → alles stumm
            std::memset(L, 0, frames * sizeof(float));
            std::memset(R, 0, frames * sizeof(float));
            return;
        }
        else if (sourceSilent)
        {
            // Teilweise schon Output, aber jetzt nichts mehr → Rest stumm
            std::memset(L + totalOut, 0, (frames - totalOut) * sizeof(float));
            std::memset(R + totalOut, 0, (frames - totalOut) * sizeof(float));
            return;
        }

        // 2) Resampler füttern
        int ninL = 0;
        int ninR = 0;
        int lastFlag = 0; // 0 = noch nicht Ende, 1 = Ende (falls du das irgendwann brauchst)

        int outL = resample_process(
            m_handleL,
            m_ratio,
            m_L.data(),                // Input
            (int)m_blockSizeIn,        // Anzahl Inputframes
            lastFlag,
            &ninL,                     // tatsächlich konsumierte Inputframes
            L + totalOut,              // Output‑Pointer (weitergeschoben)
            (int)(frames - totalOut)   // noch benötigte Out‑Frames
            );

        int outR = resample_process(
            m_handleR,
            m_ratio,
            m_R.data(),
            (int)m_blockSizeIn,
            lastFlag,
            &ninR,
            R + totalOut,
            (int)(frames - totalOut)
            );

        if (ninL != ninR)
        {
            DE_WARN("ninL(", ninL, ") != ninR(", ninR, ")");
        }

        if (outL != outR)
        {
            DE_WARN("outL(", outL, ") != outR(", outR, ")");
        }

        // Wenn weder Input konsumiert noch Output erzeugt wurde → Deadlock‑Situation
        // (z.B. Resampler voll, Quelle leer, Ratio komisch, etc.)
        if (ninL == 0 && ninR == 0 && outL == 0 && outR == 0)
        {
            DE_WARN("Resampler produced no output and consumed no input, filling rest with silence");
            std::memset(L + totalOut, 0, (frames - totalOut) * sizeof(float));
            std::memset(R + totalOut, 0, (frames - totalOut) * sizeof(float));
            break;
        }

        totalOut += outL;
    }

    // Sicherheitscheck: falls aus irgendeinem Grund zu wenig Output
    if (totalOut < (int)frames)
    {
        DE_WARN("totalOut(", totalOut, ") < frames(", frames, "), padding with silence");
        std::memset(L + totalOut, 0, (frames - totalOut) * sizeof(float));
        std::memset(R + totalOut, 0, (frames - totalOut) * sizeof(float));
    }
}

void DspResampler::dsp_read(f64 pts, u32 frames, u32 sampleRate,
                            float* L, float* R)
{
    dsp_init(frames, 2, sampleRate);

    if (!m_inputSignal)
        return;

    int totalOut = 0;

    while (totalOut < (int)frames)
    {
        // 1) Hole neuen Inputblock
        m_inputSignal->dsp_read(
            pts, m_blockSizeIn, m_sampleRateIn,
            m_L.data(), m_R.data()
            );

        int ninL = 0, ninR = 0;
        int lastFlag = 0;

        // 2) Resample
        int outL = resample_process(
            m_handleL, m_ratio,
            m_L.data(), m_blockSizeIn,
            lastFlag, &ninL,
            L + totalOut, frames - totalOut
            );

        int outR = resample_process(
            m_handleR, m_ratio,
            m_R.data(), m_blockSizeIn,
            lastFlag, &ninR,
            R + totalOut, frames - totalOut
            );

        // 3) Wenn gar nichts konsumiert wurde → Source liefert nichts → fülle mit 0
        if (ninL == 0 && ninR == 0 && outL == 0 && outR == 0)
        {
            memset(L + totalOut, 0, (frames - totalOut) * sizeof(float));
            memset(R + totalOut, 0, (frames - totalOut) * sizeof(float));
            break;
        }

        totalOut += outL;
    }
}


void
DspResampler::dsp_read(f64 pts, u32 frames, u32 sampleRate,
                        f32* __restrict__ L, f32* __restrict__ R)
{
    dsp_init(frames,2,sampleRate);

    if ( !m_inputSignal )
    {
        DE_ERROR("Must have inputsignal")
        return;
    }

    m_inputSignal->dsp_read( pts, m_blockSizeIn, m_sampleRateIn, m_L.data(), m_R.data() );

    int lastFlag = 0;
    int ninL = 0;
    int ninR = 0;
    int noutL = resample_process( m_handleL, m_ratio, m_L.data(), (int)m_blockSizeIn, lastFlag, &ninL, L, (int)frames);
    int noutR = resample_process( m_handleR, m_ratio, m_R.data(), (int)m_blockSizeIn, lastFlag, &ninR, R, (int)frames);

    if (ninL != ninR)
    {
        DE_WARN("ninL(",ninL,") != ninR(",ninR,")")
    }

    if (noutL != noutR)
    {
        DE_WARN("noutL(",noutL,") != noutR(",noutR,")")
    }

    if (ninL != m_blockSizeIn)
    {
        DE_WARN("ninL(",ninL,") != m_blockSizeIn(",m_blockSizeIn,")")
    }

    if (ninR != m_blockSizeIn)
    {
        DE_WARN("ninR(",ninR,") != m_blockSizeIn(",m_blockSizeIn,")")
    }

    if (noutL != m_blockSizeOut)
    {
        DE_WARN("noutL(",noutL,") != m_blockSizeOut(",m_blockSizeOut,")")
    }

    if (noutR != m_blockSizeOut)
    {
        DE_WARN("noutR(",noutR,") != m_blockSizeOut(",m_blockSizeOut,")")
    }
}
*/

} // end namespace audio.
} // end namespace de.


#if 0

#include <soxr.h>

void resample_soxr_to_16k(const std::vector<float> & in, uint32_t in_rate, std::vector<float> & out)
{
    const uint32_t target_rate = 16000;

    if (in_rate == target_rate) {
        out = in;
        return;
    }

    // Verhältnis bestimmen
    double ratio = double(target_rate) / double(in_rate);

    // Zielgröße schätzen
    size_t out_len_est = size_t(in.size() * ratio) + 16;
    out.resize(out_len_est);

    size_t odone = 0;

    soxr_error_t err = soxr_oneshot(
        in_rate,            // input rate
        target_rate,        // output rate
        1,                  // channels (mono)
        in.data(),          // input buffer
        in.size(),          // input samples
        nullptr,            // input samples actually used (optional)
        out.data(),         // output buffer
        out_len_est,        // output buffer size
        &odone,             // output samples actually produced
        nullptr, nullptr, nullptr // quality presets (default HQ)
    );

    if (err) {
        std::cerr << "soxr error: " << err << "\n";
        out = in;
        return;
    }

    out.resize(odone);
}

#endif
