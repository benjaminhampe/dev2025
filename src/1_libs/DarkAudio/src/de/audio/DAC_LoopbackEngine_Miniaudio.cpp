#include <de/audio/DAC_LoopbackEngine_Miniaudio.h>
#include <de/audio/IDspChainElement.h>

#ifndef BENNI_USE_MA_ENCODER
#define BENNI_USE_MA_ENCODER
#endif

#include <miniaudio.h>
#include <de/audio/de_approx_math.h>

#include <de/audio/de_kissfft.h>
#include <de/audio/de_fft_pffft.h>

struct DE_AudioLoopback_Data
{
    constexpr static int N_x64 = sizeof(size_t);

    static_assert(N_x64 == 8,"N_x64 must be 8 [bytes] (64-bit)");

    alignas(N_x64) uint32_t m_channels; // = 2;
    alignas(N_x64) uint32_t m_sampleRate; // = 48000;
    alignas(N_x64) uint32_t m_bufSize; // = 256;
    alignas(N_x64) uint32_t m_fftSize; // = 2*1024;
    alignas(N_x64) uint32_t m_wavSize; // = 64;
    alignas(N_x64) int m_gain;   // PCM y gain
    alignas(N_x64) int m_volume; // PCM y gain
    alignas(N_x64) int m_dBmin; // FFT y Window
    alignas(N_x64) int m_dBmax; // FFT y Window

    // FFT - input
    // DE_AlignedFloatVector m_fft_in;
    // DE_AlignedFloatVector m_fft_out;
    // Env
    // DE_AlignedFloatVector m_env_blak;
    // DE_AlignedFloatVector m_env_hamm;
    // DE_AlignedFloatVector m_env_hann;
    //DE_AlignedFloatShiftVector m_env_in;
    //DE_AlignedFloatShiftVector m_env_out;

    // RMS AudioLevelMeter:
    alignas(N_x64) float m_minL; // Minimum Audio Level on Left channel.
    alignas(N_x64) float m_maxL; // Maximum Audio Level on Left channel.
    alignas(N_x64) float m_minR; // Minimum Audio Level on Right channel.
    alignas(N_x64) float m_maxR; // Maximum Audio Level on Right channel.

    // RMS AudioLevelMeter dB:
    alignas(N_x64) float m_minLdB; // Minimum Audio Level on Left channel.
    alignas(N_x64) float m_maxLdB; // Maximum Audio Level on Left channel.
    alignas(N_x64) float m_minRdB; // Minimum Audio Level on Right channel.
    alignas(N_x64) float m_maxRdB; // Maximum Audio Level on Right channel.

    // float m_norm_factor_min = 1.0f;
    // float m_norm_factor_max = 1.0f;
    // float m_norm_factor_exp = 1.0f;

    // Raw:
    DE_AlignedFloatVector m_raw_L;
    DE_AlignedFloatVector m_raw_R;
    DE_AlignedFloatVector m_raw;
    DE_AlignedFloatShiftMatrix m_raw_matrix;
    DE_FFT_PFFFT m_raw_fft;
    DE_AlignedFloatVector m_raw_fft_input;
    DE_AlignedFloatVector m_raw_fft_output;
    DE_AlignedFloatShiftMatrix m_raw_fft_matrix;

#if BENNI_USE_RAW_MATRIX3D
    DE_AlignedFloatShiftMatrix m_raw_matrix_L;
    DE_AlignedFloatShiftMatrix m_raw_matrix_R;
    DE_FFT_PFFFT m_fft_raw_L;
    DE_FFT_PFFFT m_fft_raw_R;
    DE_AlignedFloatVector m_fft_raw_input_L;
    DE_AlignedFloatVector m_fft_raw_input_R;
    DE_AlignedFloatVector m_fft_raw_output_L;
    DE_AlignedFloatVector m_fft_raw_output_R;
    DE_AlignedFloatShiftMatrix m_fft_raw_matrix_L;
    DE_AlignedFloatShiftMatrix m_fft_raw_matrix_R;
#endif

    // Volume:
    DE_AlignedFloatVector m_vol_L;
    DE_AlignedFloatVector m_vol_R;
    DE_AlignedFloatVector m_vol;
    DE_AlignedFloatShiftMatrix m_vol_matrix;
    DE_FFT_PFFFT m_vol_fft;
    DE_AlignedFloatVector m_vol_fft_input;
    DE_AlignedFloatVector m_vol_fft_output;
    DE_AlignedFloatShiftMatrix m_vol_fft_matrix;

#ifdef BENNI_USE_VOL_MATRIX3D
    DE_AlignedFloatShiftMatrix m_vol_matrix_L;
    DE_AlignedFloatShiftMatrix m_vol_matrix_R;
    DE_FFT_PFFFT m_fft_vol_L;
    DE_FFT_PFFFT m_fft_vol_R;
    DE_AlignedFloatVector m_fft_vol_input_L;
    DE_AlignedFloatVector m_fft_vol_input_R;
    DE_AlignedFloatVector m_fft_vol_output_L;
    DE_AlignedFloatVector m_fft_vol_output_R;
    DE_AlignedFloatShiftMatrix m_fft_vol_matrix_L;
    DE_AlignedFloatShiftMatrix m_fft_vol_matrix_R;
#endif
    DE_AlignedFloatShiftVector m_accum_wav;
    DE_AlignedFloatShiftMatrix m_accum_wav_matrix;
    DE_FFT_PFFFT m_accum_fft;
    DE_AlignedFloatVector m_accum_fft_input;
    DE_AlignedFloatVector m_accum_fft_output;
    DE_AlignedFloatShiftMatrix m_accum_fft_matrix;

#ifdef BENNI_USE_ACCUM_L_R_MATRIX3D
    // Accum:
    DE_AlignedFloatShiftVector m_accum_L;
    DE_AlignedFloatShiftVector m_accum_R;

    DE_AlignedFloatShiftMatrix m_accum_matrix_L;
    DE_AlignedFloatShiftMatrix m_accum_matrix_R;
    DE_FFT_PFFFT m_fft_accum_L;
    DE_FFT_PFFFT m_fft_accum_R;
    DE_AlignedFloatVector m_fft_accum_input_L;
    DE_AlignedFloatVector m_fft_accum_input_R;
    DE_AlignedFloatVector m_fft_accum_output_L;
    DE_AlignedFloatVector m_fft_accum_output_R;
    DE_AlignedFloatShiftMatrix m_fft_accum_matrix_L;
    DE_AlignedFloatShiftMatrix m_fft_accum_matrix_R;

