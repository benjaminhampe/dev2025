#pragma once
#include <QWidget>
#include <QPainter>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QMenu>
#include <QTimer>
#include <QScrollBar>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QVBoxLayout>
#include <QScrollArea>
#include <vector>

namespace de {
namespace gui {
namespace track {

// ============================================
struct ShellSkin
// ============================================
{
public:
    int zoom = 100; // in percent
    int spacing = 1;
    int dropIndicatorWidth = 20;
    QColor fillColor = QColor(50,50,50);
    QColor bodyColor = QColor(50,50,50);
};

// ============================================
class Shell : public QWidget
// ============================================
{
    Q_OBJECT
public:
    Shell(const QString &fileName, QWidget *parent = nullptr);

    QString fileName() const;

protected:
    void paintEvent(QPaintEvent *) override;

public slots:
    void setIsDragging(bool);
    void setPluginPos(int x, int y);
    void setPluginIndex(int i);
public slots:
    void on_skinChanged();

private slots:
    void showContextMenu(const QPoint &pos);

signals:
    void requestRemoval(Shell *self);

private:
    QString m_strFileName;
    QString m_strIndex;
    QPoint m_pos;
    bool m_isDragging;

    ShellSkin m_skin;
};

// ============================================
class Body : public QWidget
// ============================================
{
    Q_OBJECT
public:
    Body(QWidget* parent = nullptr);
    ~Body() override;

    // ----------------------------------------
    // Speicherung / Laden
    // ----------------------------------------
    void saveState(const QString &path);
    void loadState(const QString &path);

signals:
    void reorderedWidgets();
public slots:
    void on_skinChanged();

protected:
    // ----------------------------------------
    // Drag&Drop
    // ----------------------------------------
    void dragEnterEvent(QDragEnterEvent* e) override;
    void dragMoveEvent(QDragMoveEvent* e) override;
    void dragLeaveEvent(QDragLeaveEvent* e) override;
    void dropEvent(QDropEvent* e) override;

    // ----------------------------------------
    // Zeichnen
    // ----------------------------------------
    void paintEvent(QPaintEvent* e) override;

    // ----------------------------------------
    // Drag&Drop Reorder:
    // ----------------------------------------
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    void updateLayout();

    int computeWidgetIndex(const QPoint &pos);
    void swapWidgets(int drag, int drop);
private:
    std::vector<Shell*> m_widgets;
    int m_dragIndex = -1; // Only valid for reorder with mouse-click
    int m_dropIndex = -1; // Valid for reorder with mouse-click and Drag&Drop events.

    // int m_widgetSpacing = 1;
    // int m_dropIndicatorWidth = 20;
    QRect m_dropIndicatorRect;
    QPoint m_lastDragPos;
    //QTimer* m_scrollTimer;
    //int m_scrollDirection = 0;
    bool m_isLeftPressed = false;
    bool m_isDragging = false;

    // ----------------------------------------
    // Plugin hinzufügen
    // ----------------------------------------
    void addPlugin(const QString &name);
    void insertPlugin(int index, const QString &name);
    void removePlugin(Shell *w);

    // ----------------------------------------
    // Positionierung
    // ----------------------------------------
    // void repositionWidgets();
    int computeDropIndex(const QPoint &pos);
    int computeDropX(int index);

    // ----------------------------------------
    // Auto-Scroll
    // ----------------------------------------
    void startAutoScrollIfNeeded(const QPoint &pos);
    void autoScroll();
};

// ============================================
class Track : public QWidget
// ============================================
{
    Q_OBJECT
public:
    Track(QWidget* parent = nullptr);
    ~Track() override;

private:
    Body* m_body = nullptr;
	QScrollArea* m_scrollArea = nullptr;
};


} // end namespace track
} // end namespace gui
} // end namespace de
