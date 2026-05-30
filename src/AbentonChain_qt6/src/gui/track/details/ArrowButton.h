#pragma once
#include <gui/track/details/SvgButton.h>

// ============================================
class ArrowButton : public SvgButton
// ============================================
{
    Q_OBJECT
public:
    ArrowButton(QWidget* parent = nullptr);
    //QSize sizeHint() const override;
    //QSize minimumSizeHint() const override;

    void applySkin();

// protected slots:
//     void onToggled( bool checked );
};
