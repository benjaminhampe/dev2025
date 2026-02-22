#pragma once
#include <HideOnCloseWidget.h>

#include <QWidget>
#include <QComboBox>
#include <QFormLayout>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
//#include <QCamera>
//#include <QMediaCaptureSession>
//#include <QCameraDevice>
//#include <QVideoSink>
#include <QLabel>

// for (const DE_CameraDevice &dev : DE_MediaDevices::videoInputs()) {

struct QCameraDevice
{
    QString m_description = "QCameraDevice";

    bool m_isDefault = false;

    QString description() const { return m_description; }

    bool isDefault() const { return m_isDefault; }
};

struct QMediaDevices
{
    static std::vector<QCameraDevice> videoInputs()
    {
        std::vector<QCameraDevice> videoInputs;
        return videoInputs;
    }
};

class ControlWidget_Camera : public HideOnCloseWidget
{
    Q_OBJECT
    QComboBox *deviceBox;
    QComboBox *resolutionBox;
    QComboBox *fpsBox;
    QCheckBox *autoExposure;
    QSlider *exposureSlider;
    QSlider *gainSlider;
    QPushButton *startButton;

    //QCamera *camera = nullptr;
    //QMediaCaptureSession captureSession;
    //QVideoSink videoSink;

public:
    explicit ControlWidget_Camera(QWidget *parent = nullptr);
signals:
    void cameraStarted();
    void cameraStopped();
private slots:
    void updateDeviceCapabilities();
    void toggleCamera();
    void updateExposureMode(bool enabled);
private:
    void startCamera();
    void stopCamera();
};
