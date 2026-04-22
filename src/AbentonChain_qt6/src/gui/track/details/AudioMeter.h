#pragma once
#include <QWidget>
#include <de/audio/plugin/IPlugin.h>

// ============================================================================
class AudioMeter : public QWidget
// ============================================================================
{
   Q_OBJECT
public:
    AudioMeter( QWidget* parent = 0 );
    // QSize sizeHint() const override;
    // QSize minimumSizeHint() const override;

	void applySkin();
    void setPlugin( de::audio::IPlugin* plugin )
    {
        m_plugin = plugin;
        update();
    }

    void playUpdateTimer();
    void stopUpdateTimer();
protected:
    void timerEvent( QTimerEvent* event ) override;
    void paintEvent( QPaintEvent* event ) override;
   
private:
    std::atomic<de::audio::IPlugin*> m_plugin;
    int m_updateTimerId;
    float m_Lnow;
    float m_Rnow;
    float m_Lmin;
    float m_Rmin;
    float m_Lmax;
    float m_Rmax;
    int m_baseWidth;
    int m_baseHeight;
    int m_baseTop;
    int m_baseSpacing;
    QColor m_fillColor;
    QColor m_markColor;
    QColor m_windowColor;
    int m_width;
    int m_height;
    int m_top;
    int m_spacing;
    QRect m_rcLeft;
    QRect m_rcRight;
    QRect m_rcLeftMark;
    QRect m_rcRightMark;
    // QFont5x8 m_font5x8;
    // de::LinearColorGradient m_ColorGradient;
};
