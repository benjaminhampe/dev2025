#pragma once
#include <QPushButton>

// ============================================
class SvgButton : public QPushButton
// ============================================
{
    Q_OBJECT
public:
    SvgButton(QWidget* parent = nullptr);
    //QSize sizeHint() const override;
    //QSize minimumSizeHint() const override;

    void applySkin();

    // Toggles background color for different mode
    void setKeyAssign( bool enabled );
    void setMidiAssign( bool enabled );

// protected slots:
//     void onToggled( bool checked );

protected:
    void paintEvent(QPaintEvent* event) override;

protected:
    int m_baseButtonSize = 30;
    int m_baseOutlineWidth = 2;

    bool m_bEnabledKeyAssign = false;
    bool m_bEnabledMidiAssign = false;

    QPixmap m_active;
    QPixmap m_deactive;
};
