#include "AudioConfigDialog.h"
#include <QVBoxLayout>

AudioConfigDialog::AudioConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_backendCombo      = new QComboBox(this);
    m_apiCombo          = new QComboBox(this);
    m_channelCountCombo = new QComboBox(this);
    m_sampleRateCombo   = new QComboBox(this);
    m_blockSizeCombo    = new QComboBox(this);
    m_firstChannelCombo = new QComboBox(this);
    m_inputDeviceCombo  = new QComboBox(this);
    m_outputDeviceCombo = new QComboBox(this);

    m_btnApply  = new QPushButton("Übernehmen", this);
    m_btnRefresh = new QPushButton("Erneuern", this);
    m_btnCancel = new QPushButton("Abbrechen", this);

    populateBackends();
    populateApis();
    populateStaticOptions();
    populateDevices();

    auto *form = new QFormLayout;
    form->addRow("Backend:", m_backendCombo);
    form->addRow("API:", m_apiCombo);
    form->addRow("Channel Count:", m_channelCountCombo);
    form->addRow("Sample Rate:", m_sampleRateCombo);
    form->addRow("Block Size:", m_blockSizeCombo);
    form->addRow("First Channel:", m_firstChannelCombo);
    form->addRow("Input Device:", m_inputDeviceCombo);
    form->addRow("Output Device:", m_outputDeviceCombo);

    auto *buttons = new QHBoxLayout;
    buttons->addWidget(m_btnApply);
    buttons->addWidget(m_btnRefresh);
    buttons->addWidget(m_btnCancel);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addLayout(buttons);

    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_btnApply,  &QPushButton::clicked, this, &AudioConfigDialog::configAccepted);
    connect(m_btnRefresh, &QPushButton::clicked, this, &AudioConfigDialog::refreshEnumeration);

    connect(m_backendCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AudioConfigDialog::onBackendChanged);

    setWindowTitle("Audio Configuration");
    setAttribute(Qt::WA_DeleteOnClose);

// <TestEngine>
    m_btnTestSine  = new QPushButton("Sinus Test", this);
    m_btnTestClick = new QPushButton("Klick Test", this);
    m_btnTestStop  = new QPushButton("Stop", this);

    buttons->addWidget(m_btnTestSine);
    buttons->addWidget(m_btnTestClick);
    buttons->addWidget(m_btnTestStop);

    connect(m_btnTestSine, &QPushButton::clicked, this, [this]() {
        unsigned int api = m_apiCombo->currentData().toUInt();
        unsigned int dev = m_outputDeviceCombo->currentData().toUInt();
        unsigned int sr  = m_sampleRateCombo->currentData().toUInt();
        unsigned int ch  = m_channelCountCombo->currentData().toUInt();
        unsigned int bs  = m_blockSizeCombo->currentData().toUInt();

        m_testEngine.start(static_cast<RtAudio::Api>(api),
                           dev, sr, ch, bs,
                           AudioTestEngine::Sine,
                           440.0);
    });

    connect(m_btnTestClick, &QPushButton::clicked, this, [this]() {
        unsigned int api = m_apiCombo->currentData().toUInt();
        unsigned int dev = m_outputDeviceCombo->currentData().toUInt();
        unsigned int sr  = m_sampleRateCombo->currentData().toUInt();
        unsigned int ch  = m_channelCountCombo->currentData().toUInt();
        unsigned int bs  = m_blockSizeCombo->currentData().toUInt();

        m_testEngine.start(static_cast<RtAudio::Api>(api),
                           dev, sr, ch, bs,
                           AudioTestEngine::Click);
    });

    connect(m_btnTestStop, &QPushButton::clicked,
            this, [this]() { m_testEngine.stop(); });

// </TestEngine>

}

void AudioConfigDialog::populateBackends()
{
    m_backendCombo->clear();
    m_backendCombo->addItem("RtAudio", 0);
    m_backendCombo->addItem("Benni_Wasapi_shared", 1);
}

void AudioConfigDialog::populateApis()
{
    m_apiCombo->clear();

    int backend = m_backendCombo->currentData().toInt();

    if (backend == 0) {
        std::vector<RtAudio::Api> apis;
        RtAudio::getCompiledApi(apis);

        for (auto api : apis) {
            m_apiCombo->addItem(QString::fromStdString(RtAudio::getApiDisplayName(api)),
                                static_cast<int>(api));
        }
    } else {
        m_apiCombo->addItem("WASAPI (Shared)", RtAudio::WINDOWS_WASAPI);
    }
}

void AudioConfigDialog::populateStaticOptions()
{
    for (int ch : {1, 2, 4, 6, 8, 16, 32})
        m_channelCountCombo->addItem(QString::number(ch), ch);

    for (int sr : {44100, 48000, 88200, 96000, 192000})
        m_sampleRateCombo->addItem(QString::number(sr), sr);

    for (int bs : {64, 128, 256, 512, 1024})
        m_blockSizeCombo->addItem(QString::number(bs), bs);

    for (int fc = 0; fc < 32; ++fc)
        m_firstChannelCombo->addItem(QString::number(fc), fc);
}

void AudioConfigDialog::populateDevices()
{
    m_inputDeviceCombo->clear();
    m_outputDeviceCombo->clear();

    int api = m_apiCombo->currentData().toInt();
    RtAudio audio(static_cast<RtAudio::Api>(api));

    unsigned int defaultIn  = audio.getDefaultInputDevice();
    unsigned int defaultOut = audio.getDefaultOutputDevice();
    unsigned int count = audio.getDeviceCount();

    // Default first
    if (defaultIn < count) {
        auto info = audio.getDeviceInfo(defaultIn);
        m_inputDeviceCombo->addItem("[Default] " + QString::fromStdString(info.name), defaultIn);
    }

    if (defaultOut < count) {
        auto info = audio.getDeviceInfo(defaultOut);
        m_outputDeviceCombo->addItem("[Default] " + QString::fromStdString(info.name), defaultOut);
    }

    // All devices
    for (unsigned int i = 0; i < count; ++i) {
        auto info = audio.getDeviceInfo(i);

        if (i != defaultIn)
            m_inputDeviceCombo->addItem(QString::fromStdString(info.name), i);

        if (i != defaultOut)
            m_outputDeviceCombo->addItem(QString::fromStdString(info.name), i);
    }

    m_inputDeviceCombo->setCurrentIndex(0);
    m_outputDeviceCombo->setCurrentIndex(0);
}

void AudioConfigDialog::refreshEnumeration()
{
    populateApis();
    populateDevices();
}

void AudioConfigDialog::onBackendChanged(int)
{
    populateApis();
    populateDevices();
}
