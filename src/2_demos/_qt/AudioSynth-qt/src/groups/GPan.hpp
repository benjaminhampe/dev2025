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
#include "QImageWidget.hpp"
#include "GGroupV.hpp"
#include "GSliderV.hpp"
#include <de/audio/dsp/IDspChainElement.hpp>

namespace de {
namespace audio {

// ============================================================================
struct Pan : public IDspChainElement
// ============================================================================
{
   DE_CREATE_LOGGER("de.audio.Pan")
public:
   IDspChainElement* m_inputSignal;
   bool m_isBypassed;
   //int m_volumeL;
   //int m_volumeR;
   float m_fader;

   Pan()
      : m_inputSignal( nullptr )
      , m_isBypassed( false )
      //, m_volumeL( 100 )
      //, m_volumeR( 100 )
      , m_fader( 0.0f )
   {}

   //~Pan() {}

   void
   clearInputSignals() { m_inputSignal = nullptr; }
   void
   setInputSignal( int i, IDspChainElement* input ) { m_inputSignal = input; }

   bool isBypassed() const { return m_isBypassed; }
   void setBypassed( bool enabled ) { m_isBypassed = enabled; }

   // int
   // getVolume() const { return m_volume; }
   // void
   // setVolume( int vol ) { m_volume = std::clamp( vol, 0, 100 ); }

   void setFader( float fader )
   {
      m_fader = fabs( std::clamp( fader, -1.f, 1.f ) );
   }

   static float
   almostEqual( float a, float b, float eps = std::numeric_limits< float >::epsilon() )
   {
      return std::abs( a-b ) <= eps;
   }

   uint64_t
   readDspSamples( double pts,
                float* dst,
                uint32_t dstFrames,
                uint32_t dstChannels,
                double dstRate ) override
   {
      uint64_t dstSamples = dstFrames * dstChannels;

      if ( m_inputSignal )
      {
         m_inputSignal->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
      }
      else
      {
         DSP_FILLZERO( dst, dstSamples );
      }

      if ( m_isBypassed || dstChannels != 2 )
      {
         //DE_ERROR("Unsupported")
      }
      else
      {
         auto pSrc = dst;
         auto pDst = dst;
         for ( uint64_t i = 0; i < dstFrames; ++i )
         {
            float oldL = *pSrc++;
            float oldR = *pSrc++;
            float newL = (1.0f - m_fader) * oldL + m_fader * oldR; // Stereo pan
            float newR = (1.0f - m_fader) * oldR + m_fader * oldL; // makes no sense for cc != 2
            *pDst++ = newL;
            *pDst++ = newR;
         }

      }

      return dstSamples;
   }

};

} // end namespace audio
} // end namespace de


// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GPan : public QWidget
// ============================================================================
{
   Q_OBJECT
public:
   GPan( QWidget* parent = 0 );
   ~GPan();

   de::audio::Pan* dsp()
   {
      return &m_pan;
   }

signals:
public slots:
   void setFader( float fader );
   void setBypassed( bool bypassed ) { m_menu->setBypassed( bypassed ); }
   void setMinified( bool minified ) { m_menu->setMinified( minified ); }
private slots:
   void on_menuButton_bypass(bool checked);
   void on_menuButton_more(bool checked);
   void on_menuButton_hideAll(bool checked);
   void on_panSlider( int v );

private:
   DE_CREATE_LOGGER("GPan")
   de::audio::Pan m_pan;
   GGroupV* m_menu;
   QImageWidget* m_body;
   GSliderV* m_panSlider;
};

