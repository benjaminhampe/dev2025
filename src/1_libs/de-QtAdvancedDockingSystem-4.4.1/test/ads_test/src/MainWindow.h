#pragma once

#include <QMainWindow>
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

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    ads::CDockManager* dockManager;

    void createToolbar();
    void createDockWidgets();
};
