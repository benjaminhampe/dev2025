#include "AudioConfigDialog.h"
#include <de/Core.h>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QObject>
#include <de/audio/device/DeviceInfo_WASAPI.h>

using de::audio::eBackend;

QGroupBox* createNonExclusiveGroup()
{
    QGroupBox *groupBox = new QGroupBox("Non-Exclusive Checkboxes");
    groupBox->setFlat(true);
    QCheckBox *checkBox1 = new QCheckBox("&Checkbox 1");
    QCheckBox *checkBox2 = new QCheckBox("C&heckbox 2");
    checkBox2->setChecked(true);
    QCheckBox *tristateBox = new QCheckBox("Tri-&state button");
    tristateBox->setTristate(true);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(checkBox1);
    vbox->addWidget(checkBox2);
    vbox->addWidget(tristateBox);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    return groupBox;
}

AudioConfigDialog::AudioConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    // Row[0]
    m_backendCombo      = new QComboBox(this);
    m_backendCombo->addItem("Benni", int(eBackend::Benni));
    m_backendCombo->addItem("RtAudio", int(eBackend::RtAudio));

    // Row[1]
    m_apiCombo          = new QComboBox(this);

    // Row[2]
    m_outputDeviceCombo = new QComboBox(this);

    // Row[3]
    m_inputDeviceCombo  = new QComboBox(this);

    // Row[4]
    m_channelCountCombo = new QComboBox(this);
    m_firstChannelCombo = new QComboBox(this);

    // Row[5]
    m_sampleRateCombo   = new QComboBox(this);
    m_sampleTypeCombo   = new QComboBox(this);

    // Row[6]
    m_blockSizeCombo    = new QComboBox(this);
    m_blockSizeDspCombo = new QComboBox(this);

    // Row[7]
    m_latencyCombo      = new QComboBox(this);
    m_granularityCombo  = new QComboBox(this);

    // Row[8]
    m_btnApply  = new QPushButton("Apply", this);
    m_btnCancel = new QPushButton("Cancel", this);

    populateApis();
    populateInputDevices();
    populateOutputDevices();
    populateStaticOptions();

    //====================================================

    auto form = new QFormLayout;

    //===Row[0]=================================================
    form->addRow("Backend:", m_backendCombo);

    //===Row[1]=================================================
    auto btnEnumDevices = new QPushButton("Update");
    auto hApi = new QHBoxLayout;
    hApi->setContentsMargins(0,0,0,0);
    hApi->setSpacing(0);
    hApi->addWidget(m_apiCombo,1);
    hApi->addWidget(btnEnumDevices);
    auto rApi = new QWidget(this);
    rApi->setLayout(hApi);
    form->addRow("API:", rApi);

    //===Row[2]=================================================
    form->addRow("Input-Device:", m_inputDeviceCombo);

    //===Row[3]=================================================
    form->addRow("Output-Device:", m_outputDeviceCombo);

    //===Row[4]=================================================
    auto lblFirstChannel = new QLabel("First-Channel:");
    auto hCh = new QHBoxLayout;
    hCh->setContentsMargins(0,0,0,0);
    hCh->setSpacing(0);
    hCh->addWidget(m_channelCountCombo,1);
    hCh->addWidget(lblFirstChannel);
    hCh->addWidget(m_firstChannelCombo,1);
    auto wCh = new QWidget(this);
    wCh->setLayout(hCh);
    form->addRow("Channel-Count:", wCh);

    //===Row[5]=================================================
    auto lblSampleType = new QLabel("Sample-Type:");
    auto hSt = new QHBoxLayout;
    hSt->setContentsMargins(0,0,0,0);
    hSt->setSpacing(0);
    hSt->addWidget(m_sampleRateCombo,1);
    hSt->addWidget(lblSampleType);
    hSt->addWidget(m_sampleTypeCombo,1);
    auto wSt = new QWidget(this);
    wSt->setLayout(hSt);
    form->addRow("Sample-Rate:", wSt);

    //===Row[6]=================================================
    auto lblBlockSizeDsp = new QLabel("Block-Size-Dsp:");
    auto hBs = new QHBoxLayout;
    hBs->setContentsMargins(0,0,0,0);
    hBs->setSpacing(0);
    hBs->addWidget(m_blockSizeCombo,1);
    hBs->addWidget(lblBlockSizeDsp);
    hBs->addWidget(m_blockSizeDspCombo,1);
    auto wBs = new QWidget(this);
    wBs->setLayout(hBs);
    form->addRow("Block-Size:", wBs);

    //===Row[7]=================================================
    auto lblGranularity = new QLabel("Granularity:");
    auto hGr = new QHBoxLayout;
    hGr->setContentsMargins(0,0,0,0);
    hGr->setSpacing(0);
    hGr->addWidget(m_latencyCombo,5);
    hGr->addWidget(lblGranularity);
    hGr->addWidget(m_granularityCombo,1);
    auto wGr = new QWidget(this);
    wGr->setLayout(hGr);
    form->addRow("Latency:", wGr);

    //===Row[8]=================================================
    auto buttons = new QHBoxLayout;
    buttons->addWidget(m_btnApply);
    buttons->addWidget(m_btnCancel);
    auto layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addLayout(buttons);

    connect(m_backendCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &AudioConfigDialog::onBackendChanged);

    connect(m_outputDeviceCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index)
            {
                int dev = m_outputDeviceCombo->currentData(index).toInt();
                if (dev < 0 || dev >= int(m_outputDeviceInfos.size()))
                {
                    populateDeviceInfo(nullptr);
                }
                else
                {
                    populateDeviceInfo(&m_outputDeviceInfos[ dev ]);
                }
            }
        );

    connect(m_inputDeviceCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index)
            {
                int dev = m_inputDeviceCombo->currentData(index).toInt();
                if (dev < 0 || dev >= int(m_inputDeviceInfos.size()))
                {
                    populateDeviceInfo(nullptr);
                }
                else
                {
                    populateDeviceInfo(&m_inputDeviceInfos[ dev ]);
                }
            }
        );

    connect(btnEnumDevices, &QPushButton::clicked, this, &AudioConfigDialog::enumerateDeviceInfos);

    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_btnApply,  &QPushButton::clicked, this, &AudioConfigDialog::configAccepted);


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

