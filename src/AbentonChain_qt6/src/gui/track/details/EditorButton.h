#pragma once
#include <gui/track/details/SvgButton.h>

// ============================================
class EditorButton : public SvgButton
// ============================================
{
    Q_OBJECT
public:
    EditorButton(QWidget* parent = nullptr);
    //QSize sizeHint() const override;
    //QSize minimumSizeHint() const override;

    void applySkin();

// protected slots:
//     void onToggled( bool checked );

};
