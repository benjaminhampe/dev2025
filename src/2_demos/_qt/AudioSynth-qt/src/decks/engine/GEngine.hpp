#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>

#include <QLabel>
#include <QPushButton>
#include <QDial>

#include <QFont5x8.hpp>
#include <de/audio/dsp/DspMixer.h>

/// AudioDSP - Benjamin Hampe (c) 2022 <benjaminhampe@gmx.de>
#include "AudioEngine.h"

//#include <QDebug>
//#include <QThread>

// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GEngine : public QWidget //, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   GEngine( QWidget* parent = 0 );
   ~GEngine();

   de::audio::AudioDevice* dsp() { return &m_stream0; }
/*
   uint64_t
   readDspSamples(
      double pts, float* dst,
      uint32_t frames, uint32_t channels, double rate ) override;

signals:
//   void newSamples( float* samples, uint32_t frameCount, uint32_t channels );

public slots:
   void clearInputSignals();
   void setInputSignal( int i, de::audio::IDspChainElement* input );
*/

private slots:
   void on_volumeSlider( int v );
   void on_btnPlay( bool );
   void on_btnStop( bool );
protected:
   //void timerEvent( QTimerEvent* event) override;
   //void paintEvent( QPaintEvent* event ) override;

private:

   de::audio::AudioDevice m_stream0;

   //de::audio::Mixer m_mixer;

   QLabel* m_lblSampleType;
   QLabel* m_lblSampleRate;
   QLabel* m_lblFrames;
   QLabel* m_lblChannels;
   QLabel* m_lblLatency;
   QLabel* m_lblTimePos;
   QLabel* m_lblTimeStart;

   QPushButton* m_btnPlay;
   QPushButton* m_btnStop;
};
