#include "QuickHelp.h"
#include "App.h"
#include "gui/Skin.h"

QuickHelp::QuickHelp(QWidget* parent)
    : QWidget(parent)
{
    // setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    // setContextMenuPolicy(Qt::CustomContextMenu);
    //setStyleSheet("background: transparent;"); // border:none;

    applySkin();

    // connect(this, &QWidget::customContextMenuRequested,
    // this, &QuickHelp::showContextMenu);
    // connect(this, &QPushButton::toggled,
    // this, &QuickHelp::onToggled);
}

// QSize QuickHelp::sizeHint() const { return QSize(m_width, m_height); }
// QSize QuickHelp::minimumSizeHint() const { return sizeHint(); }

void QuickHelp::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    m_zoom = skin.zoom;
    m_windowColor = skin.windowColor;
    m_panelColor = skin.panelColor;
    m_headerColor = skin.headerColor;
    m_textColor = skin.textColor;
    updateLayout();
}

void QuickHelp::updateLayout()
{
    update();
}

void QuickHelp::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    if (w < 1) return;
    if (h < 1) return;
    updateLayout();
}

void QuickHelp::paintEvent(QPaintEvent* event)
{
    const int w = width();
    const int h =  height();
    if (w < 1) return;
    if (h < 1) return;
    const int r = (6 * m_zoom) / 100;
    const int m = (8 * m_zoom) / 100;
    int headerHeight = (38 * m_zoom)/100;

    QPainter dc;
    if (dc.begin(this))
    {
        // [Draw] Window:
        dc.fillRect(rect(),m_windowColor);
        // QRect r_panel(m,0,w-m,h);

        // [Draw] Header:
        dc.setRenderHint( QPainter::Antialiasing );
        dc.setPen(Qt::NoPen);
        dc.setBrush(QBrush(m_headerColor));
        dc.drawRoundedRect(m,0,w-m,headerHeight,r,r);
        dc.drawRect(m,headerHeight/2,w-m,headerHeight-1-r);

        // [Draw] Panel:
        dc.setPen(Qt::NoPen);
        dc.setBrush(QBrush(m_panelColor));
        dc.drawRoundedRect(m,headerHeight+1,w-m,h-headerHeight-1,r,r);
        dc.drawRect(m,headerHeight+1,w-m,r+1);

        // [Draw] Text:
        dc.setPen( QPen(m_textColor) );
        dc.setBrush(Qt::NoBrush);
        QRect r_text = QRect(2*m,headerHeight+3*m,w-4*m,h-headerHeight-4*m);
        dc.drawText(r_text, Qt::TextWordWrap | Qt::AlignCenter, m_helpText, &r_text );

    }



}
/*
void QuickHelp::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/
