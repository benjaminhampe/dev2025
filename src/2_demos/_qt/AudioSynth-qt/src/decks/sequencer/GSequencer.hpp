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

#include <de/midi/MidiTools.hpp>
#include <de/audio/dsp/IDspChainElement.hpp>

// ============================================================================
class GSequencer : public QWidget
// ============================================================================
{
   Q_OBJECT
public:
   GSequencer( QWidget* parent = 0 );
   ~GSequencer();
   bool
   isMouseOverPianoBar() const;
   bool
   isMouseOverBeatGrid() const;
   int
   getKeyIndexFromMousePos() const;

   // The selected note
   struct SelNote
   {
      int32_t keyIndex = -1;
      int32_t noteIndex = -1;
      bool ok() const { return keyIndex > -1 && noteIndex > -1; }
      void reset() { keyIndex = -1; noteIndex = -1; }
   };

   SelNote
   computeHoveredNote() const;
//   int pixel2ticks( int px ) const;
//   int ticks2pixel( int ticks ) const;
//   float pixel2sec( float px ) const;
//   float sec2pixel( float ss ) const;
   // uses m_nanosPerPixel
   int64_t pixel2time( int64_t px ) const;
   // uses m_pixelPerNanos
   int64_t time2pixel( int64_t timeInNanoSec ) const;
//signals:
//   void noteOn( int channel, int midiNote, int velocity );
//   void noteOff( int channel, int midiNote );
public slots:
   void setSynth( int i, de::audio::IDspChainElement* synth );
   void sendNote( de::audio::Note const & note );
   void play( int mode = 0 );
   void stop();
protected slots:
   void stopPlayTimer();
   void startPlayTimer();
protected:
   void timerEvent( QTimerEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;
   void resizeEvent( QResizeEvent* event ) override;
   void hideEvent( QHideEvent* event ) override;
   void showEvent( QShowEvent* event ) override;
   void keyPressEvent( QKeyEvent* event ) override;
   void keyReleaseEvent( QKeyEvent* event ) override;
   void mousePressEvent( QMouseEvent* event ) override;
   void mouseReleaseEvent( QMouseEvent* event ) override;
   void mouseMoveEvent( QMouseEvent* event ) override;
   void wheelEvent( QWheelEvent* event ) override;
protected:
   DE_CREATE_LOGGER("GSequencer")
   int m_playTimerId;
   int m_drawTimerId;

   float m_bpm;
   bool m_isPlaying;
   int64_t m_timeStart;
   int64_t m_time;
   int64_t m_loopTime;
   int64_t m_loopTimeRange;
   int64_t m_loopTimeStart;
   int64_t m_loopTimeEnd;
   double m_nanosecondsPerPixel;
   double m_pixelsPerNanosecond;

   // uses
   int m_topHeight; // offset from top until start of PianoBar (keys)
   int m_keyStart;  // visible y-axis start
   int m_keyCount;  // visible y-axis count
   int m_keyWidth;
   int m_keyHeight;

   int m_beatStart;
   int m_beatCount;
   int m_beatIndex;  // play index in beats

   int m_midiTicksPerBeat;
//   float m_pixelPerSec;
//   float m_pixelPerBeat;

   // MouseInput
   int m_mouseX;
   int m_mouseY;
   int m_detectedKeyIndex; // vip
   SelNote m_hoveredNote;
   SelNote m_selectedNote;
   bool m_isOverPianoBar;  // semi vip
   bool m_isOverBeatGrid;  // semi vip
   int m_dragMode;   // 0 = none, 1 = drawing, 2 = stretch keyHeight
   int m_dragStartX;       // common pos used by all click events
   int m_dragStartY;       // common pos used by all click events

   // Font5x8 for drawing
   QFont5x8 m_font5x8;

   // Where the sequencer sends notes to.
   std::array< de::audio::IDspChainElement*, 8 > m_synths;

   // What the sequencer stores, draws and sends to synths.
   struct Note
   {
      int64_t timeBeg = 0; // in nanoseconds
      int64_t timeEnd = 0; // in nanoseconds
      uint32_t color = 0xFF0000FF;
   };

   // We store 128 keys ( all musical midi semitones )
   // Each key[0] to key[127] can have arbitrary number of notes.
   // Notes should be auto-sorted and removed to reduce overlaps.
   struct Key
   {
      int channel = 0;
      int midiNote = 0;

      // drawing
      int oktave = 0; // Precomputed for easier drawing.
      int semi = 0;
      float freq = 0.0f;
      bool isBlack = false; // black or white key. Precomputed for easier drawing.
      // uint32_t color = 0xFFFFFFFF;
      std::vector< Note > m_notes;

      void reset( int note )
      {
         midiNote = note;
         oktave = midiNote / 12;
         semi = midiNote - 12 * oktave;
         isBlack = de::midi::MidiTools::isBlackPianoKey( semi );
         freq = 440.0f * powf( 2.0f, (midiNote - 69.0f) * (1.0f/12.0f) );
      }


   };

   // Array is build up from highest note to lowest, because we draw them that way.
   std::array< Key, 128 > m_keys;
};
