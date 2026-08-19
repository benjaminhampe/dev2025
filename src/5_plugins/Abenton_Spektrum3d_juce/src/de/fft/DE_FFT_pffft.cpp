#include <de/fft/DE_FFT_pffft.h>

#if 1

#include <cassert>
extern "C"
{
    #include <pffft.h>
}

struct DE_FFT_pffft_Private
{
    PFFFT_Setup* m_ctx;
    uint32_t m_fftSize;
    uint32_t m_fftSizeRequest;
    de::AlignedFloatVector m_input;
    de::AlignedFloatVector m_output;

    DE_FFT_pffft_Private()
        : m_ctx(nullptr)
        , m_fftSize(0)
        //, m_input("pffft_input")
        //, m_output("pffft_output")
    {
        DE_TRACE("")
    }

    ~DE_FFT_pffft_Private()
    {
        //DE_TRACE("")
        if (m_ctx)
        {
            pffft_destroy_setup(m_ctx);
            m_ctx = nullptr;
        }
    }

    uint32_t fftSize() const { return m_fftSize; }

    void close()
    {
        if (m_ctx)
        {
            pffft_destroy_setup(m_ctx);
            m_ctx = nullptr;
        }

        m_fftSize = 0;
    }

    // Input ist n floats (dicht gepackte reale pcm).
    // Output ist auch n floats (clever gepacktes halbspectrum)
    // k        Realteil	Imaginärteil
    // 0        out[0]      0
    // 1..N/2‑1	out[k]      out[N/2 + k]
    // N/2      out[N/2]	0
    void resize(uint32_t n) // n = fftSize
    {
        if (n == m_fftSize)
        {
            //DE_WARN("Nothing todo.")
            return; // Nothing todo
        }

        if (n<8)
        {
            DE_ERROR("fftSize ", n, " too small.")
            return;
        }

        if (!de::isPowerOfTwo(n))
        {
            DE_ERROR("fftSize ", n, " not a power of 2.")
            n = de::nextPowerOf2(n);
            DE_ERROR("Computed fftSize ", n)
        }

        if (n>64u*1024u)
        {
            DE_ERROR("fftSize ", n, " too large clamp to 64k.")
            n = 64u*1024u;
        }

        m_input.resize(n);
        m_output.resize(n);

        if (m_ctx)
        {
            pffft_destroy_setup(m_ctx);
            m_ctx = nullptr;
        }

        m_ctx = pffft_new_setup(n, PFFFT_REAL);
        if (m_ctx)
        {
            DE_OK("New context. ",n)
            m_fftSize = n;
            m_fftSizeRequest = n;
        }
        else
        {
            m_fftSize = 0;
            m_fftSizeRequest = 0;
            DE_ERROR("No new context. ", n)
        }
    }

    void fft_real(const float* __restrict__ pSrc, uint32_t nSrc,
                        float* __restrict__ pDst, uint32_t nDst)
    {
        if (!pSrc) { DE_ERROR("No src"); return; }
        if (!pDst) { DE_ERROR("No dst"); return; }

        resize(m_fftSizeRequest);

        if (nSrc > m_fftSize)
        {
            DE_WARN("nSrc(",nSrc,") too large ",m_fftSize)
            nSrc = m_fftSize; // clamp.
        }

        if (nDst > m_fftSize/2)
        {
            DE_WARN("nDst(",nDst,") too large ",m_fftSize)
            nDst = m_fftSize/2; // clamp.
        }

        if (nSrc < 1) { DE_ERROR("Nothing todo, nSrc ",nSrc) return; }
        if (nDst < 1) { DE_ERROR("Nothing todo, nDst ",nDst) return; }

        if (!m_ctx) { DE_ERROR("No context") return; }

        // Fill input data:
        auto nAct = std::min(nSrc,uint32_t(m_input.size()));

        float* __restrict__ input = m_input.data();
        DE_ASSUME(input != pSrc);
        for (size_t i = 0; i < nAct; ++i)
        {
            input[i] = *pSrc++;
        }
        for (size_t i = nAct; i < m_input.size(); ++i)
        {
            input[i] = 0.0f;
        }

        // Perform FFT
        pffft_transform_ordered( m_ctx,
            m_input.data(),
            m_output.data(),
            NULL,
            PFFFT_FORWARD);

        // Copy result to output
        assert(nDst + m_fftSize/2 <= m_output.size());

        const float* __restrict__ output = m_output.data();
        DE_ASSUME(output != pDst);

        pDst[0] = -240.0f; // minimum dB

        for (size_t i = 1; i < nDst - 1; ++i)
        {
            //      log10f( 1e-12 ) = -12
            // 20 * log10f( 1e-12 ) = -240dB
            const float re = output[i];
            const float im = output[i + m_fftSize/2];
            const float ll = std::fmaxf(1.0e-12f, (re*re) + (im*im));
            pDst[i] = 20.0f*log10f( ll );
        }

        pDst[nDst-1] = -240.0f; // minimum dB
    }
};

DE_FFT_pffft::DE_FFT_pffft()
{
    _d = new DE_FFT_pffft_Private();
}

DE_FFT_pffft::~DE_FFT_pffft()
{
    delete _d;
}

uint32_t DE_FFT_pffft::getFftSize() const
{
    return _d->m_fftSize;
}

void DE_FFT_pffft::setFftSize( uint32_t requestFftSize )
{
    _d->m_fftSizeRequest = requestFftSize;
}

void DE_FFT_pffft::resize( uint32_t fftSize )
{
    _d->resize(fftSize);
}

void DE_FFT_pffft::fft(const float* __restrict__ src, uint32_t nSrc,
                             float* __restrict__ dst, uint32_t nDst)
{
    _d->fft_real(src,nSrc,dst,nDst);
}

#endif
