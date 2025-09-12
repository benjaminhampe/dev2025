#pragma once
#include <AudioEngine.h>
#include <GL_Canvas.h>
#include <PianoWidget.h>

class MainWindow : public QWidget 
{
    AudioEngine m_audioEngine;
    QOpenGLContext* m_customContext;
    QOffscreenSurface* m_offscreenSurface;
    GL_Canvas* m_pCanvas;
    PianoWidget* m_pianoWidget;

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

};
