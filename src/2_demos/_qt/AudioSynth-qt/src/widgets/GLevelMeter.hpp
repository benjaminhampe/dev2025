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
#include <DarkImage.h>
#include <de/audio/dsp/DspLevelMeter.h>

// ============================================================================
class GLevelMeter : public QWidget
// ============================================================================
{
   Q_OBJECT
   DE_CREATE_LOGGER("GLevelMeter")
public:
   GLevelMeter( QWidget* parent = 0 )
      : QWidget( parent )
      , m_updateTimerId( 0 )
      {}
   ~GLevelMeter() override {}

   de::audio::LevelMeter* dsp()
   {
      return &m_levelMeter;
   }

signals:
public slots:
//   void stopUpdateTimer();
//   void startUpdateTimer( int ms = 37 );
/*
   void
   clearInputSignals() override
   {
      m_levelMeter.clearInputSignals();
   }

   void
   setInputSignal( int, de::audio::IDspChainElement* dsp ) override
   {
      m_levelMeter.setInputSignal( dsp );
   }
*/
protected:
//   void timerEvent( QTimerEvent* event) override;
   void paintEvent( QPaintEvent* event ) override;

private:
   int m_updateTimerId;
   de::audio::LevelMeter m_levelMeter;
   QFont5x8 m_font5x8;
   de::LinearColorGradient m_ColorGradient;
};
