#pragma once
#include <HideOnCloseWidget.h>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

class ControlWidget_AudioScale : public HideOnCloseWidget
{
    Q_OBJECT
public:
    explicit ControlWidget_AudioScale(QWidget *parent = nullptr);

    struct AxisControl
    {
        QLabel *label2;
        QVBoxLayout *layout;
        QSlider *slider;
        QLabel *label;
    };
    void createAxis(AxisControl & ac, const QString &name);
    int gain() const;
    int volume() const;
    int dBmin() const;
    int dBmax() const;
signals:
    void gainChanged(int);
    void volumeChanged(int);
    void dBminChanged(int);
    void dBmaxChanged(int);
private slots:
    void on_gainChanged();
    void on_volumeChanged();
    void on_dBminChanged();
    void on_dBmaxChanged();

private:
    AxisControl m_gain;
    AxisControl m_volume;
    AxisControl m_dBmin;
    AxisControl m_dBmax;
};

/*
auto *posWidget = new CameraPositionWidget;
setCentralWidget(posWidget);

connect(posWidget, &CameraPositionWidget::positionChanged,
        [](int x, int y, int z){
            qDebug() << "Camera pos:" << x << y << z;
        });

*/
