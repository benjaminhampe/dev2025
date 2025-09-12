#pragma once
#include <QWidget>
#include "GSliderV.hpp"
#include "GGroupV.hpp"
#include <de/audio/dsp/IDspChainElement.hpp>

// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GOverdrive : public QWidget,
                   public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   GOverdrive( QWidget* parent = 0 );
   ~GOverdrive();

   uint64_t
   readDspSamples( double pts, float* dst, uint32_t dstFrames,
                uint32_t dstChannels, double dstRate ) override;
signals:
   // void newSamples( float* samples, uint32_t frameCount, uint32_t channels );
public slots:
   void clearInputSignals() override;
   void setInputSignal( int, de::audio::IDspChainElement* input ) override;
   void setBypassed( bool bypassed ) { m_menu->setBypassed( bypassed ); }
   void setMinified( bool minified ) { m_menu->setMinified( minified ); }
private slots:
   void on_menuButton_bypass(bool);
   void on_menuButton_more(bool);
   void on_menuButton_hideAll(bool);
   //void on_volumeSlider( float v );
protected:
   //void timerEvent( QTimerEvent* event) override;
   //void paintEvent( QPaintEvent* event ) override;
private:
   DE_CREATE_LOGGER("GOverdrive")
   de::audio::IDspChainElement* m_inputSignal;
   std::vector< float > m_inputBuffer;
   bool m_isBypassed;
   GGroupV* m_menu;
   QImageWidget* m_body;
   GSliderV* m_gainKnob;
   GSliderV* m_volumeKnob;
   GSliderV* m_cleanKnob;
   //GSliderV* m_trebleKnob;
   //GSliderV* m_midKnob;
   //GSliderV* m_bassKnob;
   //GSliderV* m_reverbKnob;
};

