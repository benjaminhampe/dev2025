#include "gui/track/details/UpdateButton.h"
#include "App.h"
#include "gui/Skin.h"

UpdateButton::UpdateButton(QWidget* parent)
    : SvgButton(parent)
{
    // setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    // setContextMenuPolicy(Qt::CustomContextMenu);
    // setStyleSheet("background:transparent; border:none;");
    setCheckable( true );
    setChecked( false );
    applySkin();

    // connect(this, &QWidget::customContextMenuRequested,
            // this, &UpdateButton::showContextMenu);

    // connect(this, &QPushButton::toggled,
    //         this, &UpdateButton::onToggled);

}

// void UpdateButton::onToggled(bool checked)
// {
//     applySkin();
// }

// QSize UpdateButton::sizeHint() const { return QSize(m_width, m_height); }
// QSize UpdateButton::minimumSizeHint() const { return sizeHint(); }

void UpdateButton::applySkin()
{
    // DE_DEBUG("")
    const auto& skin = App::instance()->getSkin();

    const int b = (m_baseButtonSize * skin.zoom) / 100;
    setFixedSize(b,b);

    //const int outlineWidth = (m_baseOutlineWidth * skin.zoom) / 100;
    const auto lineColor = QColor(79,79,79);
    const auto onColor = QColor(255,181,1);
    const auto offColor = QColor(207,207,207);
    const auto textColor = QColor(18,18,18);

    if (m_active.width() != b)
    {
        auto mkSvg_Update = []( int w, int h,
            const QColor& fillColor,
            const QColor& lineColor,
            const QColor& textColor) -> QPixmap
        {
            auto s = QString(R"(
<svg width="30" height="30" viewBox="0 0 30 30" xmlns="http://www.w3.org/2000/svg">
<g>
<circle cx="15" cy="15" r="12" fill="%1" stroke="%2" stroke-width="2" />
<path d="M14,8 L15,8 L11,9 L10,21 L11,10 L10,10 L10,11 L9,11 L9,14 L11,14 L9,16 L8,16 L9,14z" fill="%3" stroke="%3" stroke-width="2" />
<path d="M15,21 L14,21 L14,20 L18,20 L18,19 L19,19 L19,18 L20,18 L20,15 L18,15 L20,13 L21,13 L23,15 L20,15z" fill="%3" stroke="%3" stroke-width="2" />
</g>
</svg>
)")
            .arg(toSvg(fillColor))//%1
            .arg(toSvg(lineColor))//%2
            .arg(toSvg(textColor))//%3
            ;
            //dbSaveTextA( s.toStdString(), "abenton_update.svg" );
            return mkSvg(s,w,h);
        };

        m_active = mkSvg_Update(b,b, onColor, lineColor, textColor );
        m_deactive = mkSvg_Update(b,b, offColor, lineColor, textColor );
        //m_active.save("UpdateButton.Active.png");
        //m_deactive.save("UpdateButton.Deactive.png");
    }

    if (isCheckable() && !isChecked())
    {
        setButtonPix( this, m_deactive);
    }
    else
    {
        setButtonPix( this, m_active);
    }

    // updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}

/*
void UpdateButton::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/
