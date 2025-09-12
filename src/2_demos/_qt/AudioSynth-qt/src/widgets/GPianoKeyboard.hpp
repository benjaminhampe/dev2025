#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <de/audio/dsp/IDspChainElement.hpp>

// =======================================================================
struct GPianoKeyboard : public QWidget
// =======================================================================
{
   Q_OBJECT
   DE_CREATE_LOGGER("GPianoKeyboard")
   struct Key
   {
      bool pressed;
      int midiNote;
      int oktave;
      int semitone;
      Key()
         : pressed( false ), midiNote( 0 ), oktave( 0 ), semitone( 0 ) {}
      Key( int iNote, int iOktave, int iSemi )
         : pressed( false ), midiNote( iNote ), oktave( iOktave ), semitone( iSemi )    {}
   };

   bool m_wantImageUpdate = true;

   QImage m_image;

   int m_timerId;
   int m_keyStart;

   double m_timeLastNote;
   double m_timeNote;

   std::vector< Key > m_keys;
   std::array< Key*, 10 > m_touched;

   std::array< QImage, 12 > m_images;

   std::array< de::audio::IDspChainElement*, 8 > m_synths;
public:
   GPianoKeyboard( QWidget* parent );
   ~GPianoKeyboard() override {}
   int32_t findKey( int midiNote ) const;
   void setKeyRange( int midiNoteStart, int midiNoteCount );

signals:
public slots:
   void setSynth( int i, de::audio::IDspChainElement* synth )
   {
      if ( i < 0 || i >= m_synths.size() ) return;
      m_synths[ i ] = synth;
   }

protected:
   void updateImage();
   void timerEvent( QTimerEvent* event) override;
   void paintEvent( QPaintEvent* event ) override;
   void resizeEvent( QResizeEvent* event ) override;
   void keyPressEvent( QKeyEvent* event ) override;
   void keyReleaseEvent( QKeyEvent* event ) override;
   void mousePressEvent( QMouseEvent* event ) override;
   void mouseReleaseEvent( QMouseEvent* event ) override;
   void mouseMoveEvent( QMouseEvent* event ) override;
   void wheelEvent( QWheelEvent* event ) override;
};
