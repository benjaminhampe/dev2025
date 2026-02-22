#pragma once
#include <cstdint>
#include <functional>
#include <de/audio/IDspChainElement.h>

typedef std::function<void (void)> FN_onAudioLoopbackNotify;

/*
#ifndef BENNI_USE_RAW_MATRIX3D
#define BENNI_USE_RAW_MATRIX3D 1
#endif
#ifndef BENNI_USE_VOL_MATRIX3D
#define BENNI_USE_VOL_MATRIX3D 1
#endif
*/

void    dbAudioLoopback_Init();
void    dbAudioLoopback_Free();
void    dbAudioLoopback_Play();
void    dbAudioLoopback_Stop();
void    dbAudioLoopback_setNotifyL(FN_onAudioLoopbackNotify const & onNotify);
void    dbAudioLoopback_setNotifyR(FN_onAudioLoopbackNotify const & onNotify);
float   dbAudioLoopback_GetMinL();
float   dbAudioLoopback_GetMinR();
float   dbAudioLoopback_GetMaxL();
float   dbAudioLoopback_GetMaxR();

int     dbAudioLoopback_GetGain();
void    dbAudioLoopback_SetGain(int);

int     dbAudioLoopback_GetVolume();
void    dbAudioLoopback_SetVolume(int);

int     dbAudioLoopback_GetBufferSize();
void    dbAudioLoopback_SetBufferSize(int);

int     dbAudioLoopback_GetFftSize();
void    dbAudioLoopback_SetFftSize(int);

int     dbAudioLoopback_GetWavSize();
void    dbAudioLoopback_SetWavSize(int);

int     dbAudioLoopback_dBmin();
int     dbAudioLoopback_dBmax();

void    dbAudioLoopback_SetdBmin(int dB);
void    dbAudioLoopback_SetdBmax(int dB);

/*
struct DE_AudioLoopback_Data
{
    uint32_t m_bufSize = 0; // = 256;
    uint32_t m_fftSize = 0; // = 2*1024;
    uint32_t m_wavSize = 0; // = 64;
    int m_volume = 100;
    // FFT - input
    // DE_AlignedFloatVector m_fft_in;
    // DE_AlignedFloatVector m_fft_out;

    // Env
    DE_AlignedFloatVector m_env_blak;
    DE_AlignedFloatVector m_env_hamm;
    DE_AlignedFloatVector m_env_hann;
    //DE_AlignedFloatShiftVector m_env_in;
    //DE_AlignedFloatShiftVector m_env_out;

    // RMS AudioLevelMeter:
    float m_minL; // Minimum Audio Level on Left channel.
    float m_maxL; // Maximum Audio Level on Left channel.
    float m_minR; // Minimum Audio Level on Right channel.
    float m_maxR; // Maximum Audio Level on Right channel.

    // RMS AudioLevelMeter dB:
    float m_minLdB; // Minimum Audio Level on Left channel.
    float m_maxLdB; // Maximum Audio Level on Left channel.
    float m_minRdB; // Minimum Audio Level on Right channel.
    float m_maxRdB; // Maximum Audio Level on Right channel.

    float m_norm_factor_min = 1.0f;
    float m_norm_factor_max = 1.0f;
    float m_norm_factor_exp = 1.0f;

    // Volume
    DE_AlignedFloatVector m_raw_L;
    DE_AlignedFloatVector m_raw_R;
    DE_AlignedFloatShiftMatrix m_raw_matrix_L;
    DE_AlignedFloatShiftMatrix m_raw_matrix_R;

    DE_AlignedFloatVector m_vol_L;
    DE_AlignedFloatVector m_vol_R;
    DE_AlignedFloatShiftMatrix m_vol_matrix_L;
    DE_AlignedFloatShiftMatrix m_vol_matrix_R;

    // Volume fft
    DE_AlignedFloatVector m_fft_input_L;
    DE_AlignedFloatVector m_fft_input_R;
    DE_AlignedFloatVector m_fft_output_L;
    DE_AlignedFloatVector m_fft_output_R;
    DE_AlignedFloatShiftMatrix m_fft_matrix_L;
    DE_AlignedFloatShiftMatrix m_fft_matrix_R;

    // Accum
    DE_AlignedFloatShiftVector m_accum_L;
    DE_AlignedFloatShiftVector m_accum_R;
    DE_AlignedFloatShiftMatrix m_accum_matrix_L;
    DE_AlignedFloatShiftMatrix m_accum_matrix_R;

    // Accum fft
    DE_AlignedFloatVector m_fft_accum_input_L;
    DE_AlignedFloatVector m_fft_accum_input_R;
    DE_AlignedFloatVector m_fft_accum_output_L;
    DE_AlignedFloatVector m_fft_accum_output_R;
    DE_AlignedFloatShiftMatrix m_fft_accum_matrix_L;
    DE_AlignedFloatShiftMatrix m_fft_accum_matrix_R;

    // Blackman
    DE_AlignedFloatVector m_blak_front_L;
    DE_AlignedFloatVector m_blak_front_R;
    DE_AlignedFloatShiftMatrix m_blak_matrix_L;
    DE_AlignedFloatShiftMatrix m_blak_matrix_R;
    DE_AlignedFloatVector m_blak_fft_front_L;
    DE_AlignedFloatVector m_blak_fft_front_R;
    DE_AlignedFloatShiftMatrix m_blak_fft_matrix_L;
    DE_AlignedFloatShiftMatrix m_blak_fft_matrix_R;
    // Hamming
    DE_AlignedFloatVector m_hamm_front_L;
    DE_AlignedFloatVector m_hamm_front_R;
    DE_AlignedFloatShiftMatrix m_hamm_matrix_L;
    DE_AlignedFloatShiftMatrix m_hamm_matrix_R;
    DE_AlignedFloatVector m_hamm_fft_front_L;
    DE_AlignedFloatVector m_hamm_fft_front_R;
    DE_AlignedFloatShiftMatrix m_hamm_fft_matrix_L;
    DE_AlignedFloatShiftMatrix m_hamm_fft_matrix_R;
    // Hann
    DE_AlignedFloatVector m_hann_front_L;
    DE_AlignedFloatVector m_hann_front_R;
    DE_AlignedFloatShiftMatrix m_hann_matrix_L;
    DE_AlignedFloatShiftMatrix m_hann_matrix_R;
    DE_AlignedFloatVector m_hann_fft_front_L;
    DE_AlignedFloatVector m_hann_fft_front_R;
    DE_AlignedFloatShiftMatrix m_hann_fft_matrix_L;
    DE_AlignedFloatShiftMatrix m_hann_fft_matrix_R;

DE_AudioLoopback_Data();
void resize(uint32_t fftSize, uint32_t bufferFrames, uint32_t rowCount);

void zerout();

void processSamples(
    const double pts,
    const float* pSrc,
    const uint32_t nFrames,
    const uint32_t nChannels,
    const uint32_t sampleRate);
};
*/
// DE_AudioLoopback_Data const &       dbAudioLoopback_Data();

