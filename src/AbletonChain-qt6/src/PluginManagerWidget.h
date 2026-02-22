#pragma once
#include "PluginWidget.h"
#include <QScrollArea>
#include <vector>

// ------------------------------------------------------------
class PluginManagerWidget : public QWidget
// ------------------------------------------------------------
{
    Q_OBJECT
public:
    PluginManagerWidget(QWidget* parent = nullptr);
    ~PluginManagerWidget() override;

    // ------------------------------------------------------------
    // Speicherung / Laden
    // ------------------------------------------------------------
    void saveState(const QString &path);
    void loadState(const QString &path);

protected:
    // ------------------------------------------------------------
    // Drag&Drop
    // ------------------------------------------------------------
    void dragEnterEvent(QDragEnterEvent* e) override;
    void dragMoveEvent(QDragMoveEvent* e) override;
    void dragLeaveEvent(QDragLeaveEvent* e) override;
    void dropEvent(QDropEvent* e) override;

    // ------------------------------------------------------------
    // Zeichnen
    // ------------------------------------------------------------
    void paintEvent(QPaintEvent* e) override;

    // ------------------------------------------------------------
    // Drag&Drop Reorder:
    // ------------------------------------------------------------
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    void updateLayout();
    int m_spacing = 10;

    int computeWidgetIndex(const QPoint &pos);
    int m_dragIndex = -1;
    QRect m_dragRect;
    QRect m_rcDropIndicator;
private:
    std::vector<PluginWidget*> m_widgets;
    int m_dropIndex = -1;
    int m_dropWidth = 20;
    QPoint m_lastDragPos;
    QTimer* m_scrollTimer;
    int m_scrollDirection = 0;

    // ------------------------------------------------------------
    // Plugin hinzufügen
    // ------------------------------------------------------------
    void addPlugin(const QString &name);
    void insertPlugin(int index, const QString &name);
    void removePlugin(PluginWidget *w);

    // ------------------------------------------------------------
    // Positionierung
    // ------------------------------------------------------------
    // void repositionWidgets();
    int computeDropIndex(const QPoint &pos);
    int computeDropX(int index);

    // ------------------------------------------------------------
    // Auto-Scroll
    // ------------------------------------------------------------
    void startAutoScrollIfNeeded(const QPoint &pos);
    void autoScroll();
};

