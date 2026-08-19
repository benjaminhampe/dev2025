#include "ControlWidget_fft.h"

ControlWidget_fft::ControlWidget_fft(QWidget *parent)
    : HideOnCloseWidget(parent)
{
    auto *layout = new QFormLayout(this);

    // FFT size
    fftSizeBox = new QComboBox(this);
    fftSizeBox->addItems({"128", "256", "512", "1024", "2048", "4096", "8192"});
    fftSizeBox->setCurrentText("1024");

    // Window type
    windowBox = new QComboBox(this);
    windowBox->addItems({"Rectangular", "Hann", "Hamming", "Blackman", "Blackman-Harris"});

    // Sampling rate
    sampleRateBox = new QSpinBox(this);
    sampleRateBox->setRange(8000, 192000);
    sampleRateBox->setValue(48000);
    sampleRateBox->setSingleStep(1000);

    // Overlap
    overlapBox = new QSpinBox(this);
    overlapBox->setRange(0, 90);
    overlapBox->setValue(50);
    overlapBox->setSuffix(" %");

    // Data size (number of samples)
    dataSizeBox = new QSpinBox(this);
    dataSizeBox->setRange(128, 1'000'000);
    dataSizeBox->setValue(4096);
    dataSizeBox->setSingleStep(256);

    // Run button
    runButton = new QPushButton("Run FFT", this);

    // Add to layout
    layout->addRow("FFT Size:", fftSizeBox);
    layout->addRow("Window:", windowBox);
    layout->addRow("Sample Rate:", sampleRateBox);
    layout->addRow("Overlap:", overlapBox);
    layout->addRow("Data Size:", dataSizeBox);
    layout->addRow(runButton);

    // Connect signals
    connect(runButton, &QPushButton::clicked, this, &ControlWidget_fft::fftRequested);
}

// Accessors
int ControlWidget_fft::fftSize() const { return fftSizeBox->currentText().toInt(); }
QString ControlWidget_fft::windowType() const { return windowBox->currentText(); }
int ControlWidget_fft::sampleRate() const { return sampleRateBox->value(); }
int ControlWidget_fft::overlap() const { return overlapBox->value(); }
int ControlWidget_fft::dataSize() const { return dataSizeBox->value(); }
