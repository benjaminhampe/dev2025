#include "ResampleSoxr.h"

void resample_soxr_f32(AudioFile & out, uint32_t out_rate)
{

}

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
