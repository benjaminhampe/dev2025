#pragma once
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QTimer>
#include <QElapsedTimer>

#include "VideoDriver_Qt.h"

class Canvas : public QOpenGLWidget 
{
    Q_OBJECT
public:
    Canvas(QWidget *parent = nullptr);
    ~Canvas();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private slots:
    void updateFPS();

private:
    QTimer *m_timer;
    QElapsedTimer m_elapsedTimer;
    int m_frameCount;
    float m_fps;
    QOpenGLContext *m_context;
    VideoDriver_Qt m_driver;
};

