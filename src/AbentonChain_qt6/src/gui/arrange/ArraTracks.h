#pragma once
#include <QWidget>

namespace de {
namespace session {
    class Track;
} // end namespace session.
} // end namespace de.

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
    void drawTrack(QPainter & dc) const;
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
    void drawTrack(QPainter & dc,
                de::session::Track* track,
                QColor fillColor) const;
protected:

    int m_zoom = 100;
    int m_margin = 8;
    int m_headerHeight = 48;
    bool m_bVertical = false;

    QColor m_windowColor;
    QColor m_panelColor;
    QColor m_alternatingPanelColor;
};
