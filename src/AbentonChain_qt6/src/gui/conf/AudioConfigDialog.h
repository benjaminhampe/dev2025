#pragma once

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

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
    void refreshEnumeration();

private:
    void populateBackends();
    void populateApis();
    void populateDevices();
    void populateStaticOptions();

    QComboBox *m_backendCombo;
    QComboBox *m_apiCombo;
    QComboBox *m_channelCountCombo;
    QComboBox *m_sampleRateCombo;
    QComboBox *m_blockSizeCombo;
    QComboBox *m_firstChannelCombo;
    QComboBox *m_inputDeviceCombo;
    QComboBox *m_outputDeviceCombo;

    QPushButton *m_btnApply;
    QPushButton *m_btnRefresh;
    QPushButton *m_btnCancel;
	
	
// <TestEngine>
	AudioTestEngine m_testEngine;

	QPushButton *m_btnTestSine;
	QPushButton *m_btnTestClick;
	QPushButton *m_btnTestStop;
// </TestEngine>
};
