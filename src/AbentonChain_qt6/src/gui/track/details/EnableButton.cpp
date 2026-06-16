#include "gui/track/details/EnableButton.h"
#include "App.h"
#include "gui/Skin.h"

EnableButton::EnableButton(QWidget* parent)
    : SvgButton(parent)
{
    setCheckable( true );
    setChecked( true );
    applySkin();
}

void EnableButton::applySkin()
{
    // DE_DEBUG("")
    const auto& skin = App::instance()->getSkin();

    const int b = (m_baseButtonSize * skin.zoom) / 100;
    setFixedSize(b,b);

    //const int outlineWidth = (m_baseOutlineWidth * skin.zoom) / 100;
    const auto lineColor = QColor(79,79,79);
    const auto onColor = QColor(255,181,1);
    const auto offColor = QColor(207,207,207);
    // const auto textColor = skin.textColor;

    if (m_active.width() != b)
    {
        auto mkSvg_Power = [](int w, int h,
            const QColor& fillColor,
            const QColor& lineColor) -> QPixmap
        {
            auto s = QString(R"(
<svg width="30" height="30" viewBox="0 0 30 30" xmlns="http://www.w3.org/2000/svg" >
<circle cx="15" cy="15" r="12" fill="%1" stroke="%2" stroke-width="2" />
</svg>
)")
            .arg(toSvg(fillColor))  // %1
            .arg(toSvg(lineColor))  // %2
            ;
            return mkSvg(s,w,h);
        };

        m_active = mkSvg_Power(b,b, onColor, lineColor );
        m_deactive = mkSvg_Power(b,b, offColor, lineColor );

        //m_active.save("EnableButton.Active.png");
        //m_deactive.save("EnableButton.Deactive.png");
    }

    if (isCheckable() && !isChecked())
    {
        setButtonPix(this, m_deactive);
    }
    else
    {
        setButtonPix(this, m_active);
    }

    // updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}

/*
void EnableButton::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/
