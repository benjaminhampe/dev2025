#include <ControlWidget_camera.h>

ControlWidget_Camera::ControlWidget_Camera(QWidget *parent)
    : HideOnCloseWidget(parent)
{
    auto *layout = new QFormLayout(this);

    //
    // Device selection
    //
    deviceBox = new QComboBox(this);
    for (const QCameraDevice &dev : QMediaDevices::videoInputs())
    {
        QString label = dev.description();
        if (dev.isDefault())
            label += " (Default)";
        deviceBox->addItem(label, QVariant::fromValue(dev));
    }
    layout->addRow("Camera:", deviceBox);

    //
    // Resolution
    //
    resolutionBox = new QComboBox(this);
    layout->addRow("Resolution:", resolutionBox);

    //
    // Frame rate
    //
    fpsBox = new QComboBox(this);
    layout->addRow("FPS:", fpsBox);

    //
    // Auto exposure
    //
    autoExposure = new QCheckBox("Auto Exposure", this);
    autoExposure->setChecked(true);
    layout->addRow(autoExposure);

    //
    // Exposure slider
    //
    exposureSlider = new QSlider(Qt::Horizontal, this);
    exposureSlider->setRange(-10, 10);
    exposureSlider->setEnabled(false);
    layout->addRow("Exposure:", exposureSlider);

    //
    // Gain slider
    //
    gainSlider = new QSlider(Qt::Horizontal, this);
    gainSlider->setRange(0, 100);
    layout->addRow("Gain:", gainSlider);

    //
    // Start/Stop button
    //
    startButton = new QPushButton("Start Camera", this);
    layout->addRow(startButton);

    //
    // Camera setup
    //
    connect(deviceBox, &QComboBox::currentIndexChanged,
            this, &ControlWidget_Camera::updateDeviceCapabilities);

    connect(startButton, &QPushButton::clicked,
            this, &ControlWidget_Camera::toggleCamera);

    connect(autoExposure, &QCheckBox::toggled,
            this, &ControlWidget_Camera::updateExposureMode);

    updateDeviceCapabilities();
}

void ControlWidget_Camera::updateDeviceCapabilities()
{
    /*
    resolutionBox->clear();
    fpsBox->clear();

    QCameraDevice dev = deviceBox->currentData().value<QCameraDevice>();

    for (const QCameraFormat &fmt : dev.videoFormats()) {
        QString res = QString("%1x%2")
                          .arg(fmt.resolution().width())
                          .arg(fmt.resolution().height());
        resolutionBox->addItem(res, QVariant::fromValue(fmt));
    }

    if (resolutionBox->count() > 0)
        resolutionBox->setCurrentIndex(0);

    // FPS list
    if (resolutionBox->count() > 0) {
        QCameraFormat fmt = resolutionBox->currentData().value<QCameraFormat>();
        for (float fps : fmt.frameRateRange()) {
            fpsBox->addItem(QString::number(fps), fps);
        }
    }
    */
}

void ControlWidget_Camera::toggleCamera()
{
    /*
    if (!camera) {
        startCamera();
    } else {
        stopCamera();
    }
    */
}

void ControlWidget_Camera::updateExposureMode(bool enabled)
{
    /*
    if (!camera)
        return;

    if (enabled) {
        camera->setExposureMode(QCamera::ExposureAuto);
        exposureSlider->setEnabled(false);
    } else {
        camera->setExposureMode(QCamera::ExposureManual);
        exposureSlider->setEnabled(true);
    }
    */
}

void ControlWidget_Camera::startCamera()
{
    /*
    QCameraDevice dev = deviceBox->currentData().value<QCameraDevice>();
    camera = new QCamera(dev, this);

    QCameraFormat fmt = resolutionBox->currentData().value<QCameraFormat>();
    camera->setCameraFormat(fmt);

    captureSession.setCamera(camera);
    captureSession.setVideoSink(&videoSink);

    camera->start();

    startButton->setText("Stop Camera");
    emit cameraStarted();
    */
}

void ControlWidget_Camera::stopCamera()
{
    /*
    if (!camera)
        return;

    camera->stop();
    camera->deleteLater();
    camera = nullptr;

    startButton->setText("Start Camera");
    emit cameraStopped();
    */
}
