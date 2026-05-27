#pragma once
#include <QWidget>
#include <QPainter>

class Plugin : public QWidget {
    Q_OBJECT
public:
    Plugin(QWidget *parent = nullptr) : QWidget(parent) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    QSize sizeHint() const override {
        return QSize(120, 0);   // width hint, height flexible
    }

    QSize minimumSizeHint() const override {
        return QSize(0, 0);     // do NOT push minimum height upward
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(QColor(80, 120, 200));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(rect().adjusted(4, 4, -4, -4), 10, 10);
    }
};
