#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QThread>

#if 0
// ============================================================================
class FooterButton : public QWidget
// ============================================================================
{
    Q_OBJECT
    QRect m_rcShow; // Enable button
    QRect m_rcText; // Name button -> toggles
    QRect m_rcView; // overview scrollbar
    QPixmap m_pixmap; // overview acts like a scrollbar
    QString m_text;
    //bool isOpen = false;

    int m_userData = -1; // trackIndex or clipIndex when pressing on rcName
    int m_scrollWidth;  // Scrollbar
    int m_scrollTotal;  // Scrollbar
    int m_scrollPos;    // Scrollbar

    int m_baseRadius = 8;
    int m_basePadding = 8;
    int m_baseButtonHeight = 48;

    int m_radius;
    int m_padding;
    int m_buttonHeight;

    QColor m_windowColor;
    QColor m_panelColor;
    QColor m_textColor;
    QColor m_activeColor;

    QPixmap m_pixShow;
    QPixmap m_pixHide;

public:
    FooterButton(QWidget* parent = 0);
    ~FooterButton() override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void applySkin();
    void updateLayout();

    void setScrollbarData(  QPixmap pixOverview,
                            int scrollWidth,
                            int scrollTotal,
                            int scrollPos);
signals:
public slots:
protected slots:
    void on_btnShow( bool checked );
    void on_btnName( bool checked );

protected:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    // void focusInEvent(QFocusEvent* event) override;
    // void focusOutEvent(QFocusEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    // int width() const
    // {
    //     return btnShow->width() + 1 + btnName->width() + pix.width(); }
    // }
/*
    void move(const int x, const int y)
    {
        int w = 0;
        int h = 0;

        btnShow->move(x+w,y);
        rcBtn = QRect(x+w,y,btnShow->width(),btnShow->height());
        w += btnShow->width() + 1;
        h = std::max(h,btnShow->height());

        btnName->move(x+w,y);
        rcName = QRect(x+w,y,btnName->width(),btnName->height());
        w += btnName->width();
        h = std::max(h,btnName->height());

        rcPixmap = QRect(x+w,y,pix.width(),pix.height());
        w += pix.width();
        h = std::max(h,pix.height());

        rcAll = QRect(x,y,w,h);
    }

    void draw(QPainter& dc)
    {
        if (pix.width() > 0 && pix.height() > 0)
        {
            dc.drawPixmap(rcPixmap.x(),rcPixmap.y(),pix);
        }
    }
*/
};

#endif
