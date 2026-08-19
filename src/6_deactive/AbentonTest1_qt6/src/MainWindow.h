#pragma once
#include "Track.h"
#include <QMainWindow>
#include <QEvent>
#include <QWheelEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

protected:
    bool event(QEvent *e) override;

private:
    void updateZoom();

    Track* m_track;
};
