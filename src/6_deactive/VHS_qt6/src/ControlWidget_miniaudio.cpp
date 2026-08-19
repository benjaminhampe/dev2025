#include "ControlWidget_miniaudio.h"

#ifndef MA_ENABLE_ASIO
#define MA_ENABLE_ASIO   // enable Steinberg ASIO support
#endif
#include <miniaudio.h>

struct MiniAudioDeviceInfo {

    QString name;
    ma_device_id id;
    bool isDefault = false;
    bool isCapture = false;
    int channelCount;
    int sampleRate;
    std::vector<int> sampleRates;
    std::vector<int> bufferFrames;
    std::vector<QString> sampleFormats;
    QString sampleFormat;   // z.B. "f32", "s16", "u8"
};

struct MiniAudioBackendInfo {
    QString name;     // e.g. "wasapi"
    ma_backend backend;      // miniaudio enum
    std::vector<MiniAudioDeviceInfo> play;
    std::vector<MiniAudioDeviceInfo> rec;

    void clear()
    {
        name = "";
        play.clear();
        rec.clear();
    }
};

/*
Enumerator e;
e.enumerate();

for (const auto &backend : e.getBackends()) {
    qDebug() << "Backend:" << backend.backendName;

    for (const auto &dev : backend.playbackDevices) {
        qDebug() << "  Playback:" << dev.name
                 << (dev.isDefault ? "(Default)" : "");
    }

    for (const auto &dev : backend.captureDevices) {
        qDebug() << "  Capture:" << dev.name
                 << (dev.isDefault ? "(Default)" : "");
    }
}
*/

class MiniAudioEnumerator
{
public:
    MiniAudioEnumerator() = default;

    void enumerate(std::vector<MiniAudioBackendInfo> & backends)
    {
        backends.clear();

        // List of backends to probe
        ma_backend availableBackends[] = {
            ma_backend_wasapi,
            // ma_backend_asio,
            ma_backend_dsound,
            ma_backend_winmm,
            ma_backend_alsa,
            ma_backend_pulseaudio,
            ma_backend_jack,
            ma_backend_coreaudio,
            ma_backend_null
        };

        const int backendCount = sizeof(availableBackends) / sizeof(availableBackends[0]);

        for (int i = 0; i < backendCount; ++i)
        {
            ma_backend api = availableBackends[i];
            MiniAudioBackendInfo devInfo;
            if (enumerateBackend(api, devInfo))
            {
                backends.emplace_back( std::move(devInfo) );
            }
        }
    }

    bool enumerateBackend(ma_backend api, MiniAudioBackendInfo & info)
    {
        ma_context ctx;
        ma_context_config cfg = ma_context_config_init();
        if (ma_context_init(&api, 1, &cfg, &ctx) != MA_SUCCESS)
            return false; // backend not available on this system

        info.clear();
        info.backend = api;
        info.name = getBackendString(api);

        ma_device_info* pPlay;
        ma_device_info* pRec;
        ma_uint32 nPlay;
        ma_uint32 nRec;
        if (ma_context_get_devices(&ctx, &pPlay, &nPlay, &pRec, &nRec) == MA_SUCCESS)
        {
            // Playback devices
            for (ma_uint32 i = 0; i < nPlay; i++)
            {
                MiniAudioDeviceInfo dev;
                dev.name = QString::fromUtf8(pPlay[i].name);
                dev.id   = pPlay[i].id;
                dev.isDefault = pPlay[i].isDefault != 0;
                dev.isCapture = false;
                info.play.emplace_back( std::move(dev) );
            }

            // Capture devices
            for (ma_uint32 i = 0; i < nRec; i++)
            {
                MiniAudioDeviceInfo dev;
                dev.name = QString::fromUtf8(pRec[i].name);
                dev.id   = pRec[i].id;
                dev.isDefault = pRec[i].isDefault != 0;
                dev.isCapture = true;
                info.rec.emplace_back( std::move(dev) );
            }
        }

        ma_context_uninit(&ctx);
        return true;
    }

    const std::vector<MiniAudioBackendInfo>& getBackends() const { return m_backends; }

private:
    std::vector<MiniAudioBackendInfo> m_backends;

