#include "MidiConfigDialog.h"
#include <RtMidi/RtMidi.h>

namespace {

inline std::string getApiDisplayName(RtMidi::Api api)
{
    switch (api)
    {
        case RtMidi::UNSPECIFIED: return "Unspecified";
        case RtMidi::MACOSX_CORE: return "CoreMIDI";
        case RtMidi::LINUX_ALSA:  return "ALSA";
        case RtMidi::UNIX_JACK:   return "JACK";
        case RtMidi::WINDOWS_MM:  return "Windows MM";
        case RtMidi::RTMIDI_DUMMY:return "Dummy";
        default:                  return "Unknown";
    }
}

} // end namespace.

MidiConfigDialog::MidiConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    m_apiCombo = new QComboBox(this);
    m_inputCombo = new QComboBox(this);
    m_outputCombo = new QComboBox(this);

    populateApis();
    populateInputPorts();
    populateOutputPorts();

    auto layout = new QFormLayout;
    layout->addRow("MIDI API:", m_apiCombo);
    layout->addRow("Input Device:", m_inputCombo);
    layout->addRow("Output Device:", m_outputCombo);

    setLayout(layout);

    connect(m_apiCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                auto api = static_cast<RtMidi::Api>(m_apiCombo->currentData().toInt());
                //emit apiChanged(api);
                populateInputPorts();
                populateOutputPorts();
            });

    connect(m_inputCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                //emit inputPortChanged(m_inputCombo->currentData().toUInt());
            });

    connect(m_outputCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                //emit outputPortChanged(m_outputCombo->currentData().toUInt());
            });
}

void MidiConfigDialog::populateApis()
{
    m_apiCombo->clear();

    std::vector<RtMidi::Api> apis;
    RtMidi::getCompiledApi(apis);

    for (auto api : apis)
    {
        auto s = QString::fromStdString(getApiDisplayName(api));
        m_apiCombo->addItem(s, static_cast<int>(api));
    }
}

void MidiConfigDialog::populateInputPorts()
{
    m_inputCombo->clear();

    auto api = static_cast<RtMidi::Api>(m_apiCombo->currentData().toInt());
    RtMidiIn midiIn(api);

    unsigned int count = midiIn.getPortCount();
    for (unsigned int i = 0; i < count; ++i)
    {
        auto s = QString::fromStdString(midiIn.getPortName(i));
        m_inputCombo->addItem(s, i);
    }
}

void MidiConfigDialog::populateOutputPorts()
{
    m_outputCombo->clear();

    auto api = static_cast<RtMidi::Api>(m_apiCombo->currentData().toInt());
    RtMidiOut midiOut(api);

    unsigned int count = midiOut.getPortCount();
    for (unsigned int i = 0; i < count; ++i)
    {
        auto s = QString::fromStdString(midiOut.getPortName(i));
        m_outputCombo->addItem(s, i);
    }
}
