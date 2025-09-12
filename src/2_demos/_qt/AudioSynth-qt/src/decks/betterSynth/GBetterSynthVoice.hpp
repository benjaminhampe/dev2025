#pragma once
#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>

#include <QFont5x8.hpp>
#include "GDeck.hpp"
#include "GGroupV.hpp"
#include "GMultiOsc.hpp"
#include "GLFO.hpp"
#include "GADSR.hpp"
#include "GVolume.hpp"
#include "GLevelMeter.hpp"
#include "BetterSynth.hpp"

// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GBetterSynthVoice : public QWidget
// ============================================================================
{
   Q_OBJECT
public:
   explicit GBetterSynthVoice( QWidget* parent, int voiceIndex );
   ~GBetterSynthVoice() override;
   de::audio::BetterSynthVoice* getVoice() const;
   void setVoice( de::audio::BetterSynthVoice* voice );
signals:
public slots:
   void setTitleImageText( QString title ) { m_menu->setTitleImageText( title ); }
   void setBackgroundColor( QColor bgColor ) { m_menu->setBackgroundColor( bgColor ); }
   void setMinified(bool minified) { m_menu->setMinified( minified ); }
   void setBypassed(bool bypassed) { m_menu->setBypassed( bypassed ); }
   void setExtraMore(bool visible) { m_menu->setExtraMore( visible ); }
private:
   DE_CREATE_LOGGER("GBetterSynthVoice")
   de::audio::BetterSynthVoice* m_voice;
   GDeck* m_menu;
   QImageWidget* m_body;
   GMultiOsc* m_osc;
   GLFO* m_lfoPhase;
   GLFO* m_lfoPulseWidth;
   GLFO* m_lfoDetune;
   GADSR* m_adsr;
protected slots:
   void on_toggledBypass(bool checked)
   {
      if ( m_voice )
      {
         m_voice->setBypassed( checked );
      }
   }

   //connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_menuButton_more(bool)) );
   //connect( m_menu, SIGNAL(toggledMinimized(bool)), this, SLOT(on_menuButton_hideAll(bool)) );
   void on_oscTypeChanged(int value)
   {
      if ( !m_voice ) { DE_ERROR("No voice") return; }
      m_voice->setOscType( value );
   }
   void on_coarseTuneChanged(int value)
   {
      if ( !m_voice ) { DE_ERROR("No voice") return; }
      m_voice->setDetune( value );
   }
   void on_fineTuneChanged(int value)
   {
      if ( !m_voice ) { DE_ERROR("No voice") return; }
      m_voice->setFineDetune( value );
   }
   void on_phaseChanged(int value)
   {
      if ( !m_voice ) { DE_ERROR("No voice") return; }
      m_voice->setPhaseLfoType( value );
   }
   void on_volumeChanged(int value)
   {
      if ( !m_voice ) { DE_ERROR("No voice") return; }
      m_voice->setVolume( value );
   }
};
