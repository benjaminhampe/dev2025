#pragma once
#include <HideOnCloseWidget.h>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

class ControlWidget_CameraScale : public HideOnCloseWidget
{
    Q_OBJECT

public:
    explicit ControlWidget_CameraScale(QWidget *parent = nullptr);

    struct AxisControl
    {
        QLabel *label2;
        QVBoxLayout *layout;
        QSlider *slider;
        QLabel *label;
    };
    void createAxis(AxisControl & ac, const QString &name);

    int x() const;
    int y() const;
    int z() const;

signals:
    void positionChanged(float x, float y, float z);

private slots:
    //void updateLabels();
    void onValueChanged();

private:
    AxisControl m_x;
    AxisControl m_y;
    AxisControl m_z;
};

/*
auto *posWidget = new CameraPositionWidget;
setCentralWidget(posWidget);

connect(posWidget, &CameraPositionWidget::positionChanged,
        [](int x, int y, int z){
            qDebug() << "Camera pos:" << x << y << z;
        });

*/
