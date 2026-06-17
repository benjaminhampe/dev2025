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

#include "gui/footer/PixButton.h"

// ============================================================================
struct OverviewButton : public QWidget
// ============================================================================
{
    Q_OBJECT
    // QRect rcAll;
    // QRect rcBtn;
    // QRect rcName;
    // QRect rcPixmap; // overview

    PixButton* m_btnShow;
    PixButton* m_btnName;
    QPixmap m_pix; // overview acts like a scrollbar
    //QString text;
    //bool isOpen = false;

    int m_zoom = 100;
    int m_radius;
    int m_posScrollbar = 0;
    int m_userData = -1; // trackIndex or clipIndex when pressing on rcName
    int m_viewWidth; // Scrollbar
    int m_viewTotal; // Scrollbar
    int m_viewPos;   // Scrollbar

    QString m_nameText;
    QFont m_nameFont;

    QColor m_windowColor;
    QColor m_panelColor;
    QColor m_textColor;
    QColor m_activeColor;
public:
    OverviewButton(QWidget* parent = 0);
    void applySkin();
    void updateLayout();
    // void updateText();

    int computeBestWidth() const;

    void setNameText(QString name);
    void setOverviewPixmap(QPixmap pix, int visibleWidth, int totalWidth, int xPos);

signals:
    void sig_show(bool bChecked);
    void sig_combo(bool bChecked);

protected:
    void resizeEvent( QResizeEvent* event ) override;
    void paintEvent( QPaintEvent* event ) override;
    // bool event(QEvent* event) override;
    // void focusInEvent( QFocusEvent* event ) override;
    // void focusOutEvent( QFocusEvent* event ) override;

    static QPixmap
    createTextPixmap(
        int maxHeight,
        QString text,
        QColor textColor,
        QColor fillColor,
        QFont font);

    QPixmap createArrowRight(int w, int h, int r,
        QColor windowColor, QColor panelColor, QColor symbolColor);

    QPixmap createArrowUp(int w, int h, int r,
        QColor windowColor, QColor panelColor, QColor symbolColor);
};
