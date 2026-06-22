#include <de/math/fft/DE_FFT_generic.h>

/*
┌────────────────────────────────────────────────────────────────────────────┐
│ PFFFT Real FFT Output (N floats)                                           │
├────────────┬────────────┬────────────┬────────────┬────────────┬───────────┤
│ Index      │   [0]      │   [1]      │   [2]      │   [3]      │   [4]     │
│ Content    │   DC       │  Nyquist   │   Re[1]    │   Im[1]    │   Re[2]   │
│ Frequency  │   0 Hz     │   fs/2     │   fs/N     │   fs/N     │  2fs/N    │
├────────────┴────────────┴────────────┴────────────┴────────────┴───────────┤
│   ... continued interleaved Re[k], Im[k] for bins k = 1 to N/2 - 1         │
├────────────┬────────────┬────────────┬────────────┬────────────┬───────────┤
│ Index      │ [N-4]      │ [N-3]      │ [N-2]      │ [N-1]      │           │
│ Content    │ Re[N/2-2]  │ Im[N/2-2]  │ Re[N/2-1]  │ Im[N/2-1]  │           │
│ Frequency  │ (N/2-2)fs/N│ (N/2-2)fs/N│ (N/2-1)fs/N│ (N/2-1)fs/N│           │
└────────────┴────────────┴────────────┴────────────┴────────────┴───────────┘
*/

namespace de {
namespace math {
namespace fft {

void DE_FFT_generic::resize(size_t n)
{
	if ((n & (n - 1)) != 0) throw std::runtime_error("Size must be power of 2");

	m_imag.resize(n, 0.0f);
	m_real.resize(n, 0.0f);
	m_packed.resize(n, 0.0f);
}

void DE_FFT_generic::fft(const DE_AlignedFloatVector& input, DE_AlignedFloatVector& output)
{
	size_t N = input.size();

	resize( N );

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

	for (size_t len = 2; len <= N; len <<= 1)
	{

		float angle = -2.0f * M_PI / len;

		float wlen_r = cos(angle);

		float wlen_i = sin(angle);

		for (size_t i = 0; i < N; i += len)
		{

			float wr = 1.0f, wi = 0.0f;

			for (size_t j = 0; j < len / 2; ++j)
			{

				size_t u = i + j;

				size_t v = i + j + len / 2;

				float ur = output[u], ui = m_imag[u];

				float vr = output[v] * wr - m_imag[v] * wi;

				float vi = output[v] * wi + m_imag[v] * wr;

				output[u] = ur + vr;

				m_imag[u]   = ui + vi;

				output[v] = ur - vr;

				m_imag[v]   = ui - vi;

				float next_wr = wr * wlen_r - wi * wlen_i;

				wi = wr * wlen_i + wi * wlen_r;

				wr = next_wr;

			}

		}

	}


	// Pack result: [DC, Nyquist, Re[1], Im[1], ..., Re[N/2-1], Im[N/2-1]]

	m_packed[0] = output[0];           // DC

	m_packed[1] = output[N / 2];       // Nyquist

	for (size_t k = 1; k < N / 2; ++k)
	{

		m_packed[2 * k]     = output[k];

		m_packed[2 * k + 1] = m_imag[k];

	}

	// output = std::move(m_packed);
	// output.swap(packed);
	std::memcpy( output.data(), m_packed.data(), m_packed.size() * sizeof(float) );
}


void DE_FFT_generic::ifft(const std::vector<float>& spectrum, std::vector<float>& output)
{

	size_t N = spectrum.size();

	if ((N & (N - 1)) != 0) throw std::runtime_error("Size must be power of 2");


	std::vector<float> real(N, 0.0f), imag(N, 0.0f);


	real[0] = spectrum[0];           // DC

	real[N / 2] = spectrum[1];       // Nyquist

	for (size_t k = 1; k < N / 2; ++k)
	{

		real[k] = spectrum[2 * k];

		imag[k] = spectrum[2 * k + 1];

		real[N - k] = spectrum[2 * k];       // conjugate symmetry

		imag[N - k] = -spectrum[2 * k + 1];

	}


	// Inverse FFT

	for (size_t i = 1, j = 0; i < N; ++i)
	{

		size_t bit = N >> 1;

		while (j & bit) { j ^= bit; bit >>= 1; }

		j ^= bit;

		if (i < j)
		{

			std::swap(real[i], real[j]);

			std::swap(imag[i], imag[j]);

		}

	}


	for (size_t len = 2; len <= N; len <<= 1)
	{

		float angle = 2.0f * M_PI / len;

		float wlen_r = cos(angle);

		float wlen_i = sin(angle);

		for (size_t i = 0; i < N; i += len)
		{

			float wr = 1.0f, wi = 0.0f;

			for (size_t j = 0; j < len / 2; ++j)
			{

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
	{
		output[i] = real[i] / N;
	}

}

} // end namespace fft.
} // end namespace math.
} // end namespace de.


