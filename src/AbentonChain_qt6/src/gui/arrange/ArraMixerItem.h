#pragma once
#include <QWidget>

// ============================================================================
class ArraMixerItem : public QWidget
// ============================================================================
{
    Q_OBJECT
public:
    explicit ArraMixerItem(QWidget* parent = 0);
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
protected:
    //PianoRoll* m_pianoRoll{ nullptr };
    //Clip* m_clip{ nullptr };
    
    int m_zoom = 100;
    int m_margin = 8;

    QColor m_windowColor;
    QColor m_panelColor;
};