    /*
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
    */
#endif

    DE_AudioLoopback_Data();

    //void resize(uint32_t fftSize, uint32_t bufferFrames, uint32_t rowCount);
    //void zerout();

    void processSamples(
        const double pts,
        const float* pSrc,
        const uint32_t nFrames,
        const uint32_t nChannels,
        const uint32_t sampleRate);
};

inline void applyWindow(int winType, float* __restrict__ dst, const float* __restrict__ src, size_t n)
{
    if (winType == 1)
    {
        de::audio::math::apply_blackman(dst, src, n);
    }
    else if (winType == 2)
    {
        de::audio::math::apply_hamming(dst, src, n);
    }
    else if (winType == 3)
    {
        de::audio::math::apply_hann(dst, src, n);
    }
}

DE_AudioLoopback_Data::DE_AudioLoopback_Data()
{
    m_channels = 2;
    m_sampleRate = 48000;
    m_bufSize = 256;
    m_fftSize = 2*1024;
    m_wavSize = 128;
    m_gain = 100;
    m_volume = 100;
    m_dBmin = -70;
    m_dBmax = 60;

    m_accum_wav.setCallback_onFullVector(
        [&](const DE_AlignedFloatVector& v)
        {
            //DE_OK("accum_L = ",v.size())
            DSP_RESIZE(m_accum_wav_matrix, v.size(), m_wavSize);
            m_accum_wav_matrix.push( v );

            DSP_RESIZE(m_accum_fft_input, v.size());
            applyWindow(1, m_accum_fft_input.data(), v.data(), v.size());

            DSP_RESIZE(m_accum_fft_output, v.size()/2);
            m_accum_fft.fft(m_accum_fft_input.data(),
                              m_accum_fft_output.data(),
                              v.size());

            DSP_RESIZE(m_accum_fft_matrix, v.size()/2, m_wavSize);
            m_accum_fft_matrix.push( m_accum_fft_output );
        });
/*
    m_accum_F.setCallback_onFullVector(
        [&](const DE_AlignedFloatVector& v)
        {
            // DE_OK("accum_L = ",v.size())
            DSP_RESIZE(m_accum_matrix_L, v.size(), m_wavSize);
            m_accum_matrix_L.push( v );

            DSP_RESIZE(m_fft_accum_input_L, v.size());
            applyWindow(0, m_fft_accum_input_L.data(), v.data(), v.size());

            DSP_RESIZE(m_fft_accum_output_L, v.size()/2);
            m_fft_accum_L.fft(m_fft_accum_input_L.data(),
                              m_fft_accum_output_L.data(),
                              v.size());

            DSP_RESIZE(m_fft_accum_matrix_L, v.size()/2, m_wavSize);
            m_fft_accum_matrix_L.push( m_fft_accum_output_L);
            //m_onNotifyL();
        }
    );

    m_accum_R.setCallback_onFullVector(
        [&](const DE_AlignedFloatVector& v)
        {
            DSP_RESIZE(m_accum_matrix_R, v.size(), m_wavSize);
            m_accum_matrix_R.push( v );

            DSP_RESIZE(m_fft_accum_input_R, v.size());
            applyWindow(0, m_fft_accum_input_R.data(), v.data(), v.size());

            DSP_RESIZE(m_fft_accum_output_R, v.size()/2);
            m_fft_accum_R.fft(m_fft_accum_input_R.data(),
                              m_fft_accum_output_R.data(),
                              v.size());

            DSP_RESIZE(m_fft_accum_matrix_R, v.size()/2, m_wavSize);
            m_fft_accum_matrix_R.push( m_fft_accum_output_R);
            // m_onNotifyR();
        }
    );
*/
}

