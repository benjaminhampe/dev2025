#pragma once
#include "PluginWidget.h"
#include <vector>
#include <QHBoxLayout>
#include <de/audio/dsp/DspTrack.h>

// ============================================
class TrackWidget : public QWidget
// ============================================
{
    Q_OBJECT
public:
    TrackWidget(QWidget* parent = nullptr);
    ~TrackWidget() override;
    void applySkin();
    void updateLayout();
    void updateLayoutOfDropTarget();

    // const std::shared_ptr<de::audio::DspTrack> &
    // getTrack() const { return m_track; }
    void setTrack(de::audio::DspTrack* track);
    const de::audio::DspTrack* getTrack() const { return m_track; }
    de::audio::DspTrack* getTrack() { return m_track; }

signals:
    void newOverview(QPixmap overview);

public slots:
    void emitTrackOverview();

protected:
    // ----------------------------------------
    // Zeichnen
    // ----------------------------------------
    void resizeEvent(QResizeEvent* e) override;
    void paintEvent(QPaintEvent* e) override;
    // ----------------------------------------
    // Drag&Drop Reorder:
    // ----------------------------------------
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    // ----------------------------------------
    // Drag&Drop
    // ----------------------------------------
    void dragEnterEvent(QDragEnterEvent* e) override;
    void dragMoveEvent(QDragMoveEvent* e) override;
    void dragLeaveEvent(QDragLeaveEvent* e) override;
    void dropEvent(QDropEvent* e) override;

    bool swapWidgets(int drag, int drop);

private:


    std::vector<de::audio::SharedPlugin> collectPlugins() const;

    std::vector<PluginWidget*> m_plugins;

    de::audio::DspTrack* m_track;

    int m_baseHeight = 376;
    int m_baseRadius = 6;
    int m_baseDropIndicatorWidth = 20;
    int m_baseDropTargetWidth = 100;
    int m_baseWidgetSpacing = 1;

    int m_width;
    int m_height;
    int m_radius;
    int m_widgetSpacing;
    int m_dropIndicatorWidth;
    int m_dropTargetWidth;
    int m_dropIndicatorPosX = -1; // Invisible
    int m_dragIndex = -1; // Only valid for reorder with mouse-click
    int m_dropIndex = -1; // Valid for reorder with mouse-click and Drag&Drop events.

    QString m_msg = "Drop files here\nVST2|VST3|CLAP|LV2 Audio Instruments/Effects\nWAV|MP3|MP4 Youtube Video and Audio files here";

    QColor m_windowColor;
    QColor m_panelColor;
    QColor m_textColor;
    QFont m_textFont;

    QRect m_rcDropIndicator;
    QRect m_rcDropTarget;
    QPoint m_posMouse;
    QPoint m_posDragInit;

    bool m_isLeftPressed = false;
    bool m_isDragInit = false;
    bool m_isDragging = false;
    bool m_isAudioOnly = false;

    bool m_bEmitOverview = true;
    bool m_bInPaintEvent = false;
    int m_overviewHeight;

    // ----------------------------------------
    // PluginWidget hinzufügen
    // ----------------------------------------
    void addPlugin(const QString &name);
    void insertPlugin(int index, const QString &name);
    void removePlugin(PluginWidget* w);

    // ----------------------------------------
    // Positionierung
    // ----------------------------------------
    // void repositionWidgets();
    int computeDragIndex(const QPoint &pos);
    int computeDropIndex(const QPoint &pos);
    int computeDropIndicatorPosX(int dragIndex, int dropIndex);

    // ----------------------------------------
    // Auto-Scroll
    // ----------------------------------------
    void startAutoScrollIfNeeded(const QPoint &pos);
    void autoScroll();
};
