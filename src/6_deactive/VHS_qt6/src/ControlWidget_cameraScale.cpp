#include <ControlWidget_cameraScale.h>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

ControlWidget_CameraScale::ControlWidget_CameraScale(QWidget *parent)
    : HideOnCloseWidget(parent)
{
    //
    // Create axis controls
    //
    createAxis(m_x, "X");
    createAxis(m_y, "Y");
    createAxis(m_z, "Z");

    auto boxL = new QHBoxLayout();
    boxL->setContentsMargins(0,0,0,0);
    boxL->addLayout(m_x.layout);
    boxL->addLayout(m_y.layout);
    boxL->addLayout(m_z.layout);

    //
    // Decorative box
    //
    auto box = new QGroupBox("Camera Scale", this);
    box->setLayout(boxL);

    auto outerLayout = new QVBoxLayout(this);
    outerLayout->addWidget(box);

    //
    // Connect updates
    //
    connect(m_x.slider, &QSlider::valueChanged, this,
            &ControlWidget_CameraScale::onValueChanged);
    connect(m_y.slider, &QSlider::valueChanged, this,
            &ControlWidget_CameraScale::onValueChanged);
    connect(m_z.slider, &QSlider::valueChanged, this,
            &ControlWidget_CameraScale::onValueChanged);

    //updateLabels();
}
int ControlWidget_CameraScale::x() const { return m_x.slider->value(); }
int ControlWidget_CameraScale::y() const { return m_y.slider->value(); }
int ControlWidget_CameraScale::z() const { return m_z.slider->value(); }

void ControlWidget_CameraScale::onValueChanged()
{
    m_x.label->setText(QString::number(x()));
    m_y.label->setText(QString::number(y()));
    m_z.label->setText(QString::number(z()));

    emit positionChanged(x(), y(), z());
}

void
ControlWidget_CameraScale::createAxis(AxisControl & ac, const QString &name)
{
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

    //labelOut = ac.label;
}


/*
void ControlWidget_CameraScale::updateLabels()
{
    xLabel->setText(QString::number(x()));
    yLabel->setText(QString::number(y()));
    zLabel->setText(QString::number(z()));

    emit positionChanged(x(), y(), z());
}
*/
