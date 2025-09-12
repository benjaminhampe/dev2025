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
#include <QFont5x8.hpp>
#include <de/audio/dsp/IDspChainElement.hpp>
#include "GSliderV.hpp"
#include "GGroupV.hpp"

namespace de {
namespace audio {

// ============================================================================
struct Delay : public IDspChainElement
// ============================================================================
{
   DE_CREATE_LOGGER("de.audio.Delay")
public:
   IDspChainElement* m_inputSignal;
   bool m_isBypassed;
   int m_volume; // Volume in [pc]
   uint32_t m_sampleRate; // in [Hz] = [1/sec]
   float m_delay; // Delay in [sec]
   //std::vector< float > m_inputBuffer;
   ShiftBuffer m_delayBuffer;

   Delay();
   ~Delay() override;

   uint64_t
   readDspSamples( double pts,
                  float* dst,
                  uint32_t dstFrames,
                  uint32_t dstChannels,
                  double dstRate ) override;

   void clearInputSignals() override;
   void setInputSignal( int i, IDspChainElement* input ) override;

   bool isBypassed() const; // override;
   void setBypassed( bool enabled ); // override;

   int getVolume() const override;
   void setVolume( int vol ) override;

   float getDelayInMs() const; // override;

   void setDelayInMs( float ms ); // override;

};

} // end namespace audio
} // end namespace de

// ============================================================================
class GDelay : public QWidget
// ============================================================================
{
   Q_OBJECT
   DE_CREATE_LOGGER("GDelay")
public:
   GDelay( QWidget* parent = 0 );
   ~GDelay();

   de::audio::Delay* dsp()
   {
      return &m_delay;
   }

signals:
public slots:

   void setBypassed( bool enabled )
   {
      m_menu->setBypassed( enabled );
   }
   void setMinified( bool minified )
   {
      m_menu->setMinified( minified );
   }
protected:
//   void timerEvent( QTimerEvent* event) override;
//   void paintEvent( QPaintEvent* event ) override;

private:
   de::audio::Delay m_delay;
   GGroupV* m_menu;
   QWidget* m_body;
   GSliderV* m_delaySlider;
   GSliderV* m_volumeSlider;
   QFont5x8 m_font5x8;

protected slots:
   void on_toggledHideAll(bool checked)
   {
      //m_delaySlider->setVisible( !checked );
      //m_volumeSlider->setVisible( !checked );
   }
   void on_toggledBypass(bool checked)
   {
      m_delay.setBypassed( checked );
   }
   void on_toggledMore(bool checked)
   {
      //m_delaySlider->setVisible( checked );
   }
   void on_delayChanged(int value)
   {
      m_delay.setDelayInMs( value );
   }
   void on_volumeChanged(int value)
   {
      m_delay.setVolume( value );
   }

};
