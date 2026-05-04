#pragma once
#include <QMainWindow>
#include <QWheelEvent>
#include <gui/conf/AudioConfigDialog.h>
#include <gui/conf/MidiConfigDialog.h>
#include <gui/Keyboard2MidiNote_Mapping.h>
#include <gui/viz/GL_Canvas.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

    // Use keyboard as MIDI master keyboard
    void keyPressEvent( QKeyEvent* event ) override;
    void keyReleaseEvent( QKeyEvent* event ) override;

    void updateWindowTitle();

    void on_exitProgram();
    void on_openMidiConfigDialog();
    void on_openAudioConfigDialog();
    void on_vizualizeEnabled( bool bChecked );
    void on_vizualizePerfOverlay( bool bChecked );
    void on_vizualizeFftMatrix( bool bChecked );

    void createMenuFft();

private:
    void zoomIn();
    void zoomOut();

    Keyboard2MidiNote_Mapping m_keyboard2MidiNoteMapping;

    QString m_appTitle;

    //AudioEngine m_audioEngine;
    // QOpenGLContext* m_customContext;
    // QOffscreenSurface* m_offscreenSurface;
    QWidget* m_canvasContainer;
    GL_Canvas* m_canvas;
};
