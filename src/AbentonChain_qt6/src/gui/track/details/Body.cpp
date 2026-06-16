#include "gui/track/details/Body.h"
#include "App.h"
#include "gui/Skin.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

Body::Body(QWidget* parent)
    : QWidget(parent)
{
    // setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    // setContextMenuPolicy(Qt::CustomContextMenu);
    setStyleSheet("background: transparent;"); // border:none;

    // m_btnLoadPlugin = new EnableButton(this);
    // m_btnLoadPlugin->setChecked(false);
    // m_btnSavePreset = new EnableButton(this);
    // m_btnSavePreset->setChecked(false);
    m_cbxPreset = new ComboBox(this);
    m_pad = new Pad(this);
    m_cbxParam1 = new ComboBox(this);
    m_cbxParam2 = new ComboBox(this);

    applySkin();

    // connect(this, &QWidget::customContextMenuRequested,
    // this, &Body::showContextMenu);
    // connect(this, &QPushButton::toggled,
    // this, &Body::onToggled);
}

// QSize Body::sizeHint() const { return QSize(m_width, m_height); }
// QSize Body::minimumSizeHint() const { return sizeHint(); }

void Body::applySkin()
{
    // DE_DEBUG("")
    // m_btnLoadPlugin->applySkin();
    // m_btnSavePreset->applySkin();
    m_cbxPreset->applySkin();
    m_pad->applySkin();
    m_cbxParam1->applySkin();
    m_cbxParam2->applySkin();

    const auto& skin = App::instance()->getSkin();
    m_zoom = skin.zoom;

    updateLayout();
}


void Body::updateLayout()
{
    // DE_DEBUG("")
    const int w = width();
    const int h = height();

    int px = (8 * m_zoom) / 100;
    int py = (4 * m_zoom) / 100;
    int h1 = m_cbxPreset->height(); // (30 * m_zoom) / 100;
    int h3 = m_cbxParam2->height(); // (30 * m_zoom) / 100;
    int h2 = h - h1 - h3 - 4*py;

    int w1 = w - 2*px;
    m_cbxPreset->setGeometry(px, py, w1, h1);
    m_pad->setGeometry(px, 2*py+h1, w1, h2);

    int w2 = (w-3*px)/2;
    int w3 = w-w2;
    m_cbxParam1->setGeometry(px, h-py-h3, w2, h3);
    m_cbxParam2->setGeometry(px + w2 + px, h-py-h3, w3, h3);
    // int w1 = m_btnLoadPlugin->width();
    // int w2 = m_btnSavePreset->width();

    update();
}

void Body::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    if (w < 1) return;
    if (h < 1) return;
    updateLayout();
}

void Body::paintEvent(QPaintEvent* event)
{
    // // <debug>
    // if (!isVisible()) { return; }
    // const int w = width();
    // const int h = height();
    // if (w < 1) return;
    // if (h < 1) return;
}
/*
void Body::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/
