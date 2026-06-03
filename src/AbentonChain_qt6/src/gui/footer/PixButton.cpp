#include "PixButton.h"
#include <QPainter>

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
    setFixedSize(30,30);
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

void PixButton::paintEvent(QPaintEvent* event)
{
    const int w = width();
    const int h = height();
    if (w<2) return;
    if (h<2) return;

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
        const int w2 = m_deactive.width();
        const int h2 = m_deactive.height();
        const int x = (w - w2)/2;
        const int y = (h - h2)/2;
        dc.drawPixmap(x,y,m_deactive);
    }
    else
    {
        const int w2 = m_active.width();
        const int h2 = m_active.height();
        const int x = (w - w2)/2;
        const int y = (h - h2)/2;
        dc.drawPixmap(x,y,m_active);
    }

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
