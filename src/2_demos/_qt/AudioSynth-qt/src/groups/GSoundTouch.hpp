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
#include <QPushButton>

#include "GSliderV.hpp"
#include "GGroupV.hpp"

#include <de/audio/dsp/DspSoundTouch.h>

// ============================================================================
class GSoundTouch : public QWidget
// ============================================================================
{
   Q_OBJECT
   DE_CREATE_LOGGER("GSoundTouch")
   de::audio::SoundTouch m_soundTouch;
   GGroupV* m_menu;
   QImageWidget* m_body;
   //GSliderV* m_rate;
   GSliderV* m_tempo;
   GSliderV* m_pitch;
   GSliderV* m_aafilter;
   GSliderV* m_sequenceMs;
   GSliderV* m_seekWinMs;
   GSliderV* m_overlapMs;
   GSliderV* m_volume;

public:
   GSoundTouch( QWidget* parent = 0 );
   ~GSoundTouch();

   de::audio::SoundTouch* dsp()
   {
      return &m_soundTouch;
   }

signals:
public slots:
   void setMinified( bool visible )
   {
      m_menu->setMinified( visible );
   }
   void setBypassed( bool bypassed )
   {
      m_menu->setBypassed( bypassed );
      dsp()->setBypassed( bypassed );
   }
private slots:
   void on_menuButton_bypass(bool);
   void on_menuButton_more(bool);
   void on_menuButton_hideAll(bool);
   //void on_rateSlider( int value );
   void on_tempoSlider( int value );
   void on_pitchSlider( int value );
   void on_aalenSlider( int value );
   void on_sequenceSlider( int value );
   void on_seekwinSlider( int value );
   void on_overlapSlider( int value );
   void on_volumeSlider( int value );
};
