#pragma once
#include <QMainWindow>
#include <QWheelEvent>

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
private:
    void zoomIn();
    void zoomOut();

    QString m_appTitle;
};
