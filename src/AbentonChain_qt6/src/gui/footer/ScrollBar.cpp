#include "ScrollBar.h"
#include <QPainter>
#include <QResizeEvent>
#include "App.h"

// =================================================================
ScrollBar::ScrollBar(QWidget* parent)
// =================================================================
    : QWidget(parent)
{
    setObjectName( "Footer" );
    setContentsMargins(8,8,8,8);
    setMouseTracking( true );

    // setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    // setContextMenuPolicy(Qt::CustomContextMenu);
    setStyleSheet("background:transparent;"); //  border:none;
    // setCheckable( true );
    // setChecked( false );
    // applySkin();

    // connect(this, &QWidget::customContextMenuRequested,
            // this, &ScrollBar::showContextMenu);

    // connect(this, &QPushButton::toggled,
    //         this, &ScrollBar::onToggled);

    //
    // btn->setStyleSheet("background: transparent; border: none;");
    //setIcon(QIcon(pix));
    //setIconSize(pix.size());
    //setFixedSize(30,30);
}

void ScrollBar::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    m_zoom = skin.zoom;
    m_borderRadius = (8 * skin.zoom) / 100;
    m_borderWidth = (4 * skin.zoom) / 100;
    m_borderColor = skin.textColor;
    updateLayout();
}

void ScrollBar::updateLayout()
{
    update();
}

void ScrollBar::setPixmap(QPixmap pixmap)
{
    m_pixmap = pixmap;
    int w = m_pixmap.width();
    int h = m_pixmap.height();
    //setFixedSize(w,h);
}

void ScrollBar::setScrollbar(int scrollWidth, int scrollTotal, int scrollPos)
{
    // if (pix.isNull()) { return; }
    // m_pix = pix;
    m_scrollWidth = scrollWidth;
    m_scrollTotal = scrollTotal;
    m_scrollPos = scrollPos;
    //static_cast<Footer*>(parentWidget())->updateLayout();
}

void ScrollBar::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    if (w<1) return;
    if (h<1) return;
    updateLayout();
}

void ScrollBar::paintEvent(QPaintEvent* event)
{
    const int w = width();
    const int h = height();
    if (w<1) return;
    if (h<1) return;

    QPainter dc(this);

    if (!m_pixmap.isNull())
    {
        const int a = m_pixmap.width();
        const int b = m_pixmap.height();
        const int x = (w - a)/2;
        const int y = (h - b)/2;
        dc.drawPixmap(x,y,m_pixmap);
    }

    dc.setPen(QPen(m_borderColor,m_borderWidth));
    dc.setBrush(Qt::NoBrush);
    dc.drawRoundedRect(rect(),m_borderRadius,m_borderRadius);

}

/*
void ScrollBar::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/
