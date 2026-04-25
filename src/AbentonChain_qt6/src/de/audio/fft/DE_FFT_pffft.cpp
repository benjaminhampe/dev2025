#include <de/audio/fft/DE_FFT_pffft.h>
#include <de/audio/fft/approx_math.h>

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
    DE_GuardedBuffer m_input;
    DE_GuardedBuffer m_output;

    DE_FFT_pffft_Private()
        : m_ctx(nullptr)
        , m_fftSize(0)
        , m_input("pffft_input")
        , m_output("pffft_output")
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

        if (!de::audio::math::isPowerOfTwo(n))
        {
            DE_ERROR("fftSize ", n, " not a power of 2.")
            n = de::audio::math::nextPowerOf2(n);
            DE_ERROR("Computed fftSize ", n)
        }

        if (n>16u*1024u)
        {
            DE_ERROR("fftSize ", n, " too large clamp to 16k.")
            n = 16u*1024u;
        }

        assert(m_input.pre == 0xDEADBEEF);
        assert(m_input.post == 0xCAFEBABE);
        assert(m_output.pre == 0xDEADBEEF);
        assert(m_output.post == 0xCAFEBABE);

        m_input.data.resize(n);
        m_output.data.resize(n);

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
        }
        else
        {
            m_fftSize = 0;
            DE_ERROR("No new context. ", n)
        }

        assert(m_input.pre == 0xDEADBEEF);
        assert(m_input.post == 0xCAFEBABE);
        assert(m_output.pre == 0xDEADBEEF);
        assert(m_output.post == 0xCAFEBABE);
    }

    void fft_real(const float* __restrict__ pSrc, uint32_t nSrc,
                        float* __restrict__ pDst, uint32_t nDst)
    {
        if (!pSrc)
        {
            DE_ERROR("")
        }

        if (!pDst)
        {
            DE_ERROR("")
        }

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

        if (nSrc < 1)
        {
            DE_ERROR("Nothing todo for nSrc ",nSrc)
            return;
        }

        if (nDst < 1)
        {
            DE_ERROR("Nothing todo for nDst ",nDst)
            return;
        }

        if (!m_ctx)
        {
            DE_ERROR("No context")
            return;
        }

        assert(m_input.pre == 0xDEADBEEF);
        assert(m_input.post == 0xCAFEBABE);
        assert(m_output.pre == 0xDEADBEEF);
        assert(m_output.post == 0xCAFEBABE);

        // Fill input data:
        auto nAct = std::min(nSrc,uint32_t(m_input.data.size()));
        for (size_t i = 0; i < nAct; ++i)
        {
            m_input.data[i] = *pSrc++;
        }
        for (size_t i = nAct; i < m_input.data.size(); ++i)
        {
            m_input.data[i] = 0.0f;
        }

        // memcpy(m_input.data.data(), pSrc, nSrc * sizeof(float));

        assert(m_input.pre == 0xDEADBEEF);
        assert(m_input.post == 0xCAFEBABE);
        assert(m_output.pre == 0xDEADBEEF);
        assert(m_output.post == 0xCAFEBABE);

        // Fill input zeroes:
        // const uint32_t mSrc = m_fftSize - nSrc;
        // if (mSrc > 0)
        // {
        //     std::memset(m_input.data.data() + nSrc * sizeof(float),
        //         0, mSrc * sizeof(float));
        // }


        assert(m_input.pre == 0xDEADBEEF);
        assert(m_input.post == 0xCAFEBABE);
        assert(m_output.pre == 0xDEADBEEF);
        assert(m_output.post == 0xCAFEBABE);

        // Perform FFT
        pffft_transform_ordered(
            m_ctx,
            m_input.data.data(),
            m_output.data.data(),
            NULL,
            PFFFT_FORWARD);

        assert(m_input.pre == 0xDEADBEEF);
        assert(m_input.post == 0xCAFEBABE);
        assert(m_output.pre == 0xDEADBEEF);
        assert(m_output.post == 0xCAFEBABE);

        // Copy result to output
        float* __restrict__ dst = pDst;

        *dst++ = -240.0f; // minimum dB

        assert(m_input.pre == 0xDEADBEEF);
        assert(m_input.post == 0xCAFEBABE);
        assert(m_output.pre == 0xDEADBEEF);
        assert(m_output.post == 0xCAFEBABE);

        assert(nDst + m_fftSize/2 <= m_output.data.size());

        for (size_t i = 1; i < nDst; ++i)
        {
            //      log10f( 1e-12 ) = -12
            // 20 * log10f( 1e-12 ) = -240dB
            const float re = m_output.data[i];
            const float im = m_output.data[i + m_fftSize/2];
            const float ll = std::fmaxf(1.0e-12f, (re*re) + (im*im));
            *dst++ = 20.0f*log10f( ll );
        }

        assert(m_input.pre == 0xDEADBEEF);
        assert(m_input.post == 0xCAFEBABE);
        assert(m_output.pre == 0xDEADBEEF);
        assert(m_output.post == 0xCAFEBABE);
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

uint32_t DE_FFT_pffft::fftSize() const
{
    return _d->m_fftSize;
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