#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>

//#include <de/midi/MidiFile.hpp>
#include <de/audio/dsp/IDspChainElement.hpp>
// Opens a Midifile and display partitur like, 1st milestone
// Play midi by creating times notes and send to a benni synth. 2nd milestone
// Do it real midi way, whatever that is, 3rd milestone.
// Current milestone: 0.1
// =======================================================================
struct GMidiRecorder : public QWidget
// =======================================================================
{
   Q_OBJECT
public:
   GMidiRecorder( QWidget* parent );
   ~GMidiRecorder() override;
signals:
public slots:
   void chooseMidiFile();
   void openMidiFile( QString uri );
   void closeMidiFile();
   void setSynth( int i, de::audio::IDspChainElement* synth );

protected slots:

protected:
//   void timerEvent( QTimerEvent* event) override;
   void paintEvent( QPaintEvent* event ) override;
   //void resizeEvent( QResizeEvent* event ) override;
//   void keyPressEvent( QKeyEvent* event ) override;
//   void keyReleaseEvent( QKeyEvent* event ) override;
//   void mousePressEvent( QMouseEvent* event ) override;
//   void mouseReleaseEvent( QMouseEvent* event ) override;
//   void mouseMoveEvent( QMouseEvent* event ) override;
//   void wheelEvent( QWheelEvent* event ) override;
private:
   DE_CREATE_LOGGER("GMidiRecorder")
   int m_timerId;
   std::array< de::audio::IDspChainElement*, 8 > m_synths;
};
