#pragma once
//#include <DarkAudio.h>
#include <GL_Canvas.h>
#include <ControlWidget_audioScale.h>
#include <ControlWidget_fft.h>
#include <ControlWidget_miniaudio.h>
#include <ControlWidget_camera.h>
#include <ControlWidget_cameraPos.h>
#include <ControlWidget_cameraScale.h>
#include <ControlWidget_colorGradient.h>
//#include <PianoWidget.h>
// #include <QApplication>
#include <QSplitter>
#include <QTextEdit>
#include <QWidget>

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include <QToolBar>
#include <QAction>
#include <QPointer>

#include <DockManager.h>
#include <DockWidget.h>

class CanvasWidget;
class EditorWidget;


class MainWindow : public QMainWindow
{
    Q_OBJECT

    //AudioEngine m_audioEngine;
    QOpenGLContext* m_customContext;
    QOffscreenSurface* m_offscreenSurface;

    ads::CDockManager* m_dockManager;

    GL_Canvas* m_canvas;
    //PianoWidget* m_pianoWidget;

    ControlWidget_AudioScale* m_edtVOL;
    ControlWidget_ColorGradient* m_edtLCG;
    ControlWidget_fft* m_edtFFT;
    ControlWidget_miniaudio* m_edtAUD;
    ControlWidget_Camera* m_edtCAM;
    ControlWidget_CameraPos* m_edtCAP;
    ControlWidget_CameraScale* m_edtCAS;

    ads::CDockWidget* m_dockCanvas;
    ads::CDockWidget* m_dockVOL;
    ads::CDockWidget* m_dockLCG;
    ads::CDockWidget* m_dockFFT;
    ads::CDockWidget* m_dockAUD;
    ads::CDockWidget* m_dockCAM;
    ads::CDockWidget* m_dockCAP;
    ads::CDockWidget* m_dockCAS;

/*
    QAction* m_actShowEditorLCG;
    QAction* m_actShowEditorVOL;
    QAction* m_actShowEditorFFT;
    QAction* m_actShowEditorAUD;
    QAction* m_actShowEditorCAM;
    QAction* m_actShowEditorCAP;
    QAction* m_actShowEditorCAS;
*/
    // window; QMenuBar* menuBar = window.menuBar();
    // QMenu* fileMenu = menuBar->addMenu("File");
    // QAction* quitAction = fileMenu->addAction("Quit");
    // QObject::connect(quitAction, &QAction::triggered, &app,
    //                 &QApplica
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
    GL_Canvas* getCanvas() { return m_canvas; }

protected:
    void destroy();
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
private slots:
    void on_shouldClose(bool checked = false);

/*
    void on_hiddenEditor_LCG();
    void on_hiddenEditor_VOL();
    void on_hiddenEditor_FFT();
    void on_hiddenEditor_AUD();
    void on_hiddenEditor_CAM();
    void on_hiddenEditor_CAP();
    void on_hiddenEditor_CAS();

    void on_toggleEditor_LCG(bool checked);
    void on_toggleEditor_VOL(bool checked);
    void on_toggleEditor_FFT(bool checked);
    void on_toggleEditor_AUD(bool checked);
    void on_toggleEditor_CAM(bool checked);
    void on_toggleEditor_CAP(bool checked);
    void on_toggleEditor_CAS(bool checked);
*/

    void on_gainChanged(int gain);
    void on_volumeChanged(int volume);
    void on_dBminChanged(int dB);
    void on_dBmaxChanged(int dB);
};
