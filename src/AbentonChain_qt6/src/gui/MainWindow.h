#pragma once
#include <QMainWindow>
#include <QWheelEvent>
#include <gui/Keyboard2MidiNote_Mapping.h>
#include <gui/conf/AudioConfigDialog.h>
#include <gui/conf/MidiConfigDialog.h>
#include <gui/header/Header.h>
#include <gui/viz/GL_Canvas.h>
#include <gui/clip/ClipEditor.h>
#include <gui/track/ChainStack.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void keyPressEvent( QKeyEvent* event ) override;
    void keyReleaseEvent( QKeyEvent* event ) override;

private:
    void zoomIn();
    void zoomOut();
    void updateWindowTitle();
    void createMenuFile();
    void createMenuEdit();
    void createMenuView();
    void createMenuCanvas();

    Keyboard2MidiNote_Mapping m_keyboard2MidiNoteMapping;
    QString m_appTitle;
    Header* m_header;
    QWidget* m_canvasContainer;
    GL_Canvas* m_canvas;
    ClipEditor* m_clipEditor; // PianoRoll
    ChainStack* m_chainStack;
};
