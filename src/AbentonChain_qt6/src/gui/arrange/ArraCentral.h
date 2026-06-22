#pragma once
#include "ArraOverview.h"
#include "ArraTracks.h"
#include "ArraMixer.h"
#include <QWidget>
#include <de/midi/file/Parser.h>
#include <de/midi/file/ParserListener.h>

// ============================================================================
class ArraCentral : public QWidget
// ============================================================================
{
    Q_OBJECT
public:
    ArraOverview* m_overview{ nullptr };
    ArraTracks* m_tracks{ nullptr };
    ArraMixer* m_mixer{ nullptr };

    de::midi::file::MidiFile m_midiFile;

    int m_zoom = 100;
    int m_margin = 8;

    QColor m_windowColor;
    QColor m_panelColor;

public:
    explicit ArraCentral(QWidget* parent = 0);
    void applySkin();
    void updateLayout();

public slots:
    //void setClip( Clip* clip );

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

    // ------------------------------------------------------------
    // Drag&Drop
    // ------------------------------------------------------------
    void dragEnterEvent(QDragEnterEvent* e) override;
    void dragMoveEvent(QDragMoveEvent* e) override;
    void dragLeaveEvent(QDragLeaveEvent*) override;
    void dropEvent(QDropEvent* e) override;

protected:

};
