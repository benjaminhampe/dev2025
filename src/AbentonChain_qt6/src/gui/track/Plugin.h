#pragma once
#include "gui/track/details/EnableButton.h"
#include "gui/track/details/ArrowButton.h"
#include "gui/track/details/WrenchButton.h"
#include "gui/track/details/UpdateButton.h"
#include "gui/track/details/EditorButton.h"
#include "gui/track/details/Body.h"
#include "gui/track/details/AudioMeter.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <de/audio/plugin/PluginFactory.h>

// ============================================
class Plugin : public QWidget
// ============================================
{
    Q_OBJECT
public:
    Plugin(de::audio::SharedPlugin plugin, QWidget* parent = nullptr);
    ~Plugin() override;
    //QSize sizeHint() const override;
    //QSize minimumSizeHint() const override;
    QRect labelRect() const { return m_rcLabel; }
    de::audio::SharedPlugin getPlugin() { return m_plugin; }

    void setPlugin(de::audio::SharedPlugin plugin);

    void applySkin();
    void updateLayout();
    int computeBestWidth() const;

signals:
    void requestRemoval(Plugin *self);
    void collapseChanged(); // Trigger relayout of Track

protected:
    void resizeEvent(QResizeEvent*) override;
    void paintEvent(QPaintEvent*) override;

    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;

    void enterEvent(QEnterEvent*) override;
    void leaveEvent(QEvent*) override;
    void focusInEvent(QFocusEvent*) override;
    void focusOutEvent(QFocusEvent*) override;
public slots:
    // void setIsDragging(bool);

private slots:
    void on_pad(float x, float y);
    void on_comboPreset(int index);
    void on_comboParam1(int index);
    void on_comboParam2(int index);

    void on_editorWindowClosed();

    void on_showContextMenu(const QPoint &pos);
    void on_pressedBtnEnable(bool checked);
    void on_pressedBtnExpand(bool checked);
    void on_pressedBtnWrench(bool checked);
    void on_pressedBtnUpdate(bool checked);
    void on_pressedBtnEditor(bool checked);
    // void on_doubleClickedLabel();
    // void on_dragStarted(QPoint dragStart);



private:
    void unloadPlugin();
    void loadPlugin(de::audio::SharedPlugin);

    //uint32_t m_trackId = 0; // AudioCentral
    //uint32_t m_pluginId = 0; // AudioCentral
    de::audio::SharedPlugin m_plugin; // AudioCentral

    QString m_title;

    int m_zoom = 100;
    //int m_baseHeaderWidth = 38;
    //int m_baseHeaderHeight = 34;

    // int m_baseWidth = 300;
    // int m_baseHeight = 376;
    int m_baseRadius = 6;
    int m_baseButtonSize = 30;
    int m_baseSpacing4 = 4;
    int m_baseSpacing2 = 2;

    // RealTime Draw infos:
    // int m_width;
    // int m_height;
    int m_headerHeight; // Paint needs it.
    int m_radius;
    int m_buttonSize;
    int m_spacing2;
    int m_spacing4;

    bool m_bFocused = false;
    bool m_bHovered = false;
    bool m_bCollapsed = false;

    QRect m_rcHeader;
    QRect m_rcBody;
    QRect m_rcLabel;
    QFont m_headerFont;

    QColor m_windowColor;
    QColor m_panelColor;
    QColor m_headerColor;
    QColor m_headerColorActive;
    QColor m_textColor;

    EnableButton* m_btnEnable = nullptr;
    ArrowButton* m_btnExpand = nullptr;
    WrenchButton* m_btnWrench = nullptr;
    UpdateButton* m_btnUpdate = nullptr;
    EditorButton* m_btnEditor = nullptr;
    Body* m_body = nullptr;
    AudioMeter* m_audioMeter;
};
