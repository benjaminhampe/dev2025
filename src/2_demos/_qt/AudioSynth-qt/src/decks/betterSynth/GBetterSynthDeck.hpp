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
// Benni
#include <QFont5x8.hpp>
#include <QImageWidget.hpp>
#include <GDeck.hpp>
#include "GBetterSynthVoice.hpp"
#include <GVolume.hpp>
#include <GLevelMeter.hpp>
#include <GLowPass3.hpp>
#include <GSoundTouch.hpp>
#include <GOszilloskop.hpp>
#include <GSpektrometer.hpp>

// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GBetterSynthDeck : public QImageWidget, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   GBetterSynthDeck( QWidget* parent = 0 );
   ~GBetterSynthDeck();

   uint64_t
   readDspSamples(
      double pts,
      float* dst,
      uint32_t dstFrames,
      uint32_t dstChannels,
      double dstRate ) override;

   bool isSynth() const override { return true; }
signals:
   //void newSamples( float* samples, uint32_t frameCount, uint32_t channels );
public slots:
   void sendNote( de::audio::Note const & note ) override;
   void setMinified(bool minified) { m_menu->setMinified( minified ); }
   void setBypassed(bool bypassed) { m_menu->setBypassed( bypassed ); }
   void setExtraMore(bool visible) { m_menu->setExtraMore( visible ); }
private:
   de::audio::BetterSynth m_synth;
   GDeck* m_menu;
   QImageWidget* m_body;
   GLowPass3* m_lowPass3;
   GLevelMeter* m_levelMeter;
   GVolume* m_volumeSlider;
   GOszilloskop* m_waveform;

   void on_toggledBypass(bool checked)
   {
      DE_DEBUG("bypassed = ", checked)
      m_synth.setBypassed( checked );
   }

   void on_toggledMinimized(bool checked)
   {
      DE_DEBUG("minimized = ", checked)
      //m_synth.setBypassed( checked );
   }
//   void on_oscTypeChanged(int value)
//   {
//      if ( !m_voice ) { DE_ERROR("No voice") return; }
//      m_voice->setOscType( value );
//   }
//   void on_coarseTuneChanged(int value)
//   {
//      if ( !m_voice ) { DE_ERROR("No voice") return; }
//      m_voice->setDetune( value );
//   }
//   void on_fineTuneChanged(int value)
//   {
//      if ( !m_voice ) { DE_ERROR("No voice") return; }
//      m_voice->setFineDetune( value );
//   }
//   void on_phaseChanged(int value)
//   {
//      if ( !m_voice ) { DE_ERROR("No voice") return; }
//      m_voice->setPhaseLfoType( value );
//   }
//   void on_volumeChanged(int value)
//   {
//      if ( !m_voice ) { DE_ERROR("No voice") return; }
//      m_voice->setVolume( value );
//   }
};