    QString getBackendString(ma_backend b) const
    {
        switch (b) {
        case ma_backend_wasapi:      return "wasapi";
        // case ma_backend_asio:        return "asio";
        case ma_backend_dsound:      return "dsound";
        case ma_backend_winmm:       return "winmm";
        case ma_backend_alsa:        return "alsa";
        case ma_backend_pulseaudio:  return "pulse";
        case ma_backend_jack:        return "jack";
        case ma_backend_coreaudio:   return "coreaudio";
        case ma_backend_null:        return "null";
        default:                     return "unknown";
        }
    }
};

// explicit
ControlWidget_miniaudio::ControlWidget_miniaudio(QWidget *parent)
    : HideOnCloseWidget(parent)
{
    auto *layout = new QFormLayout(this);

    //
    // Backend
    //
    backendBox = new QComboBox(this);

    struct BackendEntry { QString label; QString code; };
    QVector<BackendEntry> backends = {
        {"WASAPI", "wasapi"},
        {"ASIO", "asio"},
        {"DirectSound", "dsound"},
        {"WinMM", "winmm"},
        {"ALSA", "alsa"},
        {"PulseAudio", "pulse"},
        {"JACK", "jack"},
        {"CoreAudio", "coreaudio"},
        {"Null Backend", "null"}
    };

    QString defaultBackend = "wasapi";

    for (auto &b : backends) {
        QString label = b.label;
        if (b.code == defaultBackend)
            label += " (Default)";
        backendBox->addItem(label, b.code);
    }

    backendBox->setCurrentIndex(backendBox->findData(defaultBackend));
    layout->addRow("Backend:", backendBox);

    //
    // Device Name
    //
    deviceBox = new QComboBox(this);
    deviceBox->addItem("Default Device (Default)");
    deviceBox->addItems({"Realtek HD Audio", "USB Audio Interface", "Virtual Cable"});
    layout->addRow("Device:", deviceBox);

    //
    // Sample Rate
    //
    sampleRateBox = new QComboBox(this);
    QVector<int> sampleRates = {44100, 48000, 96000, 192000};
    int defaultSR = 48000;

    for (int sr : sampleRates) {
        QString label = QString::number(sr);
        if (sr == defaultSR)
            label += " (Default)";
        sampleRateBox->addItem(label, sr);
    }
    sampleRateBox->setCurrentIndex(sampleRateBox->findData(defaultSR));
    layout->addRow("Sample Rate:", sampleRateBox);

    //
    // Buffer Frames
    //
    bufferFramesBox = new QComboBox(this);
    QVector<int> bufferSizes = {128, 256, 512, 1024, 2048};
    int defaultBF = 512;

    for (int bf : bufferSizes) {
        QString label = QString::number(bf);
        if (bf == defaultBF)
            label += " (Default)";
        bufferFramesBox->addItem(label, bf);
    }
    bufferFramesBox->setCurrentIndex(bufferFramesBox->findData(defaultBF));
    layout->addRow("Buffer Frames:", bufferFramesBox);

    //
    // Channel Count
    //
    channelCountBox = new QComboBox(this);
    QVector<int> channels = {1, 2};
    int defaultCh = 2;

    for (int ch : channels) {
        QString label = QString::number(ch);
        if (ch == defaultCh)
            label += " (Default)";
        channelCountBox->addItem(label, ch);
    }
    channelCountBox->setCurrentIndex(channelCountBox->findData(defaultCh));
    layout->addRow("Channels:", channelCountBox);

    //
    // Sample Format
    //
    sampleFormatBox = new QComboBox(this);
    struct FormatEntry { QString name; QString code; };
    QVector<FormatEntry> formats = {
        {"Float32", "f32"},
        {"Signed16", "s16"},
        {"Unsigned8", "u8"}
    };
    QString defaultFmt = "f32";

    for (auto &fmt : formats) {
        QString label = fmt.name;
        if (fmt.code == defaultFmt)
            label += " (Default)";
        sampleFormatBox->addItem(label, fmt.code);
    }
    sampleFormatBox->setCurrentIndex(sampleFormatBox->findData(defaultFmt));
    layout->addRow("Sample Format:", sampleFormatBox);
}

MiniAudioDeviceConfig
ControlWidget_miniaudio::config() const
{
    MiniAudioDeviceConfig cfg;
    cfg.backend      = backendBox->currentData().toString();
    cfg.deviceName   = deviceBox->currentText();
    cfg.sampleRate   = sampleRateBox->currentData().toInt();
    cfg.bufferFrames = bufferFramesBox->currentData().toInt();
    cfg.channelCount = channelCountBox->currentData().toInt();
    cfg.sampleFormat = sampleFormatBox->currentData().toString();
    return cfg;
}

