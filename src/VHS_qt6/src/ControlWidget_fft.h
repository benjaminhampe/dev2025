#pragma once
#include <HideOnCloseWidget.h>
#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QGroupBox>

class ControlWidget_fft : public HideOnCloseWidget
{
    Q_OBJECT
public:
    explicit ControlWidget_fft(QWidget *parent = nullptr);
    // Accessors
    int fftSize() const;
    QString windowType() const;
    int sampleRate() const;
    int overlap() const;
    int dataSize() const;
signals:
    void fftRequested();
private:
    QComboBox *fftSizeBox;
    QComboBox *windowBox;
    QSpinBox *sampleRateBox;
    QSpinBox *overlapBox;
    QSpinBox *dataSizeBox;
    QPushButton *runButton;
};

/*
auto *controls = new FftControlWidget;
setCentralWidget(controls);

connect(controls, &FftControlWidget::fftRequested, this, [controls]() {
    qDebug() << "FFT size:" << controls->fftSize();
    qDebug() << "Window:" << controls->windowType();
    qDebug() << "Sample rate:" << controls->sampleRate();
    qDebug() << "Overlap:" << controls->overlap();
    qDebug() << "Data size:" << controls->dataSize();
});

*/
