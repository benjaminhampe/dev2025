#include <ControlWidget_audioScale.h>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

ControlWidget_AudioScale::ControlWidget_AudioScale(QWidget *parent)
    : HideOnCloseWidget(parent)
{
    //
    // Create axis controls
    //
    createAxis(m_gain, "Gain");
    createAxis(m_volume, "Volume");
    createAxis(m_dBmin, "FFT dB Min");
    createAxis(m_dBmax, "FFT dB Max");
    m_gain.slider->setRange(0, 1000);
    m_gain.slider->setValue(100);
    m_volume.slider->setRange(0, 1000);
    m_volume.slider->setValue(100);
    m_dBmin.slider->setRange(-240, 240);
    m_dBmin.slider->setValue(-60);
    m_dBmax.slider->setRange(-240, 240);
    m_dBmax.slider->setValue(60);

    auto boxL = new QHBoxLayout();
    boxL->setContentsMargins(0,0,0,0);
    boxL->addLayout(m_gain.layout);
    boxL->addLayout(m_volume.layout);
    boxL->addLayout(m_dBmin.layout);
    boxL->addLayout(m_dBmax.layout);

    //
    // Decorative box
    //
    auto box = new QGroupBox("Parameters:", this);
    box->setLayout(boxL);

    auto outerLayout = new QVBoxLayout(this);
    outerLayout->addWidget(box);

    //
    // Connect updates
    //
    connect(m_gain.slider, &QSlider::valueChanged, this,
            &ControlWidget_AudioScale::on_gainChanged);
    connect(m_volume.slider, &QSlider::valueChanged, this,
            &ControlWidget_AudioScale::on_volumeChanged);
    connect(m_dBmin.slider, &QSlider::valueChanged, this,
            &ControlWidget_AudioScale::on_dBminChanged);
    connect(m_dBmax.slider, &QSlider::valueChanged, this,
            &ControlWidget_AudioScale::on_dBmaxChanged);

    //updateLabels();
}
int ControlWidget_AudioScale::gain() const { return m_gain.slider->value(); }
int ControlWidget_AudioScale::volume() const { return m_volume.slider->value(); }
int ControlWidget_AudioScale::dBmin() const { return m_dBmin.slider->value(); }
int ControlWidget_AudioScale::dBmax() const { return m_dBmax.slider->value(); }

void ControlWidget_AudioScale::on_gainChanged()
{
    m_gain.label->setText(QString::number(0.01f*gain())+"x");
    emit gainChanged(gain());
}

void ControlWidget_AudioScale::on_volumeChanged()
{
    m_volume.label->setText(QString::number(volume())+"%");
    emit volumeChanged(volume());
}

void ControlWidget_AudioScale::on_dBminChanged()
{
    m_dBmin.label->setText(QString::number(dBmin())+"dB");
    emit dBminChanged(dBmin());
}

void ControlWidget_AudioScale::on_dBmaxChanged()
{
    m_dBmax.label->setText(QString::number(dBmax())+"dB");
    emit dBmaxChanged(dBmax());
}
void
ControlWidget_AudioScale::createAxis(AxisControl & ac, const QString &name)
{
    ac.layout = new QVBoxLayout;
    ac.layout->setAlignment(Qt::AlignHCenter);

    QLabel *title = new QLabel(name);
    title->setAlignment(Qt::AlignCenter);

    ac.slider = new QSlider(Qt::Vertical);

    ac.label = new QLabel("0");
    ac.label->setAlignment(Qt::AlignCenter);

    ac.layout->addWidget(title);
    ac.layout->addWidget(ac.slider);
    ac.layout->addWidget(ac.label);

    //labelOut = ac.label;
}

/*
void ControlWidget_AudioScale::updateLabels()
{
    xLabel->setText(QString::number(x()));
    yLabel->setText(QString::number(y()));
    zLabel->setText(QString::number(z()));

    emit positionChanged(x(), y(), z());
}
*/
