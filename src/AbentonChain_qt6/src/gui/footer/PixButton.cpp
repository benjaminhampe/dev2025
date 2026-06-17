#include "PixButton.h"
#include <QPainter>
#include <QResizeEvent>

// =================================================================
PixButton::PixButton(QWidget* parent)
// =================================================================
    : QPushButton(parent)
{
    // setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    // setContextMenuPolicy(Qt::CustomContextMenu);
    // setStyleSheet("background:transparent; border:none;");
    // setCheckable( true );
    // setChecked( false );
    // applySkin();

    // connect(this, &QWidget::customContextMenuRequested,
            // this, &PixButton::showContextMenu);

    // connect(this, &QPushButton::toggled,
    //         this, &PixButton::onToggled);

    setFlat(true);
    // btn->setStyleSheet("background: transparent; border: none;");
    //setIcon(QIcon(pix));
    //setIconSize(pix.size());
    //setFixedSize(30,30);
}

void PixButton::applySkin()
{

}

void PixButton::updateLayout()
{

}

void PixButton::setPixmaps(QPixmap pixActive, QPixmap pixDeactive)
{
    m_active = pixActive;
    m_deactive = pixDeactive;
    int w = std::max(m_active.width(),m_deactive.width());
    int h = std::max(m_active.height(),m_deactive.height());
    //setMinimumSize(w,h);
    //setMaximumSize(w,h);
    setFixedSize(w,h);
}

void PixButton::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    if (w<1) return;
    if (h<1) return;
    //updateLayout();
}

void PixButton::paintEvent(QPaintEvent* event)
{
    const int w = width();
    const int h = height();
    if (w<1) return;
    if (h<1) return;

    QPainter dc(this);

    if (m_bEnabledKeyAssign)
    {
        dc.fillRect( rect(), QColor(209, 160, 129) );
    }
    else if (m_bEnabledMidiAssign)
    {
        dc.fillRect( rect(), QColor(129, 129, 209) );
    }

    if (isCheckable() && !isChecked())
    {
        if (!m_deactive.isNull())
        {
            const int a = m_deactive.width();
            const int b = m_deactive.height();
            const int x = (w - a)/2;
            const int y = (h - b)/2;
            dc.drawPixmap(x,y,m_deactive);
        }
    }
    else
    {
        if (!m_active.isNull())
        {
            const int a = m_active.width();
            const int b = m_active.height();
            const int x = (w - a)/2;
            const int y = (h - b)/2;
            dc.drawPixmap(x,y,m_active);
        }
    }

}

void PixButton::setKeyAssign( bool enabled )
{
    m_bEnabledKeyAssign = enabled;
    update();
}

void PixButton::setMidiAssign( bool enabled )
{
    m_bEnabledMidiAssign = enabled;
    update();
}

/*
void PixButton::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/
