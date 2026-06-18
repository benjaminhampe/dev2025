#pragma once
#include <QMainWindow>
#include <QWheelEvent>
#include <gui/Keyboard2MidiNote_Mapping.h>
#include <gui/conf/AudioConfigDialog.h>
#include <gui/conf/MidiConfigDialog.h>
#include <gui/CentralWidget.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

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
    void updateMenuView();
    void createMenuCanvas();

    Keyboard2MidiNote_Mapping m_keyboard2MidiNoteMapping;
    QString     m_appTitle;

    CentralWidget* m_body;

    // createMenuView
    QAction* m_actShowHeader{ nullptr };
    QAction* m_actShowArrange{ nullptr };
    QAction* m_actShowCanvas{ nullptr };
    QAction* m_actShowPianoRoll{ nullptr };
    QAction* m_actShowQuickHelp{ nullptr };
    QAction* m_actShowChain{ nullptr };
    QAction* m_actShowFooter{ nullptr };
};