void DE_AudioLoopback_Data::processSamples(
    const double pts,
    const float* pSrc,
    const uint32_t nFrames,
    const uint32_t nChannels,
    const uint32_t sampleRate)
{
    const uint32_t nSamples = nFrames * nChannels;
    if (nSamples < 1)
    {
        DE_DEBUG("Nothing todo.")
        return;
    }

    // resize( m_fftSize, nFrames, m_wavSize );
    // zerout();
    float fGain = 0.01f * m_gain;
    float fVolume = 0.01f * m_volume;

    // [L+R] Raw:
    DSP_RESIZE(m_raw, nFrames);
    DSP_ZEROES(m_raw);
    DSP_MONO(m_raw.data(), nChannels, nFrames, pSrc, -1);
    DSP_MUL(m_raw, fGain);
    DSP_RESIZE(m_raw_matrix, nFrames, m_wavSize);
    m_raw_matrix.push( m_raw );
    // [L] Raw FFT:
    DSP_RESIZE(m_raw_fft_input, nFrames);
    DSP_RESIZE(m_raw_fft_output, nFrames/2);
    applyWindow(1, m_raw_fft_input.data(), m_raw.data(), nFrames);
    m_raw_fft.fft(m_raw_fft_input.data(), m_raw_fft_output.data(), nFrames);
    DSP_RESIZE(m_raw_fft_matrix, nFrames/2, m_wavSize);
    m_raw_fft_matrix.push( m_raw_fft_output );

    // [L] Volume PCM:
    DSP_RESIZE(m_vol, nFrames);
    DSP_ZEROES(m_vol);
    DSP_MUL(m_vol.data(), nFrames, m_raw.data(), fVolume);
    DSP_RESIZE(m_vol_matrix, nFrames, m_wavSize);
    m_vol_matrix.push( m_vol );

    // [L] Raw FFT:
    DSP_RESIZE(m_vol_fft_input, nFrames);
    DSP_RESIZE(m_vol_fft_output, nFrames/2);
    applyWindow(1, m_vol_fft_input.data(), m_vol.data(), nFrames);
    m_vol_fft.fft(m_vol_fft_input.data(), m_vol_fft_output.data(), nFrames);
    DSP_RESIZE(m_vol_fft_matrix, nFrames/2, m_wavSize);
    m_vol_fft_matrix.push( m_vol_fft_output );

    // [L] Accum
    DSP_RESIZE(m_accum_wav, m_fftSize);
    m_accum_wav.push( "accum_wav", m_vol );
/*
    // [L]
    if (nChannels > 0)
    {
        // [L] Raw PCM:
        DSP_RESIZE(m_raw_L, nFrames);
        DSP_COPY(m_raw_L.data(), nFrames, pSrc, nChannels, 1);
        DSP_MUL(m_raw_L, fGain);
        DSP_ADD(m_raw.data(), nFrames, m_vol_L.data() );

        // [L] Volume PCM:
        DSP_RESIZE(m_vol_L, nFrames);
        DSP_MUL(m_vol_L.data(), nFrames, m_raw_L.data(), fVolume);

        DSP_ADD(m_vol.data(), nFrames, m_vol_L.data() );
    }

    if (nChannels > 1)
    {
        // [R] Raw PCM:
        DSP_RESIZE(m_raw_R, nFrames);
        DSP_COPY(m_raw_R.data(), nFrames, pSrc + 1, nChannels);
        DSP_MUL(m_raw_R, fGain);

        // [R] Volume PCM:
        DSP_RESIZE(m_vol_R, nFrames);
        DSP_MUL(m_vol_R.data(), nFrames, m_raw_R.data(), fVolume);

        DSP_ADD(m_vol.data(), nFrames, m_vol_R.data() );
    }

    // [L] Accum
    DSP_RESIZE(m_accum_wav, m_fftSize);

    if (nChannels > 0)
    {
        DSP_MUL(m_vol, 1.0f / float(nChannels) );
    }
    m_accum_wav.push( "accum_wav_L+R", m_vol );

    // [L]
    if (nChannels > 0)
    {
        // [L] Raw PCM:
        DSP_RESIZE(m_raw_L, nFrames);
        DSP_COPY(m_raw_L.data(), nFrames, pSrc, nChannels, 1);
        DSP_MUL(m_raw_L, gainL);
#if BENNI_USE_RAW_MATRIX3D
        DSP_RESIZE(m_raw_matrix_L, nFrames, m_wavSize);
        m_raw_matrix_L.push( m_raw_L );
        // [L] Raw FFT:
        DSP_RESIZE(m_fft_raw_input_L, nFrames);
        DSP_RESIZE(m_fft_raw_output_L, nFrames/2);
        DSP_RESIZE(m_fft_raw_matrix_L, nFrames/2, m_wavSize);
        applyWindow(0, m_fft_raw_input_L.data(), m_raw_L.data(), nFrames);
        m_fft_raw_L.fft(m_fft_raw_input_L.data(), m_fft_raw_output_L.data(), nFrames);
        m_fft_raw_matrix_L.push( m_fft_raw_output_L );
#endif

        // [L] Volume PCM:
        DSP_RESIZE(m_vol_L, nFrames);
        DSP_MUL(m_vol_L.data(), nFrames, m_raw_L.data(), volumeL);
#if BENNI_USE_VOL_MATRIX3D
        DSP_RESIZE(m_vol_matrix_L, nFrames, m_wavSize);
        m_vol_matrix_L.push( m_vol_L );
        // [L] Volume FFT:
        DSP_RESIZE(m_fft_vol_input_L, nFrames);
        DSP_RESIZE(m_fft_vol_output_L, nFrames/2);
        DSP_RESIZE(m_fft_vol_matrix_L, nFrames/2, m_wavSize);
        applyWindow(0, m_fft_vol_input_L.data(), m_vol_L.data(), nFrames);
        m_fft_vol_L.fft(m_fft_vol_input_L.data(), m_fft_vol_output_L.data(), nFrames);
        m_fft_vol_matrix_L.push( m_fft_vol_output_L );
#endif
        // [L] Accum
        DSP_RESIZE(m_accum_L, m_fftSize);
        m_accum_L.push( "L_accum", m_vol_L );
    }

    // [R] Store Channel, if any:
    if (nChannels > 1)
    {
        // [R] Raw PCM:
        DSP_RESIZE(m_raw_R, nFrames);
        DSP_COPY(m_raw_R.data(), nFrames, pSrc + 1, nChannels);
        DSP_MUL(m_raw_R, gainL);
#if BENNI_USE_RAW_MATRIX3D
        DSP_RESIZE(m_raw_matrix_R, nFrames, m_wavSize);
        m_raw_matrix_R.push( m_raw_R );
        // [R] Raw FFT:
        DSP_RESIZE(m_fft_raw_input_R, nFrames);
        DSP_RESIZE(m_fft_raw_output_R, nFrames/2);
        DSP_RESIZE(m_fft_raw_matrix_R, nFrames/2, m_wavSize);
        applyWindow(0, m_fft_raw_input_R.data(), m_raw_R.data(), nFrames);
        m_fft_raw_R.fft(m_fft_raw_input_R.data(), m_fft_raw_output_R.data(), nFrames);
        m_fft_raw_matrix_R.push( m_fft_raw_output_R );
#endif
        // [R] Volume PCM:
        DSP_RESIZE(m_vol_R, nFrames);
        DSP_MUL(m_vol_R.data(), nFrames, m_raw_R.data(), volumeL);
#if BENNI_USE_VOL_MATRIX3D
        DSP_RESIZE(m_vol_matrix_R, nFrames, m_wavSize);
        m_vol_matrix_R.push( m_vol_R );
        // [R] Volume FFT:
        DSP_RESIZE(m_fft_vol_input_R, nFrames);
        DSP_RESIZE(m_fft_vol_output_R, nFrames/2);
        DSP_RESIZE(m_fft_vol_matrix_R, nFrames/2, m_wavSize);
        applyWindow(0, m_fft_vol_input_R.data(), m_vol_R.data(), nFrames);
        m_fft_vol_R.fft(m_fft_vol_input_R.data(), m_fft_vol_output_R.data(), nFrames);
        m_fft_vol_matrix_R.push( m_fft_vol_output_R );
#endif
        // [R] Accum
        DSP_RESIZE(m_accum_R, m_fftSize);
        m_accum_R.push( "R_accum", m_vol_R );
    }
*/
}

