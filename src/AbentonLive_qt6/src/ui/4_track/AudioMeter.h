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

#include "LiveSkin.h"
#include <de/audio/dsp/IDspChainElement.h>

//#include <BottomLeftPanel.hpp>
//#include <MidiMeter.hpp>
//#include <ClipEditor.hpp>

struct App;

// ============================================================================
class AudioMeter2 : public QWidget
// ============================================================================
{
   Q_OBJECT
public:
   AudioMeter2( QWidget* parent = 0 );
   ~AudioMeter2() {}
signals:
public slots:
   void setData( float l_low, float l_high, float r_low, float r_high );

protected:
   void paintEvent( QPaintEvent* event ) override;
private:
   float m_Lmin;
   float m_Lmax;
   float m_Rmin;
   float m_Rmax;

   QFont5x8 m_font5x8;
   de::LinearColorGradient m_ColorGradient;
};

// ============================================================================
class AudioMeter : public QWidget, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   AudioMeter( LiveSkin& skin, QWidget* parent = 0 );
   ~AudioMeter();
   bool hasFocus() const { return m_hasFocus; }
   uint64_t readSamples( double pts, float* dst, uint32_t dstFrames,
               uint32_t dstChannels, uint32_t dstRate ) override;
signals:
public slots:
   void setInputSignal( int i, de::audio::IDspChainElement* input ) override;
   void clearInputSignals() override;

   void stopUpdateTimer();
   void startUpdateTimer( int ms = 37 );

protected:
   void paintEvent( QPaintEvent* event ) override;
   void resizeEvent( QResizeEvent* event ) override;
   void timerEvent( QTimerEvent* event) override;
   void enterEvent( QEnterEvent* event ) override;
   void leaveEvent( QEvent* event ) override;
   void focusInEvent( QFocusEvent* event ) override;
   void focusOutEvent( QFocusEvent* event ) override;
private:
   DE_CREATE_LOGGER("AudioMeter")
   LiveSkin & m_skin;
   bool m_isHovered;
   bool m_hasFocus;
   int m_updateTimerId;

   de::audio::IDspChainElement* m_inputSignal;
   //std::vector< float > m_inputBuffer;
   float m_Lmin;
   float m_Lmax;
   float m_Rmin;
   float m_Rmax;

//   std::vector< float > m_accumBuffer;
   QFont5x8 m_font5x8;
   de::LinearColorGradient m_ColorGradient;
};
