#pragma once
#include <QPushButton>

// ============================================
class ArrowButton : public QPushButton
// ============================================
{
    Q_OBJECT
public:
    ArrowButton(QWidget* parent = nullptr);
    //QSize sizeHint() const override;
    //QSize minimumSizeHint() const override;

    void applySkin();

    void setEnabledKeyAssign( bool enabled );
protected slots:
    void onToggled( bool checked );

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_baseButtonSize = 30;
    int m_baseOutlineWidth = 2;

    bool m_bEnabledKeyAssign = false;

    QPixmap m_active;
    QPixmap m_deactive;
};
