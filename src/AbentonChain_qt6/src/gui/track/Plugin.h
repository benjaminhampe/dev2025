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

#include <de/audio/plugin/IPlugin.h>

// ============================================
class Plugin : public QWidget
// ============================================
{
    Q_OBJECT
public:
    Plugin(de::audio::IPlugin* plugin, QWidget* parent = nullptr);
    ~Plugin() override;
    //QSize sizeHint() const override;
    //QSize minimumSizeHint() const override;

    de::audio::IPlugin* getPlugin();

    void setPlugin(de::audio::IPlugin* plugin);

    QRect labelRect() const;

    void applySkin();

protected:
    void resizeEvent(QResizeEvent *) override;
    void paintEvent(QPaintEvent *) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
public slots:
    // void setIsDragging(bool);

private slots:
    void on_pad(float x, float y);
    void on_comboParam1(int index);
    void on_comboParam2(int index);

    void on_editorWindowClosed();

    void on_showContextMenu(const QPoint &pos);
    void on_pressedBtnEnable(bool checked);
    void on_pressedBtnExpand(bool checked);
    void on_pressedBtnWrench(bool checked);
    void on_pressedBtnUpdate(bool checked);
    void on_pressedBtnEditor(bool checked);
    void on_doubleClickedLabel();
    void on_dragStarted(QPoint dragStart);

signals:
    void requestRemoval(Plugin *self);

private:
    //uint32_t m_trackId = 0; // AudioCentral
    //uint32_t m_pluginId = 0; // AudioCentral
    de::audio::IPlugin* m_plugin = nullptr; // AudioCentral

    QString m_title;

    int m_baseWidth = 300;
    int m_baseHeight = 376;
    int m_baseRadius = 6;
    int m_baseHeaderWidth = 38;
    int m_baseHeaderHeight = 34;
    int m_baseButtonSize = 30;
    int m_baseSpacing4 = 4;
    int m_baseSpacing2 = 2;

    // RealTime Draw infos:
    int m_width;
    int m_height;
    int m_radius;
    int m_headerHeight;
    int m_buttonSize;
    int m_spacing2;
    int m_spacing4;

    bool m_bFocused = false;
    bool m_bHovered = false;
    bool m_bCollapsed = false;

    QRect m_rcHeader;
    QRect m_rcBody;
    QRect m_rcLabel;
    QRect m_rcAudioMeter;

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
