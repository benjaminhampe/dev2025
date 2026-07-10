#pragma once
#include "PianoRoll.h"
#include <QWidget>
#include <QPushButton>
#include <de/midi/IMidiMessageListener.h>
#include <de/audio/plugin/details/BasePluginUtils.h>

// ============================================================================
class ClipEditor : public QWidget
                 , public de::midi::IMidiMessageListener
// ============================================================================
{
    Q_OBJECT
public:
    explicit ClipEditor(QWidget* parent = 0);
    void applySkin();
    void updateLayout();

public slots:
    void setClip(de::session::Clip* clip );

protected:
    void resizeEvent( QResizeEvent* event ) override;
    void paintEvent( QPaintEvent* event ) override;
/*
    void hideEvent( QHideEvent* event ) override;
    void showEvent( QShowEvent* event ) override;
    void keyPressEvent( QKeyEvent* event ) override;
    void keyReleaseEvent( QKeyEvent* event ) override;
    void mousePressEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;
    void mouseMoveEvent( QMouseEvent* event ) override;
    void wheelEvent( QWheelEvent* event ) override;
*/

    void onMidiMessage(double pts, const de::midi::MidiMessage& msg) override;

    void onShortMidiMessage(double pts, const de::midi::ShortMidiMessage& msg) override;

protected:
    PianoRoll* m_pianoRoll{ nullptr };
    de::session::Clip* m_clip{ nullptr };

    int m_zoom = 100;
    int m_margin = 8;
    int m_btnSize;

    QColor m_windowColor;
    QColor m_panelColor;

    QPushButton* m_btnRecord; // MidiRecorder

    de::audio::PluginClock m_midiClock;
};
