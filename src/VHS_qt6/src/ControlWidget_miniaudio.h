#pragma once
#include <HideOnCloseWidget.h>
#include <QWidget>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QVector>
#include <QString>

struct MiniAudioDeviceConfig
{
    QString backend;        // e.g. "wasapi"
    QString deviceName;
    int sampleRate;
    int bufferFrames;
    int channelCount;
    QString sampleFormat;   // z.B. "f32", "s16", "u8"
};


class ControlWidget_miniaudio : public HideOnCloseWidget
{
    Q_OBJECT

public:
    explicit ControlWidget_miniaudio(QWidget *parent = nullptr);

    MiniAudioDeviceConfig config() const;

private:
    QComboBox *backendBox;
    QComboBox *deviceBox;
    QComboBox *sampleRateBox;
    QComboBox *bufferFramesBox;
    QComboBox *channelCountBox;
    QComboBox *sampleFormatBox;
};

/*

class ControlWidget_miniaudio : public QWidget
{
    Q_OBJECT

public:
    explicit ControlWidget_miniaudio(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        auto *layout = new QFormLayout(this);

        //
        // Device Name
        //
        deviceBox = new QComboBox(this);
        deviceBox->addItem("Default Device (Default)");
        // Beispiel: echte Geräte würdest du dynamisch aus miniaudio holen
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

    MiniAudioDeviceConfig config() const
    {
        MiniAudioDeviceConfig cfg;
        cfg.deviceName   = deviceBox->currentText();
        cfg.sampleRate   = sampleRateBox->currentData().toInt();
        cfg.bufferFrames = bufferFramesBox->currentData().toInt();
        cfg.channelCount = channelCountBox->currentData().toInt();
        cfg.sampleFormat = sampleFormatBox->currentData().toString();
        return cfg;
    }

private:
    QComboBox *deviceBox;
    QComboBox *sampleRateBox;
    QComboBox *bufferFramesBox;
    QComboBox *channelCountBox;
    QComboBox *sampleFormatBox;
};
*/

/*

auto *selector = new MiniAudioDeviceSelectorWidget;
setCentralWidget(selector);

MiniAudioDeviceConfig cfg = selector->config();

qDebug() << "Device:" << cfg.deviceName;
qDebug() << "SampleRate:" << cfg.sampleRate;
qDebug() << "BufferFrames:" << cfg.bufferFrames;
qDebug() << "Channels:" << cfg.channelCount;
qDebug() << "Format:" << cfg.sampleFormat;

*/
