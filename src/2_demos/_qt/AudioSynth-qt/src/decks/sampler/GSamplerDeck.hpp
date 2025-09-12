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
// #include <QImageWidget.hpp>
// #include <QFont5x8.hpp>
// #include <GVolume.hpp>
// #include <GLevelMeter.hpp>
// #include <GLowPass3.hpp>
// #include <GSoundTouch.hpp>
// #include <GCrossFader.hpp>
// #include <GOszilloskop.hpp>
// #include <GSpektrometer.hpp>
#include <GDeck.hpp>
#include <de/audio/dsp/IDspChainElement.hpp>

// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GSamplerDeck : public QWidget,
                     public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   GSamplerDeck( QWidget* parent = 0 );
   ~GSamplerDeck();

   uint64_t
   readDspSamples( double pts,
                  float* dst,
                  uint32_t dstFrames, uint32_t dstChannels,
                  double dstRate ) override;
signals:
//   void newSamples( float* samples, uint32_t frameCount, uint32_t channels );
public slots:
   void setMinified(bool minified) { m_menu->setMinified( minified ); }
   void setBypassed(bool bypassed) { m_menu->setBypassed( bypassed ); }
   void setExtraMore(bool minimized) { m_menu->setExtraMore( minimized ); }
protected:
private:
   DE_CREATE_LOGGER("GSamplerDeck")
   GDeck* m_menu;
   QWidget* m_body;
   // GFilePlayer* m_leftPlayer;
   // GLowPass3* m_leftFilter;
   // GSoundTouch* m_leftTouch;
   // GLevelMeter* m_leftLevel;
   // //GOszilloskop* m_leftWaveform;
   // //GSpektrometer* m_leftSpektrum;
   // GFilePlayer* m_rightPlayer;
   // GLowPass3* m_rightFilter;
   // GSoundTouch* m_rightTouch;
   // GLevelMeter* m_rightLevel;
   // //GOszilloskop* m_rightWaveform;
   // //GSpektrometer* m_rightSpektrum;
   // GCrossFader* m_crossFader;
};