/*
void DE_AudioLoopback_Data::zerout()
{
    DSP_ZEROES(m_vol_L);
    DSP_ZEROES(m_vol_R);

    DSP_ZEROES(m_fft_input_L);
    DSP_ZEROES(m_fft_input_R);
    DSP_ZEROES(m_fft_output_L);
    DSP_ZEROES(m_fft_output_R);

    //========================================
    // Store WAV:
    //========================================
    m_minL = 0.0f;
    m_maxL = 0.0f;
    m_minR = 0.0f;
    m_maxR = 0.0f;
}
*/

namespace de {
namespace audio {

struct LoopbackDevice
{
    std::atomic_bool m_isPlaying;

    // IDspChainElement* m_inputSignal = nullptr;
    //uint32_t m_fftSize;
    //uint32_t m_wavSize;
    // uint32_t m_channelCount;   // Just needed for drawing
    // int m_updateTimerId;       // Calls render() not as much as AudioCallback would
    ma_result           m_result;
    ma_context          m_context;
    ma_backend          m_loopback_drivers[1]; // = { ma_backend_wasapi };
    ma_format           m_loopback_format; // = ma_format_f32;
    ma_device_config    m_loopback_config;
    ma_device           m_loopback_device; // Loopback mode is currently only supported on WASAPI.
#ifdef BENNI_USE_MA_ENCODER
    ma_encoding_format  m_encoder_format = ma_encoding_format_wav;
    ma_encoder          m_encoder_device;
    ma_encoder_config   m_encoder_config;
    std::string         m_encoder_output_uri;
#endif

    DE_AudioLoopback_Data m_data;

    // CALLBACKS
    FN_onAudioLoopbackNotify m_onNotifyL;
    FN_onAudioLoopbackNotify m_onNotifyR;

    LoopbackDevice()
    {
        m_isPlaying = false;
        m_result = MA_SUCCESS;
        m_loopback_drivers[0] = ma_backend_wasapi;
        m_loopback_format = ma_format_f32;
#ifdef BENNI_USE_MA_ENCODER
        m_encoder_output_uri =  "AudioLoopbackDevice_capture.wav";
        m_encoder_format =      ma_encoding_format_wav;
#endif
        m_onNotifyL = [](){};
        m_onNotifyR = [](){};

        // Initialize miniaudio (NULL = default backend selection)
        m_result = ma_context_init(NULL, 0, NULL, &m_context);
        if (m_result != MA_SUCCESS)
        {
            DE_ERROR("Failed to initialize context")
        }
        else
        {
            ma_device_info* playbackInfos;
            ma_uint32 playbackCount;

            ma_device_info* captureInfos;
            ma_uint32 captureCount;

            ma_device_info playbackDefault;
            ma_device_info captureDefault;

            // ma_context_get_default_device_info(
            //     &context,
            //     ma_device_type_playback,
            //     &defaultPlayback
            //     );

            // printf("Default Playback Device: %s\n", defaultPlayback.name);

            // Enumerate devices
            m_result = ma_context_get_devices(
                &m_context,
                &playbackInfos, &playbackCount,
                &captureInfos,  &captureCount
                );

            if (m_result != MA_SUCCESS)
            {
                DE_ERROR("Failed to enumerate devices")
            }
            else
            {
                DE_TRACE("=== Playback Devices ===")
                for (ma_uint32 i = 0; i < playbackCount; i++)
                {
                    const ma_device_info & dinfo = playbackInfos[i];
                    ma_uint32 n = dinfo.nativeDataFormatCount;
                    DE_OK("Playback[",i,"] ", dinfo.name, ", isDefault(",dinfo.isDefault,"), FormatCount = ",n)

                    for (ma_uint32 f = 0; f < n; f++)
                    {
                        const auto & fmt = dinfo.nativeDataFormats[f];
                        DE_OK("Playback[",i,"].Format[",f,"] "
                            "Channels(", fmt.channels,"), "
                            "SampleRate(", fmt.sampleRate,"), "
                            "SampleType(", fmt.format,")")
                    }
                }

                DE_TRACE("=== Capture Devices ===")
                for (ma_uint32 i = 0; i < captureCount; i++)
                {
                    const ma_device_info & dinfo = captureInfos[i];
                    ma_uint32 n = dinfo.nativeDataFormatCount;
                    DE_OK("Capture[",i,"] ", dinfo.name, ", FormatCount = ",n)

                    for (ma_uint32 f = 0; f < n; f++)
                    {
                        const auto & fmt = dinfo.nativeDataFormats[f];
                        DE_OK("Capture[",i,"].Format[",f,"] "
                              "Channels(", fmt.channels,"), "
                              "SampleRate(", fmt.sampleRate,"), "
                              "SampleType(", fmt.format,")")
                    }
                }

                // ma_context_default_device_info(
                //     &m_context,
                //     ma_device_type_playback,
                //     &playbackDefault
                //     );

                // printf("Default Playback Device: %s\n", defaultPlayback.name);

            }
        }
    }

    ~LoopbackDevice()
    {
        stop();

        ma_context_uninit(&m_context);
    }


    void
    record_callback(
        const double pts,
        const float* pSrc,
        const uint32_t nFrames,
        const uint32_t nChannels,
        const uint32_t sampleRate)
    {
        const uint32_t nSamples = nFrames * nChannels;
        if (nSamples < 1)
        {
            DE_DEBUG("Nothing todo.")
            return;
        }
        m_data.processSamples( pts, pSrc, nFrames, nChannels, sampleRate );

        // m_masterOutput.data()
#ifdef BENNI_USE_MA_ENCODER
        ma_encoder_write_pcm_frames(&m_encoder_device, pSrc, nFrames, NULL);
#endif
    }

    void
    setNotifyL(FN_onAudioLoopbackNotify const & onNotify) { m_onNotifyL = onNotify; }

    void
    setNotifyR(FN_onAudioLoopbackNotify const & onNotify) { m_onNotifyR = onNotify; }

    void
    stop()
    {
        if (!m_isPlaying)
        {
            DE_WARN("Already stopped ...")
            return;
        }
        m_isPlaying = false;

        ma_device_uninit(&m_loopback_device);

        #ifdef BENNI_USE_MA_ENCODER
        ma_encoder_uninit(&m_encoder_device);
        #endif
    }
#if 0
    ma_device_info defaultPlayback;
    ma_device_info defaultCapture;

    ma_context_get_default_device_info(
        &context,
        ma_device_type_playback,
        &defaultPlayback
    );