void AudioConfigDialog::populateApis()
{
    m_apiCombo->clear();

    auto backend = (eBackend)m_backendCombo->currentData().toInt();
    if (backend == eBackend::Benni)
    {
        m_apiCombo->addItem("Windows WASAPI (Shared)", 0);
    }
    else if (backend == eBackend::RtAudio)
    {
        std::vector<RtAudio::Api> apis;
        RtAudio::getCompiledApi(apis);

        DE_DEBUG("Api.Count = ",apis.size())
        for (size_t i = 0; i < apis.size(); ++i)
        {
            RtAudio::Api api = apis[i];
            std::string apiName = RtAudio::getApiDisplayName(api);
            m_apiCombo->addItem(
                QString::fromStdString(apiName),
                static_cast<int>(api));

            DE_TRACE("Api[",i,"] ",apiName)
        }
    }
    else
    {
        DE_ERROR("Unknown backend ", int(backend))
    }
}

void AudioConfigDialog::populateInputDevices()
{
    m_inputDeviceCombo->clear();
    m_inputDeviceCombo->addItem("Disabled",-1);

    int found = -1;
    for (size_t i = 0; i < m_inputDeviceInfos.size(); ++i)
    {
        const auto& d = m_inputDeviceInfos[i];

        if (d.isDefault)
        {
            found = i;
        }

        //auto v = QString::fromStdString(d.uid);
        auto s = QString("[%1] %2%3")
            .arg(i)
            .arg(QString::fromStdString(d.name))
            .arg(d.isDefault ? " [default]" : "");
        m_inputDeviceCombo->addItem(s,int(i));
    }

    m_inputDeviceCombo->setCurrentIndex(found);
}

