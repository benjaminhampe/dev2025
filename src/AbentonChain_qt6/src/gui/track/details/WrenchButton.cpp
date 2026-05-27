#include "gui/track/details/WrenchButton.h"
#include "App.h"
#include "gui/Skin.h"

WrenchButton::WrenchButton(QWidget* parent)
    : QPushButton(parent)
{
    // setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    // setContextMenuPolicy(Qt::CustomContextMenu);
    // setStyleSheet("background:transparent; border:none;");
    setCheckable( true );
    setChecked( false );
    applySkin();

    // connect(this, &QWidget::customContextMenuRequested,
            // this, &WrenchButton::showContextMenu);

    connect(this, &QPushButton::toggled,
            this, &WrenchButton::onToggled);

}

void WrenchButton::setEnabledKeyAssign( bool enabled )
{
    m_bEnabledKeyAssign = enabled;
    update();
}

void WrenchButton::onToggled(bool checked)
{
    applySkin();
}

// QSize WrenchButton::sizeHint() const { return QSize(m_width, m_height); }
// QSize WrenchButton::minimumSizeHint() const { return sizeHint(); }

void WrenchButton::applySkin()
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
        auto mkSvg_Wrench = [](
            int buttonWidth, const QColor& fillColor,
            const QColor& outlineColor) -> QPixmap
        {
            const int symbolSize = buttonWidth - 6;  // 24
            const int symbolRadius = symbolSize / 2; // 12
            const int cx = buttonWidth/2;
            const int cy = cx;
            auto s = QString(R"(
<svg width="30" height="30" viewBox="0 0 30 30" xmlns="http://www.w3.org/2000/svg">
<g>
<circle cx="15" cy="15" r="12" stroke-width="2"
    stroke="%1" fill="%2" />
<path d="M8,19  L8,20  L9,21  L10,21
         L16,15 L19,15 L22,12 L22,10
         L20,12 L17,12 L17,9  L19,7
         L17,7  L14,10 L14,13 L8,19z" />
</g>
</svg>
)")
            .arg(toSvg(outlineColor))//%1
            .arg(toSvg(fillColor));  //%2

            // dbSaveTextA( s.toStdString(), "abenton_wrench.svg" );

            return mkSvg( s, buttonWidth );
        };

        m_active = mkSvg_Wrench(b, onColor, outlineColor );
        m_deactive = mkSvg_Wrench(b, offColor, outlineColor );
        //m_active.save("WrenchButton.Active.png");
        //m_deactive.save("WrenchButton.Deactive.png");
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

void WrenchButton::paintEvent(QPaintEvent* event)
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
void WrenchButton::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/