    printf("Default Playback Device: %s\n", defaultPlayback.name);


#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <stdio.h>

    int main(void)
    {
        ma_result result;
        ma_context context;

        // Initialize context (NULL = default backend selection)
        result = ma_context_init(NULL, 0, NULL, &context);
        if (result != MA_SUCCESS) {
            printf("Failed to initialize context\n");
            return -1;
        }

        ma_device_info* playbackInfos;
        ma_uint32 playbackCount;

        ma_device_info* captureInfos;
        ma_uint32 captureCount;

        // Enumerate devices
        result = ma_context_get_devices(
            &context,
            &playbackInfos, &playbackCount,
            &captureInfos,  &captureCount
            );

        if (result != MA_SUCCESS) {
            printf("Failed to enumerate devices\n");
            ma_context_uninit(&context);
            return -1;
        }

        printf("=== Playback Devices ===\n");
        for (ma_uint32 i = 0; i < playbackCount; i++) {
            printf("[%u] %s\n", i, playbackInfos[i].name);
            printf("    Channels: %u\n", playbackInfos[i].nativeDataFormat.channels);
            printf("    Sample Rate: %u\n", playbackInfos[i].nativeDataFormat.sampleRate);
            printf("    Format: %d\n", playbackInfos[i].nativeDataFormat.format);
        }

        printf("\n=== Capture Devices ===\n");
        for (ma_uint32 i = 0; i < captureCount; i++) {
            printf("[%u] %s\n", i, captureInfos[i].name);
            printf("    Channels: %u\n", captureInfos[i].nativeDataFormat.channels);
            printf("    Sample Rate: %u\n", captureInfos[i].nativeDataFormat.sampleRate);
            printf("    Format: %d\n", captureInfos[i].nativeDataFormat.format);
        }

        ma_context_uninit(&context);
        return 0;
    }
#endif

    bool play()
    {
        if (m_isPlaying)
        {
            DE_WARN("Already playing ...")
            return true;
        }

        m_loopback_config = ma_device_config_init(ma_device_type_loopback);
        // Use default device for this example.
        // Set this to the ID of a _playback_ device if you want to capture from a specific device.
        m_loopback_config.capture.pDeviceID = NULL;
        m_loopback_config.capture.format    = m_loopback_format;
        m_loopback_config.capture.channels  = m_data.m_channels;
        m_loopback_config.sampleRate        = m_data.m_sampleRate;
        m_loopback_config.dataCallback      = LoopbackDevice::cb_miniaudio;
        m_loopback_config.pUserData         = this;
#if 0
        MA_API ma_result ma_device_init_ex(
            const ma_backend backends[],
            ma_uint32 backendCount,
            const ma_context_config* pContextConfig,
            const ma_device_config* pConfig,
            ma_device* pDevice)
#endif
        m_result = ma_device_init_ex(
            m_loopback_drivers,
            sizeof(m_loopback_drivers)/sizeof(m_loopback_drivers[0]),
            NULL,
            &m_loopback_config,
            &m_loopback_device);

        if (m_result != MA_SUCCESS)
        {
            DE_ERROR("Failed to initialize loopback device.")
            return false;
        }

        #ifdef BENNI_USE_MA_ENCODER
        m_encoder_config = ma_encoder_config_init(
            m_encoder_format,
            m_loopback_format,
            m_data.m_channels,
            m_data.m_sampleRate);

        m_result = ma_encoder_init_file(
            m_encoder_output_uri.c_str(),
            &m_encoder_config,
            &m_encoder_device);

        if (m_result != MA_SUCCESS)
        {
            DE_ERROR("Failed to initialize output file.")
            return false;
        }
        #endif

        m_result = ma_device_start(&m_loopback_device);
        if (m_result != MA_SUCCESS)
        {
            ma_device_uninit(&m_loopback_device);
            DE_ERROR("Failed to start device.")
            return false;
        }

        m_isPlaying = true;
        DE_OK("isPlaying = ",m_isPlaying)
        return true; // m_isPlaying;
    }

    static void
    cb_miniaudio(ma_device* pDevice, void* pOutput, const void* pInput,
             ma_uint32 frameCount)
    {
        (void)pOutput;
        auto pSrc = reinterpret_cast<const float*>(pInput);
        auto impl = (LoopbackDevice*)pDevice->pUserData;

        double pts = 0.0;
        impl->record_callback(  pts,
                                pSrc,
                                frameCount,
                                impl->m_data.m_channels,
                                impl->m_data.m_sampleRate);
    }

};

} // end namespace audio
} // end namespace de

namespace
{
    de::audio::LoopbackDevice* g_LoopbackDevice = nullptr;
}

//DE_AudioLoopback_Data const &
//dbAudioLoopback_Data() { return g_LoopbackDevice->m_data; }

void    dbAudioLoopback_Init() { g_LoopbackDevice = new de::audio::LoopbackDevice(); }
void    dbAudioLoopback_Free() { delete g_LoopbackDevice; }
void    dbAudioLoopback_Play() { g_LoopbackDevice->play(); }
void    dbAudioLoopback_Stop() { g_LoopbackDevice->stop(); }
void    dbAudioLoopback_SetNotifyL(FN_onAudioLoopbackNotify const & onNotify) { g_LoopbackDevice->setNotifyL(onNotify); }
void    dbAudioLoopback_SetNotifyR(FN_onAudioLoopbackNotify const & onNotify) { g_LoopbackDevice->setNotifyR(onNotify); }
int     dbAudioLoopback_GetGain() { return g_LoopbackDevice->m_data.m_gain; }
void    dbAudioLoopback_SetGain(int volume) { g_LoopbackDevice->m_data.m_gain = volume; }
int     dbAudioLoopback_GetVolume() { return g_LoopbackDevice->m_data.m_volume; }
void    dbAudioLoopback_SetVolume(int volume) { g_LoopbackDevice->m_data.m_volume = volume; }
int     dbAudioLoopback_dBmin() { return g_LoopbackDevice->m_data.m_dBmin; }
int     dbAudioLoopback_dBmax() { return g_LoopbackDevice->m_data.m_dBmax; }
void    dbAudioLoopback_SetdBmin(int dB) { g_LoopbackDevice->m_data.m_dBmin = dB; }
void    dbAudioLoopback_SetdBmax(int dB) { g_LoopbackDevice->m_data.m_dBmax = dB; }
int     dbAudioLoopback_GetBufferSize() { return g_LoopbackDevice->m_data.m_bufSize; }
void    dbAudioLoopback_SetBufferSize(int n) { g_LoopbackDevice->m_data.m_bufSize = n;}
int     dbAudioLoopback_GetFftSize() { return g_LoopbackDevice->m_data.m_fftSize; }
void    dbAudioLoopback_SetFftSize(int n) { g_LoopbackDevice->m_data.m_fftSize = n;}
int     dbAudioLoopback_GetWavSize() { return g_LoopbackDevice->m_data.m_wavSize; }
void    dbAudioLoopback_SetWavSize(int n) { g_LoopbackDevice->m_data.m_wavSize = n;}
float   dbAudioLoopback_GetMinL() { return g_LoopbackDevice->m_data.m_minL; }
float   dbAudioLoopback_GetMinR() { return g_LoopbackDevice->m_data.m_minR; }
float   dbAudioLoopback_GetMaxL() { return g_LoopbackDevice->m_data.m_maxL; }
float   dbAudioLoopback_GetMaxR() { return g_LoopbackDevice->m_data.m_maxR; }

