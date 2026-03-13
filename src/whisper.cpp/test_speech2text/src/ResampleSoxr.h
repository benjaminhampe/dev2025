#pragma once
#include "AudioFile.h"

// -lsoxr

void resample_soxr_f32(AudioFile & out, uint32_t out_rate = 16000);

