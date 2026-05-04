#pragma once

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <de/audio/device/IEndPoint.h>
#include "AudioTestEngine.h"

class AudioConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AudioConfigDialog(QWidget *parent = nullptr);

signals:
    void configAccepted();

private slots:
    void onBackendChanged(int index);
    void enumerateDeviceInfos();

private:
    void populateApis();
    void populateInputDevices();
    void populateOutputDevices();
    void populateDeviceInfo( const de::audio::DeviceInfo* di );
    void populateStaticOptions();

    QComboBox* m_backendCombo;
    QComboBox* m_apiCombo;

    QComboBox* m_inputDeviceCombo;
    QComboBox* m_outputDeviceCombo;

    QComboBox* m_channelCountCombo;
    QComboBox* m_firstChannelCombo;

    QComboBox* m_sampleRateCombo;
    QComboBox* m_sampleTypeCombo;

    QComboBox* m_blockSizeDspCombo;
    QComboBox* m_blockSizeCombo;

    QComboBox* m_granularityCombo;
    QComboBox* m_latencyCombo;

    QPushButton* m_btnApply;
    QPushButton* m_btnCancel;
	
    std::vector<de::audio::DeviceInfo> m_inputDeviceInfos;
    std::vector<de::audio::DeviceInfo> m_outputDeviceInfos;

// <TestEngine>
	AudioTestEngine m_testEngine;

    QPushButton* m_btnTestSine;
    QPushButton* m_btnTestClick;
    QPushButton* m_btnTestStop;
// </TestEngine>
};
