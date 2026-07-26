/**********************************************************************

  resample.h

  Real-time library interface by Dominic Mazzoni

  Based on resample-1.7:
    http://www-ccrma.stanford.edu/~jos/resample/

  Dual-licensed as LGPL and BSD; see README.md and LICENSE* files.

**********************************************************************/

#ifndef LIBRESAMPLE_INCLUDED
#define LIBRESAMPLE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

// 1. Fixed conversion (44.1 ↔ 48 kHz)
// minFactor = maxFactor = 48000/44100 = out/in

// 2. Drift correction (WASAPI full‑duplex)
// minFactor = 0.98
// maxFactor = 1.02

// 3. Pitch shifting / varispeed
// minFactor = 0.5
// maxFactor = 2.0

// 4. Extreme time‑stretching
// minFactor = 0.1
// maxFactor = 10.0


// 0 → fast, low‑latency, shorter filter, more aliasing
// 1 → high quality, long filter, steep rolloff, low aliasing
void *resample_open(int      highQuality,
                    double   minFactor,
                    double   maxFactor);

void *resample_dup(const void *handle);

// ✔ What happens at 44.1 → 48 kHz?

// Ratio = 1.088435

// Output grows by ~8.8%

// Buffer = inLen * 1.088435 + filterWidth

// ✔ What happens when slowing down?

// Ratio < 1

// Output shrinks

// Buffer = inLen * minFactor + filterWidth

// Slowdown 10x = 0.1 factor.
// inBufferLen  =  outLen/factor + W
// inBufferLen  =  128/0.1 + W = 1280 + W

int resample_get_filter_width(const void *handle);

// The function returns number of output samples written.
int resample_process(void   *handle,
                     double  factor,
                     float  *inBuffer,
                     int     inBufferLen,
                     int     lastFlag,      // 1 = flush filter tail (end of stream)
                     int    *inBufferUsed,  // How many input samples were consumed
                     float  *outBuffer,
                     int     outBufferLen);

void resample_close(void *handle);

#ifdef __cplusplus
}		/* extern "C" */
#endif	/* __cplusplus */

#endif /* LIBRESAMPLE_INCLUDED */
