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
#include <QFont5x8.hpp>
#include <QImageWidget.hpp>
#include <GVolume.hpp>
#include <GLevelMeter.hpp>
#include <GLowPass3.hpp>
#include <GSoundTouch.hpp>
#include <GOszilloskop.hpp>
#include <GSpektrometer.hpp>
#include <GDeck.hpp>
#include "GDetuneSynth.hpp"
#include <de/audio/dsp/DspMixer.h>

// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GDetuneSynthDeck : public QImageWidget, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   GDetuneSynthDeck( QWidget* parent = 0 );
   ~GDetuneSynthDeck();
   uint64_t
   readDspSamples(
      double pts,
      float* dst,
      uint32_t dstFrames,
      uint32_t dstChannels,
      double dstRate ) override;
   bool isSynth() const override { return true; }
signals:
//   void newSamples( float* samples, uint32_t frameCount, uint32_t channels );
public slots:
   void sendNote( de::audio::Note const & note ) override;
   void setMinified(bool minified) { m_menu->setMinified( minified ); }
   void setBypassed(bool bypassed) { m_menu->setBypassed( bypassed ); }
   void setExtraMore(bool visible) { m_menu->setExtraMore( visible ); }
private:
   DE_CREATE_LOGGER("GDetuneSynthDeck")
   GDeck* m_menu;
   //QScrollArea* m_scrollArea;
   QImageWidget* m_body;
   std::array< GDetuneSynth*, 8 > m_synths;
   std::vector< float > m_inputBuffer;
   //std::vector< float > m_mixerBuffer;
   de::audio::Mixer m_mixer;
};
