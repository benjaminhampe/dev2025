#pragma once
#include <HideOnCloseWidget.h>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>


struct AxisControl
{
    QVBoxLayout *layout;
    QSlider *slider;
    QLabel *label;
};

class ControlWidget_CameraPos : public HideOnCloseWidget
{
    Q_OBJECT

public:
    explicit ControlWidget_CameraPos(QWidget *parent = nullptr);

    AxisControl createAxis(const QString &name, QLabel *&labelOut);

    int x() const;
    int y() const;
    int z() const;

signals:
    void positionChanged(int x, int y, int z);

private slots:
    void updateLabels();
private:
    AxisControl xSlider;
    AxisControl ySlider;
    AxisControl zSlider;

    QLabel *xLabel;
    QLabel *yLabel;
    QLabel *zLabel;
};

/*
auto *posWidget = new CameraPositionWidget;
setCentralWidget(posWidget);

connect(posWidget, &CameraPositionWidget::positionChanged,
        [](int x, int y, int z){
            qDebug() << "Camera pos:" << x << y << z;
        });

*/
