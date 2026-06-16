#include "gui/track/details/ArrowButton.h"
#include "App.h"
#include "gui/Skin.h"

// =================================================================
ArrowButton::ArrowButton(QWidget* parent)
// =================================================================
    : SvgButton(parent)
{
    setCheckable( true );
    setChecked( false );
    applySkin();
}

void ArrowButton::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    const int b = (m_baseButtonSize * skin.zoom) / 100;
    setFixedSize(b,b);

    const auto lineColor = QColor(79,79,79);
    const auto onColor = QColor(255,181,1);
    const auto offColor = QColor(207,207,207);
    const auto textColor = QColor(18,18,18);

    if (m_active.width() != b)
    {
        auto mkSvg_ArrowDown = [](int w, int h,
                                  const QColor& fillColor,
                                  const QColor& lineColor,
                                  const QColor& textColor) -> QPixmap
        {
            auto s = QString(R"(
<svg width="30" height="30" viewBox="0 0 30 30" xmlns="http://www.w3.org/2000/svg">
<g>
<circle cx="15" cy="15" r="12" fill="%1" stroke="%2" stroke-width="2" />
<path d="M8,10 L21,10 L15,21 L14,21 L8,10z" fill="%3" />
</g>
</svg>
)")
            .arg(toSvg(fillColor))  // %1
            .arg(toSvg(lineColor))  // %2
            .arg(toSvg(textColor))  // %3
            ;

            // dbSaveTextA( s.toStdString(), "abenton_arrow_down.svg" );

            return mkSvg(s,w,h);
        };

        auto mkSvg_ArrowRight = [](int w, int h,
                                   const QColor& fillColor,
                                   const QColor& lineColor,
                                   const QColor& textColor) -> QPixmap
        {
            auto s = QString(R"(
<svg width="30" height="30" viewBox="0 0 30 30" xmlns="http://www.w3.org/2000/svg" >
<g>
<circle cx="15" cy="15" r="12" fill="%1" stroke="%2" stroke-width="2" />
<path d="M10,8 L21,14 L21,15 L10,21 L10,8z" fill="%3" />
</g>
</svg>
)")
            .arg(toSvg(fillColor))  // %1
            .arg(toSvg(lineColor))  // %2
            .arg(toSvg(textColor))  // %3
            ;

            // dbSaveTextA( s.toStdString(), "abenton_arrow_right.svg" );

            return mkSvg(s,w,h);
        };


        m_active = mkSvg_ArrowDown(b,b, offColor, lineColor, textColor );
        m_deactive = mkSvg_ArrowRight(b,b, offColor, lineColor, textColor );

        //m_active.save("ArrowButton.Active.png");
        //m_deactive.save("ArrowButton.Deactive.png");
    }

    if (isCheckable() && !isChecked())
    {
        setButtonPix( this, m_deactive);
    }
    else
    {
        setButtonPix( this, m_active);
    }

    //updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}