// Raw:
DE_AlignedFloatVector const &       dbAudioLoopback_GetRaw() { return g_LoopbackDevice->m_data.m_raw; }
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetRawMatrix() { return g_LoopbackDevice->m_data.m_raw_matrix; }
DE_AlignedFloatVector const &       dbAudioLoopback_GetFftRawInput() { return g_LoopbackDevice->m_data.m_raw_fft_input; }
DE_AlignedFloatVector const &       dbAudioLoopback_GetFftRawOutput() { return g_LoopbackDevice->m_data.m_raw_fft_output; }
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetFftRawMatrix() { return g_LoopbackDevice->m_data.m_raw_fft_matrix; }

// Volume:
DE_AlignedFloatVector const &       dbAudioLoopback_GetVol() { return g_LoopbackDevice->m_data.m_vol; }
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetVolMatrix() { return g_LoopbackDevice->m_data.m_vol_matrix; }
DE_AlignedFloatVector const &       dbAudioLoopback_GetFftVolInput() { return g_LoopbackDevice->m_data.m_vol_fft_input; }
DE_AlignedFloatVector const &       dbAudioLoopback_GetFftVolOutput() { return g_LoopbackDevice->m_data.m_vol_fft_output; }
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetFftVolMatrix() { return g_LoopbackDevice->m_data.m_vol_fft_matrix; }

// FFT:
DE_AlignedFloatShiftVector const &  dbAudioLoopback_GetAccum() { return g_LoopbackDevice->m_data.m_accum_wav; }
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetAccumMatrix() { return g_LoopbackDevice->m_data.m_accum_wav_matrix; }
DE_AlignedFloatVector const &       dbAudioLoopback_GetFftAccumInput() { return g_LoopbackDevice->m_data.m_accum_fft_input; }
DE_AlignedFloatVector const &       dbAudioLoopback_GetFftAccumOutput() { return g_LoopbackDevice->m_data.m_accum_fft_output; }
DE_AlignedFloatShiftMatrix const &  dbAudioLoopback_GetFftAccumMatrix() { return g_LoopbackDevice->m_data.m_accum_fft_matrix; }