// Raw:
DE_AlignedFloatVector const &       dbAudioLoopback_GetRaw();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetRawMatrix();
DE_AlignedFloatVector const &       dbAudioLoopback_GetFftRawInput();
DE_AlignedFloatVector const &       dbAudioLoopback_GetFftRawOutput();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetFftRawMatrix();

// Volume:
DE_AlignedFloatVector const &       dbAudioLoopback_GetVol();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetVolMatrix();
DE_AlignedFloatVector const &       dbAudioLoopback_GetFftVolInput();
DE_AlignedFloatVector const &       dbAudioLoopback_GetFftVolOutput();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetFftVolMatrix();

// FFT:
DE_AlignedFloatShiftVector const &  dbAudioLoopback_GetAccum();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetAccumMatrix();
DE_AlignedFloatVector const &       dbAudioLoopback_GetFftAccumInput();
DE_AlignedFloatVector const &       dbAudioLoopback_GetFftAccumOutput();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetFftAccumMatrix();

// DE_AlignedFloatVector const &       dbAudioLoopback_GetRaw_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetRaw_R();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetRawMatrix_L();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetRawMatrix_R();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetFftRawInput_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetFftRawInput_R();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetFftRawOutput_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetFftRawOutput_R();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetFftRawMatrix_L();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetFftRawMatrix_R();



// DE_AlignedFloatVector const &       dbAudioLoopback_GetVol_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetVol_R();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetVolMatrix_L();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetVolMatrix_R();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetFftVolInput_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetFftVolInput_R();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetFftVolOutput_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetFftVolOutput_R();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetFftVolMatrix_L();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetFftVolMatrix_R();

// // ## Normalized AudioTimeDomainSignal:
// DE_AlignedFloatVector const &       dbAudioLoopback_GetNormFront_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetNormFront_R();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetNormMatrix_L();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetNormMatrix_R();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetPowerFront_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetPowerFront_R();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetPowerMatrix_L();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetPowerMatrix_R();

// ## Accumulated AudioTimeDomainSignal:
// DE_AlignedFloatShiftVector const &  dbAudioLoopback_GetAccum_L();
// DE_AlignedFloatShiftVector const &  dbAudioLoopback_GetAccum_R();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetAccumMatrix_L();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetAccumMatrix_R();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetFftAccumInput_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetFftAccumInput_R();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetFftAccumOutput_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetFftAccumOutput_R();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetFftAccumMatrix_L();
// DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetFftAccumMatrix_R();

/*
// ## Computed AudioFrequencyDomainSignal:
// DE_AlignedFloatVector const &       dbAudioLoopback_GetBlackmanFront_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetBlackmanFront_R();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetBlackmanMatrix_L();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetBlackmanMatrix_R();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetBlackmanFftFront_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetBlackmanFftFront_R();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetBlackmanFftMatrix_L();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetBlackmanFftMatrix_R();

// ## Computed AudioFrequencyDomainSignal:
// DE_AlignedFloatVector const &       dbAudioLoopback_GetHammingFront_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetHammingFront_R();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetHammingMatrix_L();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetHammingMatrix_R();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetHammingFftFront_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetHammingFftFront_R();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetHammingFftMatrix_L();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetHammingFftMatrix_R();

// ## Computed AudioFrequencyDomainSignal:
// DE_AlignedFloatVector const &       dbAudioLoopback_GetHannFront_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetHannFront_R();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetHannMatrix_L();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetHannMatrix_R();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetHannFftFront_L();
// DE_AlignedFloatVector const &       dbAudioLoopback_GetHannFftFront_R();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetHannFftMatrix_L();
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetHannFftMatrix_R();
*/
