#pragma once
#include <QWidget>

// ============================================
class Pad : public QWidget
// ============================================
{
    Q_OBJECT
public:
    Pad(QWidget* parent = nullptr);
    // QSize sizeHint() const override;
    // QSize minimumSizeHint() const override;

    void applySkin();
    void updateLayout();

    enum eText
    {
        eT_Type = 0,
        eT_Runtime, //  of dsp_read()
        eT_Name,
        eT_Vendor,
        //eT_Version
        eT_Max
    };

    void setText(eText i, QString msg)
    {
        switch (i)
        {
            case eT_Type:   m_str0 = msg; break;
            case eT_Runtime:m_str1 = msg; break;
            case eT_Name:   m_str2 = msg; break;
            case eT_Vendor: m_str3 = msg; break;
            //case eT_Version:m_str4 = msg; break;
            default: break;
        }
    }

signals:
    void onParamChanged(float x, float y);

public slots:

    void setValueXY(float fx, float fy);
    void setValueX(float fx);
    void setValueY(float fy);

    // void setParamX(uint32_t id) { m_paramX = id; }
    // void setParamY(uint32_t id) { m_paramY = id; }

protected slots:
    // void onToggled( bool checked );

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    // void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    int m_zoom = 100;

    // int m_baseWidth = 280;
    // int m_baseHeight = 250;
    int m_baseBorderRadius = 4;
    int m_baseCircleRadius = 13;
    int m_baseCircleBorder = 4;

    // int m_width;
    // int m_height;
    int m_borderRadius;
    int m_circleRadius;
    int m_circleBorder;

    float m_fx = 0.0f;
    float m_fy = 0.0f;

    bool m_bIsDragging = false;

    QRect m_rcBody;

    QString m_str0 = "VST3 Effect"; // VST3 <Effect|Synth>
    QString m_str1 = "Runtime"; // Runtime of dsp_read
    QString m_str2 = "Name"; // Name
    QString m_str3 = "Vendor"; // Vendor
    QString m_str4 = ""; // Version

    std::optional<uint32_t> m_paramX;
    std::optional<uint32_t> m_paramY;

};
