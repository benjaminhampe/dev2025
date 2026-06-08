#pragma once
#include "Plugin.h"
#include <vector>
#include <QHBoxLayout>
#include <de/audio/Track.h>

// ============================================
class Track : public QWidget
// ============================================
{
    Q_OBJECT
public:
    Track(de::audio::Track* track, QWidget* parent = nullptr);
    ~Track() override;

    const de::audio::Track* getTrack() const { return m_track; }
    de::audio::Track* getTrack() { return m_track; }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void applySkin();
    void updateLayout();
    void updateLayoutOfDropTarget();

    // QSize sizeHint() const override;

    // ----------------------------------------
    // Speicherung / Laden
    // ----------------------------------------
    void saveState(const QString &path);
    void loadState(const QString &path);

signals:
    //void reorderedWidgets();

    void newOverview(QPixmap overview);

public slots:
    void emitTrackOverview();

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
    void resizeEvent(QResizeEvent* e) override;

    // ----------------------------------------
    // Drag&Drop Reorder:
    // ----------------------------------------
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;


    bool swapWidgets(int drag, int drop);

private:

    std::vector<de::audio::SharedPlugin>
    collectPlugins() const
    {
        std::vector<de::audio::SharedPlugin> plugins;
        plugins.reserve( m_plugins.size() );
        for (auto pluginWidget : m_plugins)
        {
            if (pluginWidget)
            {
                de::audio::SharedPlugin audioPlugin = pluginWidget->getPlugin();
                if (audioPlugin)
                {
                    plugins.push_back( audioPlugin );
                }
            }
        }

        return plugins;
    }


    // void updatePluginIds()
    // {
    //     m_pluginIds.clear();
    //     m_pluginIds.reserve( m_plugins.size() );
    //     for (auto p : m_plugins)
    //     {
    //         m_pluginIds.push_back( p->getPluginId() );
    //     }
    // }

    de::audio::Track* m_track;
    // uint32_t m_trackId = 0; // AudioCentral
    // std::vector<uint32_t> m_pluginIds; // AudioCentral

    int m_baseHeight = 376;
    int m_baseRadius = 6;
    int m_baseDropIndicatorWidth = 20;
    int m_baseDropTargetWidth = 100;
    int m_baseWidgetSpacing = 1;

    int m_width;
    int m_height;
    int m_radius;
    int m_widgetSpacing = 1;
    int m_dropIndicatorWidth = 20;
    int m_dropIndicatorPosX = -1; // Invisible
    int m_dropTargetWidth;

    QString m_msg1 = "Ziehen Sie Audio-Effekte hierhin";
    QString m_msg2 = "Drop files here\nVST2|VST3|CLAP|LV2 Audio Instruments/Effects\nWAV|MP3|MP4 Youtube Video and Audio files here";

    QColor m_windowColor;
    QColor m_panelColor;
    QColor m_textColor;
    QFont m_textFont;

    std::vector<Plugin*> m_plugins;

    int m_dragIndex = -1; // Only valid for reorder with mouse-click
    int m_dropIndex = -1; // Valid for reorder with mouse-click and Drag&Drop events.

    QRect m_rcDropIndicator;
    QRect m_rcDropTarget;
    QPoint m_posMouse;
    QPoint m_posDragInit;
    //QTimer* m_scrollTimer;
    //int m_scrollDirection = 0;
    bool m_isLeftPressed = false;
    bool m_isDragInit = false;
    bool m_isDragging = false;
    bool m_isAudioOnly = false;

    bool m_bEmitOverview = true;
    bool m_bInPaintEvent = false;
    int m_overviewHeight;

    // ----------------------------------------
    // Plugin hinzufügen
    // ----------------------------------------
    void addPlugin(const QString &name);
    void insertPlugin(int index, const QString &name);
    void removePlugin(Plugin* w);

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
