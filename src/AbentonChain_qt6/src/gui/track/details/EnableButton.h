#pragma once
#include <gui/track/details/SvgButton.h>

// ============================================
class EnableButton : public SvgButton
// ============================================
{
    Q_OBJECT
public:
    EnableButton(QWidget* parent = nullptr);
    //QSize sizeHint() const override;
    //QSize minimumSizeHint() const override;

    void applySkin();

// protected slots:
//     void onToggled( bool checked );
};
