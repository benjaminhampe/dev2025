#include <de/audio/fft/WindowFunction.h>
// #include <vector>
// #include <cmath>

namespace de {
namespace audio {

/*
void applyHann(std::vector<float>& x) {
    const size_t N = x.size();
    for (size_t n = 0; n < N; ++n) {
        float w = 0.5f * (1.0f - std::cos(2.0f * M_PI * n / (N - 1)));
        x[n] *= w;
    }
}

void applyHamming(std::vector<float>& x) {
    const size_t N = x.size();
    for (size_t n = 0; n < N; ++n) {
        float w = 0.54f - 0.46f * std::cos(2.0f * M_PI * n / (N - 1));
        x[n] *= w;
    }
}

void applyBlackman(std::vector<float>& x) {
    const size_t N = x.size();
    for (size_t n = 0; n < N; ++n) {
        float w =
            0.42f
            - 0.5f * std::cos(2.0f * M_PI * n / (N - 1))
            + 0.08f * std::cos(4.0f * M_PI * n / (N - 1));
        x[n] *= w;
    }
}

void applyBlackmanHarris(std::vector<float>& x) {
    const size_t N = x.size();
    const float a0 = 0.35875f;
    const float a1 = 0.48829f;
    const float a2 = 0.14128f;
    const float a3 = 0.01168f;

    for (size_t n = 0; n < N; ++n) {
        float w =
            a0
            - a1 * std::cos(2.0f * M_PI * n / (N - 1))
            + a2 * std::cos(4.0f * M_PI * n / (N - 1))
            - a3 * std::cos(6.0f * M_PI * n / (N - 1));
        x[n] *= w;
    }
}

void applyFlatTop(std::vector<float>& x) {
    const size_t N = x.size();
    const float a0 = 1.0f;
    const float a1 = 1.93f;
    const float a2 = 1.29f;
    const float a3 = 0.388f;
    const float a4 = 0.028f;

    for (size_t n = 0; n < N; ++n) {
        float w =
            a0
            - a1 * std::cos(2.0f * M_PI * n / (N - 1))
            + a2 * std::cos(4.0f * M_PI * n / (N - 1))
            - a3 * std::cos(6.0f * M_PI * n / (N - 1))
            + a4 * std::cos(8.0f * M_PI * n / (N - 1));
        x[n] *= w;
    }
}

// Approximation der modifizierten Bessel-Funktion I0
static float I0(float x) {
    float ax = std::fabs(x);
    float y = ax / 2.0f;
    float sum = 1.0f;
    float t = 1.0f;

    for (int k = 1; k < 20; ++k) {
        t *= (y * y) / (k * k);
        sum += t;
    }
    return sum;
}

void applyKaiser(std::vector<float>& x, float beta) {
    const size_t N = x.size();
    const float denom = I0(beta);

    for (size_t n = 0; n < N; ++n) {
        float r = (2.0f * n) / (N - 1) - 1.0f;
        float w = I0(beta * std::sqrt(1.0f - r * r)) / denom;
        x[n] *= w;
    }
}

void applyBartlett(std::vector<float>& x) {
    const size_t N = x.size();
    const float half = (N - 1) * 0.5f;

    for (size_t n = 0; n < N; ++n) {
        float w = 1.0f - std::fabs((n - half) / half);
        x[n] *= w;
    }
}

void applyTukey(std::vector<float>& x, float alpha) {
    const size_t N = x.size();
    const float edge = alpha * (N - 1) / 2.0f;

    for (size_t n = 0; n < N; ++n) {
        float w;

        if (alpha > 0.0f && n < edge) {
            // Rising cosine
            w = 0.5f * (1.0f + std::cos(M_PI * (2.0f * n / (alpha * (N - 1)) - 1.0f)));
        }
        else if (alpha > 0.0f && n > (N - 1) * (1.0f - alpha / 2.0f)) {
            // Falling cosine
            w = 0.5f * (1.0f + std::cos(M_PI * (2.0f * n / (alpha * (N - 1)) - 2.0f / alpha + 1.0f)));
        }
        else {
            // Flat region
            w = 1.0f;
        }

        x[n] *= w;
    }
}
*/

void WindowFunction::updateCoefficients()
{
    if (m_lut.size() < 2)
    {
        return; // Nothing todo.
    }

    auto my_clampf = [](float x, float lo, float hi) -> float
    {
        return fminf(fmaxf(x, lo), hi); // Branchless
    };

    if (m_func == Rect)
    {
        for (size_t i = 0; i < m_lut.size(); ++i)
        {
            m_lut[i] = 1.0f;
        }
    }
    else if (m_func == Hann)
    {
        const float f = 1.0f / (m_lut.size() - 1);
        for (uint32_t i = 0; i < m_lut.size(); ++i)
        {
            float w = 0.5f
                    - 0.5f * cosf(2.0f * M_PI * i * f);
            m_lut[i] = my_clampf(w, 0.0f, 1.0f);
        }
    }
    else if (m_func == Hamming)
    {
        const float f = 1.0f / (m_lut.size() - 1);
        for (uint32_t i = 0; i < m_lut.size(); ++i)
        {
            float w = 0.54f
                    - 0.46f * cosf(2.0f * M_PI * i * f);
            m_lut[i] = my_clampf(w, 0.0f, 1.0f);
        }
    }
    else if (m_func == Blackman)
    {
        const float f = 1.0f / (m_lut.size() - 1);
        for (uint32_t i = 0; i < m_lut.size(); ++i)
        {
            float w = 0.42f
                    - 0.5f * cosf(2.0f * M_PI * i * f)
                    + 0.08f * cosf(4.0f * M_PI * i * f);
            m_lut[i] = my_clampf(w, 0.0f, 1.0f);
        }
    }
    else
    {
        for (size_t i = 0; i < m_lut.size(); ++i)
        {
            m_lut[i] = 0.0f;
        }
    }
}

void WindowFunction::setFunction(eFunc func)
{
    if (m_func != func)
    {
        m_func = func;
        updateCoefficients();
    }
}

void WindowFunction::resize(uint32_t n)
{
    if (n < 2)
    {
        return; // Bad n, nothing todo
    }

    if (n != m_lut.size())
    {
        m_lut.resize(n);
        updateCoefficients();
    }
}

void WindowFunction::apply(const AlignedFloatVector& v_in, AlignedFloatVector& v_out)
{
    apply(v_in.data(), v_in.size(), v_out.data(), v_out.size());
}

void WindowFunction::apply(const float* __restrict__ pIn, uint32_t nIn,
                                 float* __restrict__ pOut, uint32_t nOut)
{
    if (nIn != nOut)
    {
        DE_WARN("Input.size(",nIn,") != Output.size(",nOut,")")
    }

    const size_t N = std::min(nIn, nOut);

    // if (m_func == Rect)
    // {
    //     std::memcpy(pOut, pIn, N * sizeof(float));
    //     return;
    // }

    resize( N );

    // Transform with WindowFunction
    for (size_t i = 0; i < N; ++i) { pOut[i] = pIn[i] * m_lut[i]; }

    // Fill output residue with zeroes
    for (size_t i = nOut; i < N; ++i) { pOut[i] = 0.0f; }
}

} // end namespace audio.
} // end namespace de.


