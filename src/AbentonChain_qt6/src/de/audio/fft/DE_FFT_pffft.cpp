#include <de/audio/fft/DE_FFT_pffft.h>

extern "C"
{
	#include <pffft.h>
}

struct DE_FFT_pffft_Private
{
    PFFFT_Setup* m_ctx;
    size_t m_n;

    DE_AlignedFloatVector m_input;
    DE_AlignedFloatVector m_output;

    DE_FFT_pffft_Private()
        : m_ctx(nullptr), m_n(0)
    {
        open(128);
    }

    ~DE_FFT_pffft_Private() { close(); }

    size_t size() const { return m_n; }

    bool open(size_t n)
    {
        if (m_ctx) { return true; }

        m_ctx = pffft_new_setup(n, PFFFT_REAL);
        if (!m_ctx)
        {
            DE_ERROR("Failed to create PFFFT setup")
            m_n = 0;
        }
        else
        {
            m_n = n;
        }
        return true;
    }

    void close()
    {
        if (!m_ctx) { return; }

        // Clean up
        pffft_destroy_setup(m_ctx);
        m_ctx = nullptr;
        m_n = 0;
    }

    inline bool isPowerOfTwo(uint32_t x)
    {
        return x && !(x & (x - 1));
    }

    void fft_real( const float* __restrict__ pSrc,
        float* __restrict__ pDst, uint32_t n)
    {
        if (n<1 || n>10000000)
        {
            DE_ERROR("Invalid input n = ",n)
            return;
        }
		
        if (!m_ctx)
        {
            DE_ERROR("No PFFFT")
            return;
        }

        if ( n != size())
        {
            if (!isPowerOfTwo(n))
            {
                DE_ERROR("Not power of 2, n = ",n)
            }
            close();
            open(n);

            m_input.resize(n);
            m_output.resize(n);
            // DE_WARN("Resize(",n,")")
        }

        // Copy input to aligned buffer
        memcpy(m_input.data(), pSrc, n * sizeof(float));

        // Perform FFT
        pffft_transform_ordered(
            m_ctx,
            m_input.data(),
            m_output.data(),
            NULL,
            PFFFT_FORWARD);

        // Copy result to output
        const float* __restrict__ src = m_output.data();
        float* __restrict__ dst = pDst;
        for (size_t i = 0; i < n/2; ++i)
        {
            float re = *src++;
            float im = *src++;
            float ll = std::max(re*re + im*im, 1.0e-12f);
            //      log10f( 1e-24 ) = -12
            // 10 * log10f( 1e-24 ) = -120dB
            *dst++ = 10.0f*log10f( ll );
        }
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

void DE_FFT_pffft::fft(const float* __restrict__ src, float* __restrict__ dst, size_t n)
{
    _d->fft_real(src,dst,n);
}
