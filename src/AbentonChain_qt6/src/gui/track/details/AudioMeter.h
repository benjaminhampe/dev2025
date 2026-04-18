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
    // bool m_bHovered = false;
    // bool m_bFocused = false;
    de::audio::IPlugin* m_plugin = nullptr;

    int m_updateTimerId = 0;

    int m_baseWidth = 4; // w = 18 = 4*(m_baseWidth)+m_baseSpacing
    int m_baseHeight = 216;
    int m_baseTop = 22;
    int m_baseSpacing = 2;

    QColor m_windowColor;
    QColor m_fillColor = QColor(36,36,36);
    QColor m_markColor = QColor(165,165,165);

    int m_width;
    int m_height;
    int m_top;
    int m_spacing;
    QRect m_rcLeft;
    QRect m_rcRight;

    QRect m_rcLeftMark;
    QRect m_rcRightMark;



    float m_Lnow = 0.0f;
    float m_Rnow = 0.0f;
    float m_Lmin = 0.0f;
    float m_Rmin = 0.0f;
    float m_Lmax = 0.0f;
    float m_Rmax = 0.0f;

//   std::vector< float > m_accumBuffer;
   // QFont5x8 m_font5x8;
   // de::LinearColorGradient m_ColorGradient;
};
