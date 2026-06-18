#pragma once
#include <QPushButton>

// ============================================
class PixButton : public QPushButton
// ============================================
{
    Q_OBJECT
public:
    PixButton(QWidget* parent = nullptr);
    //QSize sizeHint() const override;
    //QSize minimumSizeHint() const override;
    void applySkin();
    void updateLayout();
    int maxWidth() const {
        return std::max( m_active.width(), m_deactive.width() );
    }
    void setPixmaps(QPixmap pixActive, QPixmap pixDeactive);

    // Toggles background color for different mode
    void setKeyAssign( bool enabled );
    void setMidiAssign( bool enabled );

protected:
    void resizeEvent(QResizeEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

protected:
    bool m_bEnabledKeyAssign = false;
    bool m_bEnabledMidiAssign = false;

    QPixmap m_active;
    QPixmap m_deactive;
};
