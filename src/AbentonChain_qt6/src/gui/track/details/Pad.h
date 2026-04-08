#pragma once
#include <QWidget>

// ============================================
class Pad : public QWidget
// ============================================
{
    Q_OBJECT
public:
    Pad(QWidget* parent = nullptr);
    // QSize sizeHint() const override;
    // QSize minimumSizeHint() const override;

    void applySkin();

protected slots:
    void onToggled( bool checked );

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    // void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    int m_baseWidth = 280;
    int m_baseHeight = 250;
    int m_baseBorderRadius = 4;
    int m_baseCircleRadius = 13;
    int m_baseCircleBorder = 4;

    int m_width;
    int m_height;
    int m_borderRadius;
    int m_circleRadius;
    int m_circleBorder;

    float m_fx = 0.0f;
    float m_fy = 0.0f;

    bool m_bIsDragging = false;

    QRect m_rcBody;
};
