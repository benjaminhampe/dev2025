#include "gui/track/details/EditorButton.h"
#include "App.h"
#include "gui/Skin.h"

EditorButton::EditorButton(QWidget* parent)
    : QPushButton(parent)
{
    // setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    // setContextMenuPolicy(Qt::CustomContextMenu);
    // setStyleSheet("background:transparent; border:none;");
    setCheckable( true );
    setChecked( false );
    applySkin();

    // connect(this, &QWidget::customContextMenuRequested,
            // this, &EditorButton::showContextMenu);

    connect(this, &QPushButton::toggled,
            this, &EditorButton::onToggled);

}

void EditorButton::setEnabledKeyAssign( bool enabled )
{
    m_bEnabledKeyAssign = enabled;
    update();
}


void EditorButton::onToggled(bool checked)
{
    applySkin();
}

// QSize EditorButton::sizeHint() const { return QSize(m_width, m_height); }
// QSize EditorButton::minimumSizeHint() const { return sizeHint(); }

void EditorButton::applySkin()
{
    // DE_DEBUG("")
    const auto& skin = App::instance()->currentSkin();
    const int b = (m_baseButtonSize * skin.zoom) / 100;
    setFixedSize(b,b);

    const int outlineWidth = (m_baseOutlineWidth * skin.zoom) / 100;
    const auto outlineColor = QColor(79,79,79);
    const auto onColor = QColor(255,181,1);
    const auto offColor = QColor(207,207,207);
    const auto textColor = skin.textColor;

    if (m_active.width() != b)
    {
        auto mkSvg_Editor = []( int buttonWidth,
            const QColor& fillColor,
            const QColor& outlineColor,
            const QColor& textColor ) -> QPixmap
        {
            const int symbolSize = buttonWidth - 6;  // 24
            const int symbolRadius = symbolSize / 2; // 12
            const int cx = buttonWidth/2;
            const int cy = cx;
            auto s = QString(R"(
<svg width="30" height="30" viewBox="0 0 30 30" xmlns="http://www.w3.org/2000/svg">
<g>
<circle cx="15" cy="15" r="12"
    fill="%1" stroke="%2" stroke-width="2" />

<circle cx="9" cy="15" r="2" fill="%3" />
<circle cx="15" cy="15" r="2" fill="%3" />
<circle cx="21" cy="15" r="2" fill="%3" />
</g>
</svg>
)")
            .arg(toSvg(fillColor))      // %1
            .arg(toSvg(outlineColor))   // %2
            .arg(toSvg(textColor));     // %3

            dbSaveTextA( s.toStdString(), "abenton_editor.svg" );

            return mkSvg( s, buttonWidth );
        };

        m_active = mkSvg_Editor(b,
            onColor, outlineColor, textColor);

        m_deactive = mkSvg_Editor(b,
            offColor, outlineColor, textColor);

        m_active.save("EditorButton.Active.png");
        m_deactive.save("EditorButton.Deactive.png");
    }

    if (isCheckable() && !isChecked())
    {
        setButtonSvg( this, m_deactive);
    }
    else
    {
        setButtonSvg( this, m_active);
    }

    // updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}

void EditorButton::paintEvent(QPaintEvent* event)
{
    QPainter dc(this);
    if (m_bEnabledKeyAssign)
    {
        dc.fillRect( rect(), QColor(209, 160, 129) );
    }

    if (isCheckable() && !isChecked())
    {
        int w1 = width();
        int w2 = m_deactive.width();
        int h1 = height();
        int h2 = m_deactive.height();
        int x = (w1 - w2)/2;
        int y = (h1 - h2)/2;
        dc.drawPixmap(x,y,m_deactive);
    }
    else
    {
        int w1 = width();
        int w2 = m_active.width();
        int h1 = height();
        int h2 = m_active.height();
        int x = (w1 - w2)/2;
        int y = (h1 - h2)/2;
        dc.drawPixmap(x,y,m_active);
    }

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
