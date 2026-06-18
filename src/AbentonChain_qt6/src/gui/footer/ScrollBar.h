#pragma once
#include <QWidget>

// ============================================
class ScrollBar : public QWidget
// ============================================
{
    Q_OBJECT
public:
    ScrollBar(QWidget* parent = nullptr);
    //QSize sizeHint() const override;
    //QSize minimumSizeHint() const override;
    void applySkin();
    void updateLayout();
    void setPixmap(QPixmap pixmap);
    void setScrollbar(int scrollWidth, int scrollTotal, int scrollPos);
    // Toggles background color for different mode
    void setKeyAssign( bool enabled );
    void setMidiAssign( bool enabled );

protected:
    void resizeEvent(QResizeEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

protected:
    int m_zoom = 100;
    int m_scrollWidth = 100; // Scrollbar
    int m_scrollTotal = 100; // Scrollbar
    int m_scrollPos = 0; // Scrollbar
    int m_borderRadius = 8;
    int m_borderWidth = 4;
    QColor m_borderColor = QColor(32,32,32);

    QPixmap m_pixmap;
};
