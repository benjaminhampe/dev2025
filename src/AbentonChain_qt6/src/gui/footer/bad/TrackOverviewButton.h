#pragma once
#include <QPushButton>

// ============================================
class TrackOverviewButton : public QPushButton
// ============================================
{
    Q_OBJECT
public:
    TrackOverviewButton(QWidget* parent = nullptr);
    //QSize sizeHint() const override;
    //QSize minimumSizeHint() const override;

    // Toggles background color for different mode
    void setKeyAssign( bool enabled );
    void setMidiAssign( bool enabled );

    void setPixmaps(QPixmap pixActive, QPixmap pixDeactive);
// protected slots:
//     void onToggled( bool checked );

protected:
    void paintEvent(QPaintEvent* event) override;

protected:
    bool m_bEnabledKeyAssign = false;
    bool m_bEnabledMidiAssign = false;

    QPixmap m_active;
    QPixmap m_deactive;
};
