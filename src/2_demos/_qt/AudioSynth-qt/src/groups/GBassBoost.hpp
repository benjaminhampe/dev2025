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
#include <QImageWidget.hpp>
#include <QFont5x8.hpp>
#include "GSliderV.hpp"
#include "GGroupV.hpp"
#include <de/audio/dsp/DspLowPass_IIR_Biquad.hpp>

// ============================================================================
class GBassBoost : public QWidget //, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   GBassBoost( QWidget* parent = 0 );
   ~GBassBoost() override;

   de::audio::LowPass_IIR_Biquad* dsp() { return &m_lp; }
/*
   uint64_t
   readDspSamples( double pts, float* d, uint32_t fr, uint32_t ch, double rate ) override
   {
      return m_lp.readDspSamples( pts, d, fr, ch, rate );
   }
*/
signals:
public slots:
   void setBypassed( bool bypassed )
   {
      m_menu->setBypassed( bypassed );
      m_lp.setBypassed( bypassed );
   }
   void setMinified( bool minified )
   {
      m_menu->setMinified( minified );
   }
/*
   void setInputSignal( int i, de::audio::IDspChainElement* input) override
   {
      m_lp.setInputSignal( i, input );
   }

   void clearInputSignals() override
   {
      m_lp.clearInputSignals();
   }
*/
protected:
   DE_CREATE_LOGGER("GBassBoost")
   de::audio::LowPass_IIR_Biquad m_lp;
   GGroupV* m_menu;
   QImageWidget* m_body;
   GSliderV* m_freq;
   GSliderV* m_qpeak;
   GSliderV* m_volume;

protected slots:
   void on_toggledHideAll(bool checked)
   {
//      if ( checked )
//      {
//         m_qpeak->setVisible( false );
//         m_volume->setVisible( false );
//      }
//      else
//      {
//         m_qpeak->setVisible( true );
//         if ( m_menu->isExtraMore() )
//         {
//            m_volume->setVisible( true );
//         }
//      }
   }
   void on_toggledBypass(bool checked)
   {
      m_lp.setBypassed( checked );
   }
   void on_toggledMore(bool checked)
   {
      //m_qpeak->setVisible( checked );
      m_volume->setVisible( checked );
   }
   void on_valueChanged_f(int value)
   {
      m_lp.setFrequency( float( value ) );
   }
   void on_valueChanged_Q(int value)
   {
      m_lp.setQ( 0.01f * value );
   }
   void on_valueChanged_vol(int value)
   {
      m_lp.setVolume( value ); // Boost volume, can be over 100%, ca. 300%.
   }
};
