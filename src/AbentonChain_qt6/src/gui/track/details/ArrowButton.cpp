#include "gui/track/details/ArrowButton.h"
#include "App.h"
#include "gui/Skin.h"

// =================================================================
ArrowButton::ArrowButton(QWidget* parent)
// =================================================================
    : QPushButton(parent)
{
    // setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    // setContextMenuPolicy(Qt::CustomContextMenu);
    // setStyleSheet("background:transparent; border:none;");
    setCheckable( true );
    setChecked( false );
    applySkin();

    // connect(this, &QWidget::customContextMenuRequested,
            // this, &ArrowButton::showContextMenu);

    connect(this, &QPushButton::toggled,
            this, &ArrowButton::onToggled);

}

void ArrowButton::setEnabledKeyAssign( bool enabled )
{
    m_bEnabledKeyAssign = enabled;
    update();
}

void ArrowButton::onToggled(bool checked)
{
    applySkin();
}

//QSize ArrowButton::sizeHint() const { return QSize(m_width, m_height); }
//QSize ArrowButton::minimumSizeHint() const { return sizeHint(); }

void ArrowButton::applySkin()
{
    const auto& skin = App::instance()->currentSkin();
    const int b = (m_baseButtonSize * skin.zoom) / 100;
    setFixedSize(b,b);

    const auto lineColor = QColor(79,79,79);
    const auto onColor = QColor(255,181,1);
    const auto offColor = QColor(207,207,207);
    const auto textColor = QColor(18,18,18);

    if (m_active.width() != b)
    {
        auto mkSvg_ArrowDown = [](int buttonWidth,
                                  const QColor& fillColor,
                                  const QColor& lineColor,
                                  const QColor& textColor) -> QPixmap
        {
            const int symbolSize = buttonWidth - 6;  // 24
            const int symbolRadius = symbolSize / 2; // 12
            const int cx = buttonWidth/2;
            const int cy = cx;

            //  stroke="%3" stroke-width="2"
            auto s = QString(R"(
<svg width="30" height="30" viewBox="0 0 30 30" xmlns="http://www.w3.org/2000/svg">
<g>
<circle cx="15" cy="15" r="12"
    fill="%1" stroke="%2" stroke-width="2" />
<path d="M8,10 L21,10 L15,21 L14,21 L8,10z"
    fill="%3" />
</g>
</svg>
)")
            .arg(toSvg(fillColor))  // %1
            .arg(toSvg(lineColor))  // %2
            .arg(toSvg(textColor))  // %3
            ;

            dbSaveTextA( s.toStdString(), "abenton_arrow_down.svg" );

            return mkSvg( s, buttonWidth );
        };

        auto mkSvg_ArrowRight = [](int buttonWidth,
                                   const QColor& fillColor,
                                   const QColor& lineColor,
                                   const QColor& textColor) -> QPixmap
        {
            const int symbolSize = buttonWidth - 6;  // 24
            const int symbolRadius = symbolSize / 2; // 12
            const int cx = buttonWidth/2;
            const int cy = cx;

            // stroke="%3" stroke-width="2"
            auto s = QString(R"(
<svg width="30" height="30" viewBox="0 0 30 30"
    xmlns="http://www.w3.org/2000/svg" >
<g>
<circle cx="15" cy="15" r="12"
    fill="%1" stroke="%2" stroke-width="2" />
<path d="M10,8 L21,14 L21,15 L10,21 L10,8z"
    fill="%3" />
</g>
</svg>
)")
            .arg(toSvg(fillColor))  // %1
            .arg(toSvg(lineColor))  // %2
            .arg(toSvg(textColor))  // %3
            ;

            dbSaveTextA( s.toStdString(), "abenton_arrow_right.svg" );

            return mkSvg( s, buttonWidth );
        };


        m_active = mkSvg_ArrowDown(b, onColor,
                                lineColor, textColor );
        m_deactive = mkSvg_ArrowRight(b, offColor,
                                lineColor, textColor );

        m_active.save("ArrowButton.Active.png");
        m_deactive.save("ArrowButton.Deactive.png");
    }

    if (isCheckable() && !isChecked())
    {
        setButtonSvg( this, m_deactive);
    }
    else
    {
        setButtonSvg( this, m_active);
    }

    //updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}

void ArrowButton::paintEvent(QPaintEvent* event)
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
void ArrowButton::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/