void AudioConfigDialog::populateOutputDevices()
{
    m_outputDeviceCombo->clear();

    int found = -1;
    for (size_t i = 0; i < m_outputDeviceInfos.size(); ++i)
    {
        const auto& d = m_outputDeviceInfos[i];

        if (d.isDefault)
        {
            found = i;
        }

        //auto v = QString::fromStdString(d.uid);
        auto s = QString("[%1] %2%3")
            .arg(i)
            .arg(QString::fromStdString(d.name))
            .arg(d.isDefault ? " [default]" : "");
        m_outputDeviceCombo->addItem(s,int(i));
    }

    m_outputDeviceCombo->setCurrentIndex(found);
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

void AudioConfigDialog::enumerateDeviceInfos()
{
    auto backend = (eBackend)m_backendCombo->currentData().toInt();
    if (backend == eBackend::Benni)
    {
        m_outputDeviceInfos = de::audio::GetOutputDeviceInfos_Wasapi();
        m_inputDeviceInfos = de::audio::GetInputDeviceInfos_Wasapi();
    }
    else // if (backend == eBackend::RtAudio)
    {
        m_outputDeviceInfos.clear();
        m_inputDeviceInfos.clear();
    }

    populateOutputDevices();
    populateInputDevices();
}

void AudioConfigDialog::populateDeviceInfo( const de::audio::DeviceInfo* pdi )
{
    m_channelCountCombo->clear();
    m_firstChannelCombo->clear();
    m_sampleRateCombo->clear();
    m_sampleTypeCombo->clear();
    m_blockSizeCombo->clear();
    m_blockSizeDspCombo->clear();
    m_granularityCombo->clear();
    m_latencyCombo->clear();

    if (!pdi)
    {
        return;
    }

    const de::audio::DeviceInfo& di = *pdi;

    m_channelCountCombo->addItem(QString::number(di.channels), di.channels);
    m_firstChannelCombo->addItem(QString::number(0), 0);

    {
        // SampleRates:
        int found = -1;
        for (size_t i = 0; i < di.sampleRates.size(); ++i)
        {
            auto sr = di.sampleRates[i];
            auto s = QString("%1 Hz").arg(sr);
            if (sr == di.sampleRate)
            {
                s += " [Default]";
                found = i;
            }
            m_sampleRateCombo->addItem(s,int(sr));
        }
        m_sampleRateCombo->setCurrentIndex(found);
    }

    {
        // SampleTypes:
        int found = -1;
        for (size_t i = 0; i < di.sampleTypes.size(); ++i)
        {
            auto st = di.sampleTypes[i];
            auto s = QString::fromStdString(getStr(st));
            if (st == di.sampleType)
            {
                s += " [Default]";
                found = i;
            }
            m_sampleTypeCombo->addItem(s,int(st));
        }
        m_sampleTypeCombo->setCurrentIndex(found);
    }

    // Granularity:
    int granularity = std::max(di.granularity,4u);
    auto gs = QString::number(granularity) + " [AVX2]";
    m_granularityCombo->addItem(gs,granularity);

    {
        // BlockSizes:
        int found = -1;
        uint32_t n = (2048 - di.blockSizeMin) / granularity;
        for (size_t i = 0; i <= n; ++i)
        {
            auto bs = i * granularity + di.blockSizeMin;
            auto s = QString::number(bs) + " frames";
            if (bs == di.blockSizeMin)
            {
                s += " [Min]";
            }
            else if (bs == di.blockSize)
            {
                s += " [Default]";
                found = i;
            }
            m_blockSizeCombo->addItem(s,int(bs));
        }
        m_blockSizeCombo->setCurrentIndex(found);
    }

    {
        // BlockSizesDsp:
        int found = -1;
        uint32_t blockSizeDspMin = 16;
        uint32_t n = (2048 - 16 / granularity);
        for (size_t i = 0; i <= n; ++i)
        {
            auto bs = i * granularity + blockSizeDspMin;
            auto s = QString::number(bs) + " frames";
            if (bs == blockSizeDspMin)
            {
                s += " [Min]";
            }
            else if (bs == 128)
            {
                s += " [Default]";
                found = i;
            }
            m_blockSizeDspCombo->addItem(s,int(bs));
        }
        m_blockSizeDspCombo->setCurrentIndex(found);
    }
}


void AudioConfigDialog::onBackendChanged(int)
{
    populateApis();
    populateOutputDevices();
    populateInputDevices();
}
