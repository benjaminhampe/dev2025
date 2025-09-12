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
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFont5x8.hpp>

#include "GSequencer.hpp"

#include <QScrollBar>
// ============================================================================
class GSequencerCtl : public QWidget
// ============================================================================
{
   Q_OBJECT
public:
   GSequencerCtl( QWidget* parent = 0 );
   ~GSequencerCtl();

signals:
public slots:
protected slots:
   void on_playButton( bool checked );
   void on_stopButton( bool checked );
protected:

   DE_CREATE_LOGGER("GSequencerCtl")
   QPushButton* m_playButton;
   QPushButton* m_stopButton;
   GSequencer* m_sequencer;
   QScrollBar* m_scrollX;
   QScrollBar* m_scrollY;
   // QSpinBox* m_bpmSlider;
};
