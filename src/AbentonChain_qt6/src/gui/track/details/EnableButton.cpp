#include "gui/track/details/EnableButton.h"
#include "App.h"
#include "gui/Skin.h"

EnableButton::EnableButton(QWidget* parent)
    : QPushButton(parent)
{
    // setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    // setContextMenuPolicy(Qt::CustomContextMenu);
    // setStyleSheet("background:transparent; border:none;");
    setCheckable( true );
    setChecked( true );
    applySkin();

    // connect(this, &QWidget::customContextMenuRequested,
            // this, &EnableButton::showContextMenu);

    connect(this, &QPushButton::toggled,
            this, &EnableButton::onToggled);

}

void EnableButton::setEnabledKeyAssign( bool enabled )
{
    m_bEnabledKeyAssign = enabled;
    update();
}

void EnableButton::onToggled(bool checked)
{
    applySkin();
}

//QSize EnableButton::sizeHint() const { return QSize(m_width, m_height); }
//QSize EnableButton::minimumSizeHint() const { return sizeHint(); }

void EnableButton::applySkin()
{
    // DE_DEBUG("")
    const auto& skin = App::instance()->currentSkin();

    const int b = (m_baseButtonSize * skin.zoom) / 100;
    setFixedSize(b,b);

    //const int outlineWidth = (m_baseOutlineWidth * skin.zoom) / 100;
    const auto lineColor = QColor(79,79,79);
    const auto onColor = QColor(255,181,1);
    const auto offColor = QColor(207,207,207);
    // const auto textColor = skin.textColor;

    if (m_active.width() != b)
    {
        auto mkSvg_Power = [](
            int buttonWidth,
            const QColor& fillColor,
            const QColor& lineColor) -> QPixmap
        {
            const int symbolSize = buttonWidth - 6;  // 24
            const int symbolRadius = symbolSize / 2; // 12
            const int cx = buttonWidth/2;
            const int cy = cx;
            auto s = QString(R"(
<svg width="30" height="30" viewBox="0 0 30 30"
    xmlns="http://www.w3.org/2000/svg" >
<circle cx="15" cy="15" r="12"
    fill="%1" stroke="%2" stroke-width="2" />
</svg>
)")
            .arg(toSvg(fillColor))  // %1
            .arg(toSvg(lineColor))  // %2
            ;
            return mkSvg( s, buttonWidth );
        };

        m_active = mkSvg_Power(b, onColor, lineColor );
        m_deactive = mkSvg_Power(b, offColor, lineColor );

        m_active.save("EnableButton.Active.png");
        m_deactive.save("EnableButton.Deactive.png");
    }

    if (isCheckable() && !isChecked())
    {
        setButtonSvg(this, m_deactive);
    }
    else
    {
        setButtonSvg(this, m_active);
    }

    // updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}

void EnableButton::paintEvent(QPaintEvent* event)
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
void EnableButton::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/
