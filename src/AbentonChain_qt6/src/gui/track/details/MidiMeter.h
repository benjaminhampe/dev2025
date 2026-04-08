#pragma once
#include <QWidget>

// ============================================================================
class MidiMeter : public QWidget
// ============================================================================
{
    Q_OBJECT
public:
    MidiMeter( QWidget* parent = 0 );
    ~MidiMeter() override {}
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void applySkin();

protected:
    void paintEvent( QPaintEvent* event ) override;

private:
    const int m_numCircles = 21;

    int m_baseWidth = 18;
    int m_baseCircleSpacing = 4;
    int m_baseCircleRadius = 6;
    int m_baseLeft = 6;
    int m_baseRight = 2;

    int m_width;
    int m_height;
    int m_circleSpacing;
    int m_circleRadius;
    int m_left;
    int m_right;

    QColor m_windowColor;
    QColor m_offColor = QColor(36,36,36);
    QColor m_onColor = QColor(255,155,25);
};
