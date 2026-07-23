#include <DarkAudio.h>

#if 0

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
    loopbackConfig.capture.pDeviceID = NULL; /* Use default device for this example. Set this to the ID of a _playback_ device if you want to capture from a specific device. */
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

    /* Loopback mode is currently only supported on WASAPI. */
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
    deviceConfig.capture.pDeviceID = NULL; /* Use default device for this example. Set this to the ID of a _playback_ device if you want to capture from a specific device. */
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
