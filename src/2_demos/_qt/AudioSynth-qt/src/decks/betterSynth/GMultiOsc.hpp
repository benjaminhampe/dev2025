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

#include "GGroupV.hpp"
#include "GSliderV.hpp"

// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GMultiOsc : public QWidget
// ============================================================================
{
   Q_OBJECT
public:
   GMultiOsc( QWidget* parent = 0 );
   ~GMultiOsc();

signals:
   //void newSamples( float* samples, uint32_t frameCount, uint32_t channels );
   void oscTypeChanged(int);
   void coarseTuneChanged(int);
   void fineTuneChanged(int);
   void phaseChanged(int);
   void volumeChanged(int);
public slots:
   //void clearInputSignals();
   //void setInputSignal( de::audio::IDspChainElement* input );
   //void setBypassed( bool enabled );
   //void setVolume( int volume_in_pc );
   //void setChannel( int channel = -1 );
   //void setOSC( de::audio::MultiOSCf* osc );
   //void setValues( int type, double rate, double center, double range = 0.0 );
   void setMinified(bool minified) { m_menu->setMinified( minified ); }
   void setBypassed(bool bypassed) { m_menu->setBypassed( bypassed ); }
//private slots:
//   void on_menuButton_bypass(bool);
//   void on_menuButton_more(bool);
//   void on_menuButton_hideAll(bool);
//   void on_oscTypeChanged( int v );
//   void on_coarseTuneChanged( int v );
//   void on_fineTuneChanged( int v );
//   void on_phaseChanged( int v );
//   void on_volumeChanged( int v );
private:
   DE_CREATE_LOGGER("GMultiOsc")
public:
   //de::audio::MultiOSCf* m_osc;
   GGroupV* m_menu;
   QImageWidget* m_body;

   QLabel* m_oscTypeLabel;
   QLabel* m_coarseTuneLabel;
   QLabel* m_fineTuneLabel;
   QLabel* m_phaseLabel;

   QComboBox* m_oscType;
   QSpinBox* m_coarseTune;
   QSpinBox* m_fineTune;
   QSpinBox* m_phase;
   GSliderV* m_volume;

protected slots:
   void on_toggledHideAll( bool checked )
   {
      m_body->setVisible( !checked );
   }
   void on_toggledBypass( bool checked )
   {
      // if ( m_osc ) m_osc->setBypassed( checked );
   }
   void on_toggledMore( bool checked )
   {

   }
   void on_currentIndexChanged_oscType( int value )
   {
   //   if ( !m_osc ) return;
   //   m_osc->setFunction( v );
      emit oscTypeChanged( m_oscType->itemData(value,Qt::UserRole).toInt() );
   }
   void on_valueChanged_coarseTune( int value )
   {
      //   if ( !m_osc ) return;
      //   float note = m_coarseTune->value();
      //   float detune = 0.01f * m_fineTune->value();
      //   m_osc->setFrequency( getFrequencyFromMidiNote( note + detune ) );

      emit coarseTuneChanged(value);
   }
   void on_valueChanged_fineTune( int value )
   {
   //   if ( !m_osc ) return;
   //   float note = m_coarseTune->value();
   //   float detune = 0.01f * m_fineTune->value();
   //   m_osc->setFrequency( getFrequencyFromMidiNote( note + detune ) );
      emit fineTuneChanged(value);
   }
   void on_valueChanged_phase( int value )
   {
      //   if ( !m_osc ) return;
      //   float phase = 0.01f * m_phase->value();
      //   m_osc->setPhase( phase );
      emit phaseChanged(value);
   }
   void on_valueChanged_volume( int value )
   {
      emit volumeChanged(value);
   }
};
