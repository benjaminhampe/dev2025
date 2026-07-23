#include <de/audio/de_fft_generic.h>

#if 0

#include <vector>
#include <cmath>

void fft_real_generic(const std::vector<float>& input, std::vector<float>& output) {
    size_t N = input.size();
    if ((N & (N - 1)) != 0) throw std::runtime_error("Size must be power of 2");

    output.resize(N);
    std::vector<float> imag(N, 0.0f);

    // Copy input to output
    for (size_t i = 0; i < N; ++i) output[i] = input[i];

    // Bit reversal
    for (size_t i = 1, j = 0; i < N; ++i) {
        size_t bit = N >> 1;
        while (j & bit) { j ^= bit; bit >>= 1; }
        j ^= bit;
        if (i < j) std::swap(output[i], output[j]);
    }

    // Cooley-Tukey
    for (size_t len = 2; len <= N; len <<= 1) {
        float angle = -2.0f * M_PI / len;
        float wlen_r = cos(angle);
        float wlen_i = sin(angle);
        for (size_t i = 0; i < N; i += len) {
            float wr = 1.0f, wi = 0.0f;
            for (size_t j = 0; j < len / 2; ++j) {
                size_t u = i + j;
                size_t v = i + j + len / 2;
                float ur = output[u], ui = imag[u];
                float vr = output[v] * wr - imag[v] * wi;
                float vi = output[v] * wi + imag[v] * wr;
                output[u] = ur + vr;
                imag[u]   = ui + vi;
                output[v] = ur - vr;
                imag[v]   = ui - vi;
                float next_wr = wr * wlen_r - wi * wlen_i;
                wi = wr * wlen_i + wi * wlen_r;
                wr = next_wr;
            }
        }
    }

    // Pack result: [DC, Nyquist, Re[1], Im[1], ..., Re[N/2-1], Im[N/2-1]]
    std::vector<float> packed(N);
    packed[0] = output[0];           // DC
    packed[1] = output[N / 2];       // Nyquist
    for (size_t k = 1; k < N / 2; ++k) {
        packed[2 * k]     = output[k];
        packed[2 * k + 1] = imag[k];
    }
    output = std::move(packed);
}

void ifft_real_generic(const std::vector<float>& spectrum, std::vector<float>& output) {
    size_t N = spectrum.size();
    if ((N & (N - 1)) != 0) throw std::runtime_error("Size must be power of 2");

    std::vector<float> real(N, 0.0f), imag(N, 0.0f);

    real[0] = spectrum[0];           // DC
    real[N / 2] = spectrum[1];       // Nyquist
    for (size_t k = 1; k < N / 2; ++k) {
        real[k] = spectrum[2 * k];
        imag[k] = spectrum[2 * k + 1];
        real[N - k] = spectrum[2 * k];       // conjugate symmetry
        imag[N - k] = -spectrum[2 * k + 1];
    }

    // Inverse FFT
    for (size_t i = 1, j = 0; i < N; ++i) {
        size_t bit = N >> 1;
        while (j & bit) { j ^= bit; bit >>= 1; }
        j ^= bit;
        if (i < j) {
            std::swap(real[i], real[j]);
            std::swap(imag[i], imag[j]);
        }
    }

    for (size_t len = 2; len <= N; len <<= 1) {
        float angle = 2.0f * M_PI / len;
        float wlen_r = cos(angle);
        float wlen_i = sin(angle);
        for (size_t i = 0; i < N; i += len) {
            float wr = 1.0f, wi = 0.0f;
            for (size_t j = 0; j < len / 2; ++j) {
                size_t u = i + j;
                size_t v = i + j + len / 2;
                float vr = real[v] * wr - imag[v] * wi;
                float vi = real[v] * wi + imag[v] * wr;
                real[v] = real[u] - vr;
                imag[v] = imag[u] - vi;
                real[u] += vr;
                imag[u] += vi;
                float next_wr = wr * wlen_r - wi * wlen_i;
                wi = wr * wlen_i + wi * wlen_r;
                wr = next_wr;
            }
        }
    }

    output.resize(N);
    for (size_t i = 0; i < N; ++i)
        output[i] = real[i] / N;
}


#endif
