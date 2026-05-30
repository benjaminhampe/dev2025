#include "gui/track/details/EditorButton.h"
#include "App.h"
#include "gui/Skin.h"

EditorButton::EditorButton(QWidget* parent)
    : SvgButton(parent)
{
    // setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    // setContextMenuPolicy(Qt::CustomContextMenu);
    // setStyleSheet("background:transparent; border:none;");
    setCheckable( true );
    setChecked( false );
    applySkin();

    // connect(this, &QWidget::customContextMenuRequested,
            // this, &EditorButton::showContextMenu);

    // connect(this, &QPushButton::toggled,
    //         this, &EditorButton::onToggled);

}

// void EditorButton::onToggled(bool checked)
// {
//     applySkin();
// }

// QSize EditorButton::sizeHint() const { return QSize(m_width, m_height); }
// QSize EditorButton::minimumSizeHint() const { return sizeHint(); }

void EditorButton::applySkin()
{
    // DE_DEBUG("")
    const auto& skin = App::instance()->getSkin();
    const int b = (m_baseButtonSize * skin.zoom) / 100;
    setFixedSize(b,b);

    const int outlineWidth = (m_baseOutlineWidth * skin.zoom) / 100;
    const auto outlineColor = QColor(79,79,79);
    const auto onColor = QColor(255,181,1);
    const auto offColor = QColor(207,207,207);
    const auto textColor = skin.textColor;

    if (m_active.width() != b)
    {
        auto mkSvg_Editor = []( int w, int h,
            const QColor& fillColor,
            const QColor& outlineColor,
            const QColor& textColor ) -> QPixmap
        {
            auto s = QString(R"(
<svg width="30" height="30" viewBox="0 0 30 30" xmlns="http://www.w3.org/2000/svg">
<g>
<circle cx="15" cy="15" r="12" fill="%1" stroke="%2" stroke-width="2" />
<circle cx="9" cy="15" r="2" fill="%3" />
<circle cx="15" cy="15" r="2" fill="%3" />
<circle cx="21" cy="15" r="2" fill="%3" />
</g>
</svg>
)")
            .arg(toSvg(fillColor))      // %1
            .arg(toSvg(outlineColor))   // %2
            .arg(toSvg(textColor));     // %3

            //dbSaveTextA( s.toStdString(), "abenton_editor.svg" );

            return mkSvg(s,w,h);
        };

        m_active = mkSvg_Editor(b,b, onColor, outlineColor, textColor);
        m_deactive = mkSvg_Editor(b,b, offColor, outlineColor, textColor);

        //m_active.save("EditorButton.Active.png");
        //m_deactive.save("EditorButton.Deactive.png");
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
void EditorButton::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/
