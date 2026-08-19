#include <ControlWidget_cameraPos.h>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

ControlWidget_CameraPos::ControlWidget_CameraPos(QWidget *parent)
    : HideOnCloseWidget(parent)
{
    //
    // Create axis controls
    //
    xSlider = createAxis("X", xLabel);
    ySlider = createAxis("Y", yLabel);
    zSlider = createAxis("Z", zLabel);
    //
    // Decorative box
    //
    auto *box = new QGroupBox("Camera Position", this);
    auto *boxLayout = new QHBoxLayout(box);
    boxLayout->setContentsMargins(0,0,0,0);
    boxLayout->addLayout(xSlider.layout);
    boxLayout->addLayout(ySlider.layout);
    boxLayout->addLayout(zSlider.layout);

    auto *outerLayout = new QVBoxLayout();
    outerLayout->setContentsMargins(0,0,0,0);
    outerLayout->addWidget(box);
    this->setLayout(outerLayout);

    //
    // Connect updates
    //
    connect(xSlider.slider, &QSlider::valueChanged, this, &ControlWidget_CameraPos::updateLabels);
    connect(ySlider.slider, &QSlider::valueChanged, this, &ControlWidget_CameraPos::updateLabels);
    connect(zSlider.slider, &QSlider::valueChanged, this, &ControlWidget_CameraPos::updateLabels);

    updateLabels();
}

AxisControl ControlWidget_CameraPos::createAxis(const QString &name, QLabel *&labelOut)
{
    AxisControl ac;

    ac.layout = new QVBoxLayout;
    ac.layout->setAlignment(Qt::AlignHCenter);

    QLabel *title = new QLabel(name);
    title->setAlignment(Qt::AlignCenter);

    ac.slider = new QSlider(Qt::Vertical);
    ac.slider->setRange(-1000, 1000);
    ac.slider->setValue(0);

    ac.label = new QLabel("0");
    ac.label->setAlignment(Qt::AlignCenter);

    ac.layout->addWidget(title);
    ac.layout->addWidget(ac.slider);
    ac.layout->addWidget(ac.label);

    labelOut = ac.label;
    return ac;
}

int ControlWidget_CameraPos::x() const { return xSlider.slider->value(); }
int ControlWidget_CameraPos::y() const { return ySlider.slider->value(); }
int ControlWidget_CameraPos::z() const { return zSlider.slider->value(); }

void ControlWidget_CameraPos::updateLabels()
{
    xLabel->setText(QString::number(x()));
    yLabel->setText(QString::number(y()));
    zLabel->setText(QString::number(z()));

    emit positionChanged(x(), y(), z());
}
