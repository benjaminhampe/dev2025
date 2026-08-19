#pragma once
#include "CentralWidget.h"
#include <QMainWindow>
#include <QEvent>
#include <QWheelEvent>
/*
updateGeometry()
    ↓
Parent bekommt LayoutRequest
    ↓
Parent ruft sizeHint() auf
    ↓
Parent entscheidet neue Geometrie
    ↓
Wenn Größe sich ändert → resizeEvent()



App::skin->setZoom(z)
    ↓
    emit zoomChanged(z)
    ↓
    CentralWidget::onZoomChanged(z)
    ↓
    CentralWidget::updateGeometry()
    ↓
    Qt relayouts MainWindow
    ↓
    Qt calls CentralWidget::resizeEvent
    ↓
    CentralWidget::resizeEvent:
                                 header->setGeometry(...)
        ↓
                                 header->resizeEvent
            ↓
                                 header lays out its children

                                 body->setGeometry(...)
        ↓
                                 body->resizeEvent
            ↓
                                 body->setGeometry(child...)
                ↓
                                 child->resizeEvent
                    ↓
                                 child lays out its children
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

private slots:
    void on_skinChanged();
protected:
    bool event(QEvent *e) override;

private:

};
