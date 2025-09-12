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
#include <fftw3.h>
#include <de_kissfft.hpp>

// ============================================================================
class GSpektrometer : public QWidget, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
   DE_CREATE_LOGGER("GSpektrometer")
public:
   GSpektrometer( QWidget* parent = 0 );
   ~GSpektrometer();

   void setFFTSize( uint32_t bufferSize );

   uint64_t
   readDspSamples( double pts, float* dst, uint32_t dstFrames,
               uint32_t dstChannels, double dstRate ) override;
signals:
//   void newSamples( float* samples, uint32_t frameCount, uint32_t channels );

public slots:
//   void stopUpdateTimer();
//   void startUpdateTimer();

   void clearInputSignals()
   {
      m_inputSignal = nullptr;
   }

   void setInputSignal( de::audio::IDspChainElement* input )
   {
      m_inputSignal = input;
   }

protected:
protected:
   void startUpdateTimer();
   void stopUpdateTimer();
   void timerEvent( QTimerEvent* event) override;

   void showEvent( QShowEvent* event ) override;
   void hideEvent( QHideEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;
   void updateImage( int w, int h );
private:
   int m_updateTimerId;
   de::audio::IDspChainElement* m_inputSignal;
   uint32_t m_channelCount;
   uint32_t m_sampleRate;

   bool m_isPlaying;

   float L_min;
   float L_max;
   float R_min;
   float R_max;

   QImage m_img;
   de::audio::ShiftBuffer m_shiftBuffer;
   std::vector< float > m_inputBuffer;
   std::vector< float > m_outputBuffer; // in deciBel dB

#if 1
   de::KissFFT m_fft;
#else
   uint32_t m_fftSize;
   std::string m_fftWisdomFile;
   double* m_fftInput;
   //std::vector< double > m_fftOutput; // complex output-vector
   fftw_complex* m_fftOutput; // complex output ( needs malloc() )
   fftw_plan m_fftPlan;
#endif

   QFont5x8 m_font5x8;
};
