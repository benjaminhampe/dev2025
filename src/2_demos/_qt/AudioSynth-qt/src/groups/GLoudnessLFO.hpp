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
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include <de/audio/dsp/MultiOSC.hpp> // LFO


namespace de {
namespace audio {

// A paramter can oscillate aswell, from min to max value
template < typename T >
// =========================================================
struct LoudnessLFO
// =========================================================
{
   DE_CREATE_LOGGER("de.audio.LoudnessLFO")
   bool m_isBypassed;
   T m_freq;
   T m_period;
   T m_range;
   T m_phase;
   T m_minimum;
   T m_maximum;

   LoudnessLFO()
      : m_isBypassed( true )
      , m_freq( T(0.44) ) // 0.44 schwingungen pro ms = 440Hz
      , m_period( T(1) / m_freq )
      , m_range( T(1) )
      , m_phase( T(0) )
      , m_minimum( T(0) )
      , m_maximum( T(1) )
   {}

   bool isBypassed() const { return m_isBypassed; }
   void setBypassed( bool bypass ) { m_isBypassed = bypass; }

   void setVolumeRange( int min, int max ) // volume in [%] 0...100
   {
      if ( min > max )
      {
         std::swap( min, max );
      }
      m_minimum = min;
      m_maximum = max;
      m_range = m_maximum - m_minimum;
   }

   void setFrequency( T freq )
   {
      m_freq = freq / T(1000); // in [mHz]
      m_period = T(1) / m_freq; // in [ms]
   }

   void setPhase( T phase )
   {
      m_phase = phase;
   }

   float
   computeValue( T t ) const
   {
      if ( m_isBypassed ) return 1.0f; // Bypass
      t = fmod( 1000.0 * t, m_period );
      float sample = dbSin( m_freq * T(M_PI) * t ); //  * (t + m_phase * m_period)
      sample = m_range * sample + m_minimum;
      return sample;
   }
};

typedef LoudnessLFO< float > LoudnessLFOf;
typedef LoudnessLFO< double > LoudnessLFOd;

} // end namespace audio
} // end namespace de

// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GLoudnessLFO : public QWidget
// ============================================================================
{
   Q_OBJECT
public:
   GLoudnessLFO( QString title, QWidget* parent = 0 );
   ~GLoudnessLFO();

signals:
//   void newSamples( float* samples, uint32_t frameCount, uint32_t channels );

public slots:
//   void clearInputSignals();
//   void setInputSignal( de::audio::IDspChainElement* input );
//   void setBypassed( bool enabled );
   void setLFO( de::audio::LoudnessLFOf* lfo ) { m_LFO = lfo; }
//   void setVolume( int volume_in_pc );
   //void setChannel( int channel = -1 );

private slots:
   void on_lfoTypeChanged( int v );
   void on_lfoRateChanged( double v );
   void on_lfoMinChanged( double v );
   void on_lfoMaxChanged( double v );

//   void stopUpdateTimer()
//   {
//      if ( m_updateTimerId )
//      {
//         killTimer( m_updateTimerId );
//         m_updateTimerId = 0;
//      }
//   }
//   void startUpdateTimer( int ms = 67 )
//   {
//      stopUpdateTimer();
//      if ( ms < 1 ) { ms = 1; }
//      m_updateTimerId = startTimer( ms, Qt::PreciseTimer );
//   }

private:
   DE_CREATE_LOGGER("GLoudnessLFO")
   de::audio::LoudnessLFOf* m_LFO;
   bool m_isBypassed;

   QLabel* m_typeLabel;
   QComboBox* m_typeEdit;
   QLabel* m_rateLabel;
   QDoubleSpinBox* m_rateEdit;
   QLabel* m_minLabel;
   QDoubleSpinBox* m_minEdit;
   QLabel* m_maxLabel;
   QDoubleSpinBox* m_maxEdit;

};
