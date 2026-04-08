#pragma once
#include <QPushButton>

// ============================================
class EnableButton : public QPushButton
// ============================================
{
    Q_OBJECT
public:
    EnableButton(QWidget* parent = nullptr);
    //QSize sizeHint() const override;
    //QSize minimumSizeHint() const override;

    void applySkin();

    void setEnabledKeyAssign( bool enabled );

protected slots:
    void onToggled( bool checked );

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_baseButtonSize = 30; // -6 = symbolSize
    int m_baseOutlineWidth = 2;

    // int m_w;
    // int m_h;

    bool m_bEnabledKeyAssign = false;

    QPixmap m_active;
    QPixmap m_deactive;
};
