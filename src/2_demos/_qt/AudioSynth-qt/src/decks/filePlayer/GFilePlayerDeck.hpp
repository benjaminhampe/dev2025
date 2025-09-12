#pragma once
#include "GFilePlayer.hpp"
#include <GDeck.hpp>
#include <de/audio/dsp/IDspChainElement.hpp>
#include <GCrossFader.hpp>

// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GFilePlayerDeck : public QImageWidget,
                        public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   GFilePlayerDeck( QWidget* parent = 0 );
   ~GFilePlayerDeck();

   uint64_t
   readDspSamples( double pts, float* dst, uint32_t dstFrames,
               uint32_t dstChannels, double dstRate ) override;

signals:
//   void newSamples( float* samples, uint32_t frameCount, uint32_t channels );

public slots:
   void setMinified(bool minified) { m_menu->setMinified( minified ); }
   void setBypassed(bool bypassed) { m_menu->setBypassed( bypassed ); }
   void setExtraMore(bool visible) { m_menu->setExtraMore( visible ); }
protected:
private:
   DE_CREATE_LOGGER("GFilePlayerDeck")

   GDeck* m_menu;
   QImageWidget* m_body;

   GFilePlayer* m_leftPlayer;

   //GOszilloskop* m_leftWaveform;
   //GSpektrometer* m_leftSpektrum;

   GFilePlayer* m_rightPlayer;
   //GLowPass3* m_rightFilter;
   //GSoundTouch* m_rightTouch;
   //GLevelMeter* m_rightLevel;

   //GOszilloskop* m_rightWaveform;
   //GSpektrometer* m_rightSpektrum;
   GCrossFader* m_crossFader;

};
