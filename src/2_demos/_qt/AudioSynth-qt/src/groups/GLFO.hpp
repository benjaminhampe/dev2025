#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QThread>

#include <QSlider>
#include <QDial>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include <de/audio/dsp/MultiOSC.hpp> // LFO

#include "GSliderV.hpp"
#include "GGroupV.hpp"

// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GLFO : public QWidget
// ============================================================================
{
   Q_OBJECT
public:
   GLFO( QString title, QWidget* parent = 0 );
   ~GLFO();

signals:
   //void newSamples( float* samples, uint32_t frameCount, uint32_t channels );
public slots:
   //void clearInputSignals();
   //void setInputSignal( de::audio::IDspChainElement* input );
   //void setBypassed( bool enabled );
   //void setVolume( int volume_in_pc );
   //void setChannel( int channel = -1 );
   void setLFO( de::audio::MultiOSCf* lfo );
   void setValues( int type, double rate, double center, double range = 0.0 );
   void setMinified(bool visible) { m_menu->setMinified( visible ); }
   void setBypassed(bool bypassed) { m_menu->setBypassed( bypassed ); }
private slots:
   void on_menuButton_bypass(bool);
   void on_menuButton_more(bool);
   void on_typeChanged( int v );
   void on_rateChanged( double v );
   void on_centerChanged( double v );
   void on_rangeChanged( double v );
private:
   DE_CREATE_LOGGER("GLFO")
public:
   de::audio::MultiOSCf* m_LFO;
   GGroupV* m_menu;
   QImageWidget* m_body;

   //GSliderV* m_

   QLabel* m_typeLabel;
   QComboBox* m_typeEdit;
   QLabel* m_rateLabel;
   QDoubleSpinBox* m_rateEdit;
   QLabel* m_centerLabel;
   QDoubleSpinBox* m_centerEdit;
   QLabel* m_rangeLabel;
   QDoubleSpinBox* m_rangeEdit;
};
