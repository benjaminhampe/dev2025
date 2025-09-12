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

// ============================================================================
class GOszilloskop : public QWidget, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
   DE_CREATE_LOGGER("GOszilloskop")
public:
   GOszilloskop( QWidget* parent = 0 );
   ~GOszilloskop();

   void setBypassed( bool bypassed )
   {

   }

   uint64_t
   readDspSamples( double pts, float* dst, uint32_t dstFrames,
               uint32_t dstChannels, double dstRate ) override;
signals:
//   void newSamples( float* samples, uint32_t frameCount, uint32_t channels );
public slots:
   void clearInputSignals();
   void setInputSignal( int i, de::audio::IDspChainElement* input );
   void setShiftBufferSize( uint32_t dstSamples );
protected:
   void startUpdateTimer();
   void stopUpdateTimer();
   void timerEvent( QTimerEvent* event) override;
   void paintEvent( QPaintEvent* event ) override;
   void hideEvent( QHideEvent* event ) override;
   void showEvent( QShowEvent* event ) override;
private:
   de::audio::IDspChainElement* m_inputSignal;  // We are a relay/processor
   uint32_t m_channelCount;   // Just needed for drawing
   int m_updateTimerId;       // Calls render() not as much as AudioCallback would
   float m_minL;              // Level min L
   float m_maxL;              // Level max L
   float m_minR;              // Level min R
   float m_maxR;              // Level max R
//   uint32_t m_backgroundColor;
//   uint32_t m_leftChannelColor;
//   uint32_t m_rightChannelColor;
   QColor m_backgroundColor;
   QColor m_leftChannelColor;
   QColor m_rightChannelColor;

   QImage m_img;  // Expensive but a bit faster drawing ops

   de::audio::ShiftBuffer m_shiftBuffer; // Collects samples for one new row

//   std::vector< float > m_accumBuffer;
   QFont5x8 m_font5x8;
};
