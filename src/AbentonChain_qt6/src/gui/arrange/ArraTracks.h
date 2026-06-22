#pragma once
#include "ArraTrack.h"
#include <QWidget>

// ============================================================================
class ArraTracks : public QWidget
// ============================================================================
{
    Q_OBJECT
public:
    explicit ArraTracks(QWidget* parent = 0);
    void applySkin();
    void updateLayout();
    void updateFromSession();

public slots:
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
protected:
    ArraTrack m_masterTrack;
    std::vector<ArraTrack> m_tracks;

    int m_zoom = 100;
    int m_margin = 8;

    QColor m_windowColor;
    QColor m_panelColor;
};
