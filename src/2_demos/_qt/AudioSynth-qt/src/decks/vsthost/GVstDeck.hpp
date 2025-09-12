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

#include "GVstPlugin.hpp"
#include <de/audio/dsp/DspMixer.h>

// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GVstDeck : public QImageWidget, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
   DE_CREATE_LOGGER("GVstDeck")
private:
   std::vector< de::audio::IDspChainElement* > m_synths;
   de::audio::Mixer m_mixer;
   GDeck* m_menu;
   QImageWidget* m_body;
   GVstPlugin* m_vst1;
   GVstPlugin* m_vst2;
   GVstPlugin* m_vst3;
   GVstPlugin* m_vst4;
   GVstPlugin* m_vst5;
   //QScrollArea* m_scrollArea;

public:
   GVstDeck( QWidget* parent = 0 );
   ~GVstDeck();

   uint64_t
   readDspSamples( double pts, float* dst,
         uint32_t frames, uint32_t channels, double rate ) override
   {
      return m_mixer.readDspSamples( pts, dst, frames, channels, rate );
   }

signals:
public slots:
   void sendNote( de::audio::Note const & note ) override
   {
      for ( auto & synth : m_synths )
      {
         //if ( synth )
         synth->sendNote( note );
         //if ( synth && !synth->isBypassed() ) synth->sendNote( note );
      }
   }

   void setMinified(bool minified) { m_menu->setMinified( minified ); }
   void setBypassed(bool bypassed) { m_menu->setBypassed( bypassed ); }
   void setExtraMore(bool visible) { m_menu->setExtraMore( visible ); }


};
