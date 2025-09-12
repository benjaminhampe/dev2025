#pragma once
#include <QWidget>
//#include <QImage>
//#include <QTimer>
//#include <QPainter>
//#include <QPaintEvent>
//#include <QResizeEvent>
//#include <QMouseEvent>
//#include <QKeyEvent>
//#include <QDebug>
//#include <QThread>
//#include <QCheckBox>
//#include <QSlider>
//#include <QDial>
//#include <QLabel>
#include "GSliderV.hpp"
#include "GGroupV.hpp"
#include <de/audio/dsp/ADSR.hpp>

// Placed in the DSP chain.
// Controls envelope of incoming audio signal.
// ============================================================================
class GADSR : public QWidget //, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   GADSR( QWidget* parent = 0 );
   ~GADSR();
//   int A() const { return m_dialA->value(); }
//   int D() const { return m_dialD->value(); }
//   int R() const { return m_dialR->value(); }
//   int S() const { return m_dialS->value(); }
/*
   uint64_t readSamples( double pts, float* dst, uint32_t dstFrames,
               uint32_t dstChannels, uint32_t dstRate ) override;
*/
   bool isBypassed() const { return m_menu->isBypassed(); }
signals:
//   void newSamples( float* samples, uint32_t frameCount, uint32_t channels );

public slots:
   //void clearInputSignals();
   //void setInputSignal( de::audio::IDspChainElement* input );
   void setADSR( de::audio::ADSR* adsr );
   void setMinified(bool enabled) { m_menu->setMinified( enabled ); }
   void setBypassed(bool bypassed) { m_menu->setBypassed( bypassed ); }
private slots:
   void on_menuButton_bypass(bool checked);
   void on_menuButton_more(bool checked);
   void on_menuButton_hideAll(bool checked);

   void on_sliderAttackTime( int v );
   void on_sliderDecayTime( int v );
   void on_sliderSustainLevel( int v );
   void on_sliderReleaseTime( int v );

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

protected:
   //void timerEvent( QTimerEvent* event) override;
   //void paintEvent( QPaintEvent* event ) override;

private:
   DE_CREATE_LOGGER("GADSR")
   //bool m_isBypassed;
   GGroupV* m_menu;
   GSliderV* m_A;
   GSliderV* m_D;
   GSliderV* m_S;
   GSliderV* m_R;
//   QLabel* m_labelA;
//   QLabel* m_labelD;
//   QLabel* m_labelS;
//   QLabel* m_labelR;

//   QLabel* m_valueA;
//   QLabel* m_valueD;
//   QLabel* m_valueS;
//   QLabel* m_valueR;

//   QSlider* m_dialA;
//   QSlider* m_dialD;
//   QSlider* m_dialS;
//   QSlider* m_dialR;

   //de::audio::IDspChainElement* m_inputSignal;

   de::audio::ADSR* m_adsr;
};
