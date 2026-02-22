#include <de/audio/de_fft_pffft.h>


extern "C"
{
#include <pffft.h>
}

struct DE_FFT_Private_PFFFT
{
    PFFFT_Setup* m_ctx;
    size_t m_n;

    DE_AlignedFloatVector m_input;
    DE_AlignedFloatVector m_output;

    DE_FFT_Private_PFFFT()
        : m_ctx(nullptr)
        , m_n(0)
    {
        open(128);
    }

    ~DE_FFT_Private_PFFFT()
    {
        close();
    }

    size_t
    size() const { return m_n; }

    bool
    open(size_t n)
    {
        if (m_ctx)
        {
            return true;
        }

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
        if (!m_ctx)
        {
            return;
        }

        // Clean up
        pffft_destroy_setup(m_ctx);
        m_ctx = nullptr;
        m_n = 0;
    }

    // const TAlignedVector<float>& getInput()  { return m_aligned_input; }
    // const TAlignedVector<float>& getOutput()  { return m_aligned_output; }

    void fft_real(const float* __restrict__ pSrc, float* __restrict__ pDst, size_t n)
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
            close();
            open(n);

            m_input.resize(n);
            m_output.resize(n);
            // DE_WARN("Resize(",n,")")
        }

        // Copy input to aligned buffer
        memcpy(m_input.data(), pSrc, n * sizeof(float));

        // Perform FFT
        pffft_transform_ordered(m_ctx, m_input.data(), m_output.data(), NULL, PFFFT_FORWARD);

        // Copy result to output
        const float* src = m_output.data();
        float* dst = pDst;
        for (size_t i = 0; i < n/2; i++)
        {
            float re = *src++;
            float im = *src++;
            float l2 = std::max(1.0e-24f, re*re + im*im);
            float dB = 10.0f*log10f( l2 ); // Umwandzzzzlung in Dezibels
            //if ( len2 > 1.0e-24f )
            //{
            //    dB = 5.0f*log10f( len2 ); // Umwandlung in Dezibels
            //}
            //dB *= T(1)/T(320); T(10) *
            //dB *= T(1)/T(320); T(10) *
            //if ( dB < T(-20) ) dB = T(-320); // Benni filter
            //if ( dB > T(0) ) dB *= T(2); // Benni enhancer
            *dst++ = dB; // std::max(0.0f, (100 + dB) / 100); // / 160.0f; //  / 160.0f
        }
        // memcpy(dst, m_output.data(), n * sizeof(float));
    }
};

DE_FFT_PFFFT::DE_FFT_PFFFT()
{
    _d = new DE_FFT_Private_PFFFT();
}
DE_FFT_PFFFT::~DE_FFT_PFFFT()
{
    delete _d;
}

void DE_FFT_PFFFT::fft(const float* __restrict__ src, float* __restrict__ dst, size_t n)
{
    _d->fft_real(src,dst,n);
}
