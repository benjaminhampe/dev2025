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
    bool eventFilter(QObject *obj, QEvent *event) override;

    void updateWindowTitle();
private:
    void zoomIn();
    void zoomOut();

    QString m_appTitle;
};
