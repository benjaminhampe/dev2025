#include "de_FFT_Controller_PFFFT.h"

#if 0

#include <pffft.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void fft_pffft_real(const float* input, float* output, int N) {
    // Create FFT setup
    PFFFT_Setup* fft = pffft_new_setup(N, PFFFT_REAL);
    if (!fft) {
        fprintf(stderr, "Failed to create PFFFT setup\n");
        return;
    }

    // Allocate aligned buffers
    float* aligned_input = (float*)pffft_aligned_malloc(N * sizeof(float));
    float* aligned_output = (float*)pffft_aligned_malloc(N * sizeof(float));

    // Copy input to aligned buffer
    memcpy(aligned_input, input, N * sizeof(float));

    // Perform FFT
    pffft_transform_ordered(fft, aligned_input, aligned_output, NULL, PFFFT_FORWARD);

    // Copy result to output
    memcpy(output, aligned_output, N * sizeof(float));

    // Clean up
    pffft_aligned_free(aligned_input);
    pffft_aligned_free(aligned_output);
    pffft_destroy_setup(fft);
}

#endif
