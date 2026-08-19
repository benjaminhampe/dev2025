#pragma once
#include "Canvas.h"
#include "Track.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = nullptr);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    //QSize calcWishSize() const;

public slots:
    //void on_skinChanged();

protected:
    //void updateLayout(int w, int h);
    //void resizeEvent(QResizeEvent *) override;

    //int m_zoom;
    Canvas* m_canvas;
    Track* m_track;
};
