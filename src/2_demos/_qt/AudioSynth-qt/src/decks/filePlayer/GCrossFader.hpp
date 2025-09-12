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
#include "GSliderV.hpp"
#include <de/audio/dsp/IDspChainElement.hpp>


// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GCrossFader : public QWidget,
                     public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   GCrossFader( QWidget* parent = 0 );
   ~GCrossFader();

   static float
   almostEqual( float a, float b, float eps = std::numeric_limits< float >::epsilon() )
   {
      return std::abs( a-b ) <= eps;
   }

   uint64_t
   readDspSamples( double pts, float* dst,
      uint32_t frames, uint32_t channels, double rate ) override;

signals:
//   void newSamples( float* samples, uint32_t frameCount, uint32_t channels );

public slots:
   void clearInputSignals() override;
   void setInputSignal( int i, de::audio::IDspChainElement* pSignal ) override;
   void setInputVolume( int i, int volume );
   void setFader( float fader );

private slots:
   void on_leftSlider( int v );
   void on_rightSlider( int v );
   void on_faderSlider( int v );

private:
   DE_CREATE_LOGGER("GCrossFader")
   de::audio::IDspChainElement* m_leftSignal;
   de::audio::IDspChainElement* m_rightSignal;
   int m_volumeL;
   int m_volumeR;
   float m_fader;

   QImageWidget* m_centerWidget;
   GSliderV* m_leftSlider;
   GSliderV* m_rightSlider;
   QSlider* m_faderSlider;

   std::vector< float > m_leftBuffer;
   std::vector< float > m_rightBuffer;


};
