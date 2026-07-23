#pragma once
#include <de/session/Clip.h>
#include <de/audio/dsp/IDspChainElement.h>
#include <QWidget>
#include "QFont5x8.h"

// ============================================================================
class PianoRoll : public QWidget
// ============================================================================
{
    Q_OBJECT
public:
    explicit PianoRoll(QWidget* parent = 0);
    ~PianoRoll() override;

    void applySkin();
    void updateLayout();

    bool isMouseOverPianoBar() const;
    bool isMouseOverBeatGrid() const;
    int getKeyIndexFromMousePos() const;

    static bool inRange(const int a, const int min, const int max)
    {
        if (a < min) return false;
        if (a > max) return false;
        return true;
    }

    // The selected note
    struct SelNote
    {
        int32_t keyIndex = -1;
        int32_t noteIndex = -1;
        bool ok() const { return keyIndex > -1 && noteIndex > -1; }
        void reset() { keyIndex = -1; noteIndex = -1; }
    };

    SelNote computeHoveredNote() const;
    //   int pixel2ticks( int px ) const;
    //   int ticks2pixel( int ticks ) const;
    //   float pixel2sec( float px ) const;
    //   float sec2pixel( float ss ) const;

    // uses m_nanosPerPixel
    double pix2sec( double px ) const
    {
       return m_secondsPerPixel * (px - m_skin.keyW);
    }

    // uses m_pixelPerNanos
    double sec2pix( double seconds ) const
    {
       return (seconds * m_pixelsPerSecond) + m_skin.keyW;
    }

    //signals:
    //   void noteOn( int channel, int midiNote, int velocity );
    //   void noteOff( int channel, int midiNote );
public slots:
    void reset();
    void setClip(de::session::Clip* clip );
    // void setSynth( int i, de::audio::IDspChainElement* synth );
    // void sendNote( de::audio::Note const & note );
    //void play( int mode = 0 );
    //void stop();
// protected slots:
//     void stopPlayTimer();
//     void startPlayTimer();
protected:
//  void timerEvent( QTimerEvent* event ) override;
    void resizeEvent( QResizeEvent* event ) override;
    void paintEvent( QPaintEvent* event ) override;

    void drawPianoBar(QPainter & dc, QRect pos);
    void drawTimeline(QPainter & dc, QRect pos);
    void drawNotes(QPainter & dc, QRect pos);

    void hideEvent( QHideEvent* event ) override;
    void showEvent( QShowEvent* event ) override;
    void keyPressEvent( QKeyEvent* event ) override;
    void keyReleaseEvent( QKeyEvent* event ) override;
    void mousePressEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;
    void mouseMoveEvent( QMouseEvent* event ) override;
    void wheelEvent( QWheelEvent* event ) override;
protected:
    int m_playTimerId;
    int m_drawTimerId;
    de::session::Clip* m_clip;

struct MySkin
{
    QColor panelColor;
    QColor pianobarColor;
    QColor timelineColor;
    QColor bodyColor;
    QColor gridColorX;
    QColor gridColorY;
    QColor yWhite;
    QColor yBlack;

    // Font5x8 for drawing
    de::Font5x8 fontKey;

    // uses
    int zoom;
    float zoomX;
    float zoomY;

    int pianobarW;
    int timelineH;
    int keyW;
    int keyH;

    QRect r_corner;
    QRect r_pianobar;
    QRect r_timeline;
    QRect r_body;
};

    MySkin m_skin;

    // View axis X = Time
    int m_scrollX;
    //int64_t m_xEnd;
    //int64_t m_xNow;

    // View axis Y = MidiNote/Key
    int m_scrollY;
    //int64_t m_yEnd;
    //int64_t m_yNow;  // mouse over

    double m_secondsPerPixel;
    double m_pixelsPerSecond;

    bool m_isPlaying;
    bool m_isBeatSync;
    float m_bpm;
    int m_ppq;

    int64_t m_beatBeg;
    int64_t m_beatEnd;
    int64_t m_beatNow;  // play index in beats

    int64_t m_loopNow;
    int64_t m_loopCount;
    int64_t m_loopTimeBeg;
    int64_t m_loopTimeEnd;

    // MouseInput
    // bool m_isOverPianoBar;  // semi vip
    // bool m_isOverBeatGrid;  // semi vip
    int m_mx;
    int m_my;
    int m_detectedMidiNote; // vip
    SelNote m_hoveredNote;
    SelNote m_selectedNote;
    int m_dragMode;   // 0 = none, 1 = drawing, 2 = stretch keyHeight
    int m_dragStartX;       // common pos used by all click events
    int m_dragStartY;       // common pos used by all click events

};
