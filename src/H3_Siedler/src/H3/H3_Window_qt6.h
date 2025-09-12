#pragma once

#ifdef HAVE_QT6

#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QSurfaceFormat>
#include <QOffscreenSurface>

#include <QTimer>
#include <QDebug>

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLFunctions_4_5_Core>

#include <QtGlobal>
#include <QTimerEvent>
#include <QMouseEvent>

//#include <QOpenGLShaderProgram>
//#include <QOpenGLBuffer>

#include <H3/H3_Game.h>

#include <QApplication>
#include <QWidget>
#include <QKeyEvent>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QDial>
#include <QLabel>

#include <GLWidget.h>

#include <QLabel>
#include <QSplitter>
#include <QTextBrowser>

// ===========================================================================
class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
// ===========================================================================
{
    Q_OBJECT
private:
    int m_fpsTimerId;
    int m_fpsTarget = 60;

    //int m_screenWidth = 800;
    //int m_screenHeight = 600;
    
    QOpenGLContext* m_sharedContext;

    H3_Game* m_game;

public:
    GLWidget(H3_Game* game, QOpenGLContext *sharedContext, QWidget *parent = nullptr);
    ~GLWidget() override;

    void startFpsTimer();
    void stopFpsTimer();

protected:
    void timerEvent(QTimerEvent* event) override;

    void deinitializeGL();

    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

    void mousePressEvent( QMouseEvent* event ) override;

    void mouseReleaseEvent( QMouseEvent* event ) override;

    void mouseMoveEvent( QMouseEvent* event ) override;

    void wheelEvent( QWheelEvent* event ) override;

};


// ===========================================================================
class Window : public QWidget
// ===========================================================================
{
    //AudioEngine m_audioEngine;
    QOpenGLContext* m_customContext;
    QOffscreenSurface* m_offscreenSurface;

    QSplitter *m_splitter;

    GLWidget* m_GLWidget;

    //PianoWidget* m_pianoWidget;
    QTextBrowser *m_helpWidget;

public:
    Window(QWidget* parent = nullptr);

    ~Window() override;

protected:

    void keyPressEvent( QKeyEvent* event ) override;

    void keyReleaseEvent( QKeyEvent* event ) override;
};


u32 translateQKey( int qtkey );

#endif
