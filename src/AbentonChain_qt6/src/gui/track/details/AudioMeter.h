#pragma once
#include <QWidget>

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

protected:
    void paintEvent( QPaintEvent* event ) override;
   
private:
    // bool m_bHovered = false;
    // bool m_bFocused = false;
    // int m_updateTimerId = 0;

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

    //de::audio::IDspChainElement* m_inputSignal;

    // float m_Lmin;
    // float m_Lmax;
    // float m_Rmin;
    // float m_Rmax;

//   std::vector< float > m_accumBuffer;
   // QFont5x8 m_font5x8;
   // de::LinearColorGradient m_ColorGradient;
};
