#include "MainWindow.h"
#include "CanvasWidget.h"
#include "EditorWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Create the ADS dock manager
    dockManager = new ads::CDockManager(this);

    // Set ADS as the central widget
    setCentralWidget(dockManager);

    createToolbar();
    createDockWidgets();
}

void MainWindow::createToolbar()
{
    auto *toolbar = addToolBar("Main Toolbar");

    QAction *actionNew = new QAction("New", this);
    QAction *actionOpen = new QAction("Open", this);
    QAction *actionSave = new QAction("Save", this);

    toolbar->addAction(actionNew);
    toolbar->addAction(actionOpen);
    toolbar->addAction(actionSave);
}

void MainWindow::createDockWidgets()
{
    // --- Canvas ---
    auto *canvas = new CanvasWidget();
    auto *canvasDock = new ads::CDockWidget("Canvas");
    canvasDock->setWidget(canvas);

    auto *centerArea = dockManager->addDockWidget(ads::CenterDockWidgetArea, canvasDock);

    // --- Editor 1 ---
    auto *editor1 = new EditorWidget("Editor A");
    auto *editorDock1 = new ads::CDockWidget("Editor A");
    editorDock1->setWidget(editor1);

    dockManager->addDockWidget(ads::RightDockWidgetArea, editorDock1, centerArea);

    // --- Editor 2 ---
    auto *editor2 = new EditorWidget("Editor B");
    auto *editorDock2 = new ads::CDockWidget("Editor B");
    editorDock2->setWidget(editor2);

    dockManager->addDockWidget(ads::BottomDockWidgetArea, editorDock2, centerArea);
}
