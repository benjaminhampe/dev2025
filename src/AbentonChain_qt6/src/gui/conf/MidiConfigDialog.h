#pragma once

#include <QDialog>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

class MidiConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MidiConfigDialog(QWidget* parent = nullptr);

signals:
    //void apiChanged(RtMidi::Api api);
    //void inputPortChanged(unsigned int port);
    //void outputPortChanged(unsigned int port);

private:
    void populateApis();
    void populateInputPorts();
    void populateOutputPorts();

    QComboBox* m_apiCombo;
    QComboBox* m_inputCombo;
    QComboBox* m_outputCombo;
};

/*
auto *config = new MidiConfigDialog;

connect(config, &MidiConfigDialog::apiChanged,
        this, [](RtMidi::Api api) {
            qDebug() << "Selected API:" << RtMidi::getApiDisplayName(api).c_str();
        });

connect(config, &MidiConfigDialog::inputPortChanged,
        this, [](unsigned int port) {
            qDebug() << "Selected input port:" << port;
        });

connect(config, &MidiConfigDialog::outputPortChanged,
        this, [](unsigned int port) {
            qDebug() << "Selected output port:" << port;
        });
*/