/*

DE_AlignedFloatVector const &
dbAudioLoopback_GetRaw_L() { return g_LoopbackDevice->m_data.m_raw_L; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetRaw_R() { return g_LoopbackDevice->m_data.m_raw_R; }

DE_AlignedFloatShiftMatrix const &
dbAudioLoopback_GetRawMatrix_L() { return g_LoopbackDevice->m_data.m_raw_matrix_L; }
DE_AlignedFloatShiftMatrix const &
dbAudioLoopback_GetRawMatrix_R() { return g_LoopbackDevice->m_data.m_raw_matrix_R; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetFftRawInput_L() { return g_LoopbackDevice->m_data.m_fft_raw_input_L; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetFftRawInput_R() { return g_LoopbackDevice->m_data.m_fft_raw_input_R; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetFftRawOutput_L() { return g_LoopbackDevice->m_data.m_fft_raw_output_L; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetFftRawOutput_R() { return g_LoopbackDevice->m_data.m_fft_raw_output_R; }
DE_AlignedFloatShiftMatrix const &
dbAudioLoopback_GetFftRawMatrix_L() { return g_LoopbackDevice->m_data.m_fft_raw_matrix_L; }
DE_AlignedFloatShiftMatrix const &
dbAudioLoopback_GetFftRawMatrix_R() { return g_LoopbackDevice->m_data.m_fft_raw_matrix_R; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetVol_L() { return g_LoopbackDevice->m_data.m_vol_L; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetVol_R() { return g_LoopbackDevice->m_data.m_vol_R; }
DE_AlignedFloatShiftMatrix const &
dbAudioLoopback_GetVolMatrix_L() { return g_LoopbackDevice->m_data.m_vol_matrix_L; }
DE_AlignedFloatShiftMatrix const &
dbAudioLoopback_GetVolMatrix_R() { return g_LoopbackDevice->m_data.m_vol_matrix_R; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetFftVolInput_L() { return g_LoopbackDevice->m_data.m_fft_vol_input_L; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetFftVolInput_R() { return g_LoopbackDevice->m_data.m_fft_vol_input_R; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetFftVolOutput_L() { return g_LoopbackDevice->m_data.m_fft_vol_output_L; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetFftVolOutput_R() { return g_LoopbackDevice->m_data.m_fft_vol_output_R; }
DE_AlignedFloatShiftMatrix const &
dbAudioLoopback_GetFftVolMatrix_L() { return g_LoopbackDevice->m_data.m_fft_vol_matrix_L; }
DE_AlignedFloatShiftMatrix const &
dbAudioLoopback_GetFftVolMatrix_R() { return g_LoopbackDevice->m_data.m_fft_vol_matrix_R; }
DE_AlignedFloatShiftVector const &
dbAudioLoopback_GetAccum_L() { return g_LoopbackDevice->m_data.m_accum_L; }
DE_AlignedFloatShiftVector const &
dbAudioLoopback_GetAccum_R() { return g_LoopbackDevice->m_data.m_accum_R; }
DE_AlignedFloatShiftMatrix const &
dbAudioLoopback_GetAccumMatrix_L() { return g_LoopbackDevice->m_data.m_accum_matrix_L; }
DE_AlignedFloatShiftMatrix const &
dbAudioLoopback_GetAccumMatrix_R() { return g_LoopbackDevice->m_data.m_accum_matrix_R; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetFftAccumInput_L() { return g_LoopbackDevice->m_data.m_fft_accum_input_L; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetFftAccumInput_R() { return g_LoopbackDevice->m_data.m_fft_accum_input_R; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetFftAccumOutput_L() { return g_LoopbackDevice->m_data.m_fft_accum_output_L; }
DE_AlignedFloatVector const &
dbAudioLoopback_GetFftAccumOutput_R() { return g_LoopbackDevice->m_data.m_fft_accum_output_R; }
DE_AlignedFloatShiftMatrix const &
dbAudioLoopback_GetFftAccumMatrix_L() { return g_LoopbackDevice->m_data.m_fft_accum_matrix_L; }
DE_AlignedFloatShiftMatrix const &
dbAudioLoopback_GetFftAccumMatrix_R() { return g_LoopbackDevice->m_data.m_fft_accum_matrix_R; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetVolumeMatrix_L() { return g_LoopbackDevice->m_volume_matrix_L; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetVolumeMatrix_R() { return g_LoopbackDevice->m_volume_matrix_R; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetVolumeFftMatrix_L() { return g_LoopbackDevice->m_volume_fft_matrix_L; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetVolumeFftMatrix_R() { return g_LoopbackDevice->m_volume_fft_matrix_R; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetNormFront_L() { return g_LoopbackDevice->m_norm_front_L; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetNormFront_R() { return g_LoopbackDevice->m_norm_front_R; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetNormMatrix_L() { return g_LoopbackDevice->m_norm_matrix_L; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetNormMatrix_R() { return g_LoopbackDevice->m_norm_matrix_R; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetPowerFront_L() { return g_LoopbackDevice->m_power_front_L; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetPowerFront_R() { return g_LoopbackDevice->m_power_front_R; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetPowerMatrix_L() { return g_LoopbackDevice->m_power_matrix_L; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetPowerMatrix_R() { return g_LoopbackDevice->m_power_matrix_R; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetBlackmanFront_L() { return g_LoopbackDevice->m_blak_front_L; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetBlackmanFront_R() { return g_LoopbackDevice->m_blak_front_R; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetBlackmanFftFront_L() { return g_LoopbackDevice->m_fft_blak_front_L; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetFftBlackmanFftFront_R() { return g_LoopbackDevice->m_fft_blak_front_R; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetBlackmanMatrix_L() { return g_LoopbackDevice->m_blak_matrix_L; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetBlackmanMatrix_R() { return g_LoopbackDevice->m_blak_matrix_R; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetBlackmanFftMatrix_L() { return g_LoopbackDevice->m_blak_fft_matrix_L; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetBlackmanFftMatrix_R() { return g_LoopbackDevice->m_blak_fft_matrix_R; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetHammingFront_L() { return g_LoopbackDevice->m_hamm_front_L; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetHammingFront_R() { return g_LoopbackDevice->m_hamm_front_R; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetFftHammingFront_L() { return g_LoopbackDevice->m_fft_hamm_front_L; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetFftHammingFront_R() { return g_LoopbackDevice->m_fft_hamm_front_R; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetHammingMatrix_L() { return g_LoopbackDevice->m_hamm_matrix_L; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetHammingMatrix_R() { return g_LoopbackDevice->m_hamm_matrix_R; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetHammingFftMatrix_L() { return g_LoopbackDevice->m_hamm_fft_matrix_L; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetHammingFftMatrix_R() { return g_LoopbackDevice->m_hamm_fft_matrix_R; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetHannFront_L() { return g_LoopbackDevice->m_hann_front_L; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetHannFront_R() { return g_LoopbackDevice->m_hann_front_R; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetFftHannFront_L() { return g_LoopbackDevice->m_fft_hann_front_L; }
// DE_AlignedFloatVector const &
// dbAudioLoopback_GetFftHannFront_R() { return g_LoopbackDevice->m_fft_hann_front_R; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetHannMatrix_L() { return g_LoopbackDevice->m_hann_matrix_L; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetHannMatrix_R() { return g_LoopbackDevice->m_hann_matrix_R; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetHannFftMatrix_L() { return g_LoopbackDevice->m_hann_fft_matrix_L; }
// DE_AlignedFloatShiftMatrix const &
// dbAudioLoopback_GetHannFftMatrix_R() { return g_LoopbackDevice->m_hann_fft_matrix_R; }

#if 0

#include <DarkAudio.h>
#include <cstdint>
#include <sstream>

#include <de/audio/HardwareBackend_Miniaudio.h>
#include <de/audio/IDspChainElement.h>

namespace de {
namespace audio {

struct Engine
{
    EndPoint_Miniaudio m_endpoint;

    // DspSource m_loopbackSource;

    DspSampleCollector m_collector;

    Engine()
    {

    }

    ~Engine()
    {

    }

    void init()
    {
        m_endpoint.dsp_setInputSignal( &m_collector );
    }

    void start()
    {
        m_endpoint.start();
    }
};

Engine* g_pAudioEngine = nullptr;

} // end namespace audio
} // end namespace de


void dbInitDarkAudio()
{
    // de::audio::g_pDarkAudio = new de::audio::DarkAudio;
    // de::audio::Ctx_init(*de::audio::g_pDarkAudio, 2, 48000);

    if (de::audio::g_pAudioEngine) // Ensure existence.
    {
        return;
    }

    de::audio::g_pAudioEngine = new de::audio::Engine();
    de::audio::g_pAudioEngine->init();
}

void dbFreeDarkAudio()
{
    if (!de::audio::g_pAudioEngine) // Does not exist?
    {
        return; // Nothing todo. (Already freed)
    }

    //de::audio::g_pAudioEngine->destroy();   // Destroy
    delete de::audio::g_pAudioEngine;                 // Destroy
    de::audio::g_pAudioEngine = nullptr;              // Destroy
}







Ctx* g_pDarkAudio = nullptr;

struct MiniaudioUtil
{
    static void
    LoopbackDeviceCallback(ma_device* pDevice,
               void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
        (void)pOutput;

        Ctx* dev = (Ctx*)pDevice->pUserData;

#ifdef BENNI_USE_MA_ENCODER
        // enc = (ma_encoder*)pDevice->pUserData
        ma_encoder* enc = &dev->m_encoderDevice;
        ma_encoder_write_pcm_frames(enc, pInput, frameCount, NULL);
#endif
    }

};

int Ctx_init(Ctx & ctx, int nChannels, int iSampleRate)
{
    // MA_API ma_encoder_config
    //        ma_encoder_config_init(
    //             ma_encoding_format encodingFormat,
    //             ma_format format,
    //             ma_uint32 channels,
    //             ma_uint32 sampleRate)

#ifdef BENNI_USE_MA_ENCODER

    ctx.m_encoderOutputUri = "DarkAudio_capture.wav";

    ma_encoder_config & encoderConfig = ctx.m_encoderConfig;
    encoderConfig = ma_encoder_config_init(
        ctx.m_encodingFormat,
        ctx.m_sampleType,
        ctx.m_channels,
        ctx.m_sampleRate);

    // MA_API ma_result
    //        ma_encoder_init_file(
    //              const char* pFilePath,
    //              const ma_encoder_config* pConfig,
    //              ma_encoder* pEncoder);


    if (MA_SUCCESS != ma_encoder_init_file(
            ctx.m_encoderOutputUri.c_str(),
            &ctx.m_encoderConfig,
            &ctx.m_encoderDevice))
    {
        printf("Failed to initialize output file.\n");
        return -1;
    }
#endif

    ma_device_config & loopbackConfig = ctx.m_loopbackConfig;

    loopbackConfig = ma_device_config_init(ma_device_type_loopback);
    loopbackConfig.capture.pDeviceID = NULL;
    // Use default device for this example. Set this to the ID of a _playback_ device if you want to capture from a specific device.
    loopbackConfig.capture.format    = ctx.m_sampleType;
    loopbackConfig.capture.channels  = ctx.m_channels;
    loopbackConfig.sampleRate        = ctx.m_sampleRate;
    loopbackConfig.dataCallback      = MiniaudioUtil::LoopbackDeviceCallback;
    loopbackConfig.pUserData         = &ctx;

    ctx.m_result = ma_device_init_ex(
        ctx.m_backends,
        sizeof(ctx.m_backends)/sizeof(ctx.m_backends[0]),
        NULL,
        &ctx.m_loopbackConfig,
        &ctx.m_loopbackDevice);

    if (ctx.m_result != MA_SUCCESS)
    {
        printf("Failed to initialize loopback device.\n");
        return -2;
    }

    ctx.m_result = ma_device_start(&ctx.m_loopbackDevice);
    if (ctx.m_result != MA_SUCCESS)
    {
        ma_device_uninit(&ctx.m_loopbackDevice);
        printf("Failed to start device.\n");
        return -3;
    }

    //printf("Press Enter to stop recording...\n");
    //getchar();

    // ma_device_uninit(&device);
    // ma_encoder_uninit(&encoder);

    return 0;
}

int Ctx_destroy(Ctx & ctx)
{
    //printf("Press Enter to stop recording...\n");
    //getchar();

    ma_device_uninit(&ctx.m_loopbackDevice);

    ma_encoder_uninit(&ctx.m_encoderDevice);

    return 0;
}

} // end namespace audio
} // end namespace de


void wdbInitDarkAudio()
{
    // de::audio::g_pDarkAudio = new de::audio::DarkAudio;
    // de::audio::Ctx_init(*de::audio::g_pDarkAudio, 2, 48000);

    if (de::audio::g_pDarkAudio) // Ensure existence.
    {
        return;
    }

    de::audio::g_pDarkAudio = new de::audio::Ctx();
    de::audio::Ctx_init(*de::audio::g_pDarkAudio, 2, 48000);
}

void dbFreeDarkAudio()
{
    if (!de::audio::g_pDarkAudio) // Does not exist?
    {
        return; // Nothing todo. (Already freed)
    }

    de::audio::Ctx_destroy(*de::audio::g_pDarkAudio);   // Destroy
    delete de::audio::g_pDarkAudio;                 // Destroy
    de::audio::g_pDarkAudio = nullptr;              // Destroy
}



#include <miniaudio.h>

#include <stdlib.h>
#include <stdio.h>

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_encoder_write_pcm_frames((ma_encoder*)pDevice->pUserData, pInput, frameCount, NULL);

    (void)pOutput;
}

int main(int argc, char** argv)
{
    ma_result result;
    ma_encoder_config encoderConfig;
    ma_encoder encoder;
    ma_device_config deviceConfig;
    ma_device device;

    // Loopback mode is currently only supported on WASAPI.
    ma_backend backends[] = {
        ma_backend_wasapi
    };

    if (argc < 2) {
        printf("No output file.\n");
        return -1;
    }

    encoderConfig = ma_encoder_config_init(ma_encoding_format_wav, ma_format_f32, 2, 44100);

    if (ma_encoder_init_file(argv[1], &encoderConfig, &encoder) != MA_SUCCESS) {
        printf("Failed to initialize output file.\n");
        return -1;
    }

    deviceConfig = ma_device_config_init(ma_device_type_loopback);
    deviceConfig.capture.pDeviceID = NULL;
    // Use default device for this example. Set this to the ID of a _playback_ device if you want to capture from a specific device.
    deviceConfig.capture.format    = encoder.config.format;
    deviceConfig.capture.channels  = encoder.config.channels;
    deviceConfig.sampleRate        = encoder.config.sampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &encoder;

    result = ma_device_init_ex(backends, sizeof(backends)/sizeof(backends[0]), NULL, &deviceConfig, &device);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize loopback device.\n");
        return -2;
    }

    result = ma_device_start(&device);
    if (result != MA_SUCCESS) {
        ma_device_uninit(&device);
        printf("Failed to start device.\n");
        return -3;
    }

    printf("Press Enter to stop recording...\n");
    getchar();

    ma_device_uninit(&device);
    ma_encoder_uninit(&encoder);

    return 0;
}

#endif


*/

