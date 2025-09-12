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

/// 2nd order biquad IIR lowpass ( IIR = uses output, which makes the response infinite )
/// @TODO: Extract filter and use the one in GBassBoost
// ============================================================================
class GLowPass3 : public QWidget //, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   GLowPass3( QWidget* parent = 0 );
   ~GLowPass3();

   de::audio::LowPass_IIR_Biquad* dsp() { return &m_lpf; }
   /*
   uint64_t
   readDspSamples( double pts, float* dst,
                uint32_t dstFrames, uint32_t dstChannels,
                double dstRate ) override;
   void computeCoefficients( uint32_t sampleRate, float freq, float q = 0.707071 );
   */
signals:
public slots:
//   void setInputSignal(int i, de::audio::IDspChainElement* input) { m_inputSignal = input; }
//   void clearInputSignals() { m_inputSignal = nullptr; }
   void setMinified(bool minified)
   {
      m_menu->setMinified( minified );
   }
   void setBypassed(bool bypassed)
   {
      m_menu->setBypassed( bypassed );
      m_lpf.setBypassed( bypassed );
   }
protected:
   DE_CREATE_LOGGER("GLowPass3")
   GGroupV* m_menu;
   QImageWidget* m_body;
   GSliderV* m_freq;
   GSliderV* m_qpeak;
   GSliderV* m_volume;

   de::audio::LowPass_IIR_Biquad m_lpf;
   /*
   de::audio::IDspChainElement* m_inputSignal;
   std::vector< float > m_inputBuffer;
   bool m_isBypassed;
   // Coefficients and z^-1 delays for input and output ( IIR )
   // y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
   float m_b0;
   float m_b1;
   float m_b2;
   float m_a1;
   float m_a2;
   float m_x1L; // z^-1 delay register for input signal L
   float m_x2L; // z^-1 delay register for input signal L
   float m_y1L; // z^-1 delay register for output signal L
   float m_y2L; // z^-1 delay register for output signal L
   float m_x1R; // z^-1 delay register for input signal R
   float m_x2R; // z^-1 delay register for input signal R
   float m_y1R; // z^-1 delay register for output signal R
   float m_y2R; // z^-1 delay register for output signal R
   */
protected slots:
   void on_toggledBypass(bool checked)
   {
      m_lpf.setBypassed( checked );
   }
   void on_toggledMore(bool checked)
   {
      m_volume->setVisible( checked );
   }
   void on_toggledHideAll(bool checked)
   {
      //   if ( checked )
      //   {
      //      m_qpeak->setVisible( false );
      //      m_freq->setVisible( false );
      //      m_volume->setVisible( false );
      //   }
      //   else
      //   {
      //      m_qpeak->setVisible( true );
      //      m_freq->setVisible( true );
      //      if ( m_menu->isExtraMore() )
      //      {
      //         m_volume->setVisible( true );
      //      }
      //   }
   }
   void on_valueChanged_f(int value)
   {
      setBypassed( value < 1 );
   }
   void on_valueChanged_Q(int value)
   {
      m_qpeak->setValue( value );
   }
   void on_valueChanged_vol(int value)
   {
      m_volume->setValue( value ); // Boost volume, can be over 100%, ca. 300%.
   }
};
