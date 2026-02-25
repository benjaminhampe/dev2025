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

signals:
    void reorderedWidgets();

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

    int computeWidgetIndex(const QPoint &pos);
    void swapWidgets(int drag, int drop);
private:
    std::vector<PluginWidget*> m_widgets;
    int m_widgetSpacing = 1;
    int m_dragIndex = -1; // Only valid for reorder with mouse-click
    int m_dropIndex = -1; // Valid for reorder with mouse-click and Drag&Drop events.
    int m_dropIndicatorWidth = 20;
    QRect m_dropIndicatorRect;
    QPoint m_lastDragPos;
    QTimer* m_scrollTimer;
    int m_scrollDirection = 0;
    bool m_isLeftPressed = false;
    bool m_isDragging = false;
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

