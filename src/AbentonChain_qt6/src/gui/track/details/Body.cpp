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

    setStyleSheet("background: transparent;");

    // setStyleSheet("background:transparent; border:none;");

    m_btnLoadPlugin = new EnableButton(this);
    m_btnSavePreset = new WrenchButton(this);
    m_cbxPreset = new ComboBox(this);
    m_pad = new Pad(this);
    m_cbxParam1 = new ComboBox(this);
    m_cbxParam2 = new ComboBox(this);

    auto h1 = new QHBoxLayout;
    h1 -> setContentsMargins(0,0,0,0);
    h1 -> setSpacing( 4 );
    h1 -> addWidget(m_btnLoadPlugin);
    h1 -> addWidget(m_btnSavePreset);
    h1 -> addWidget(m_cbxPreset,1);

    auto h3 = new QHBoxLayout;
    h3 -> setContentsMargins(0,0,0,0);
    h3 -> setSpacing( 8 );
    h3 -> addWidget(m_cbxParam1,1);
    h3 -> addWidget(m_cbxParam2,1);

    auto v1 = new QVBoxLayout;
    v1 -> setContentsMargins(10,10,10,0);
    v1 -> setSpacing( 8 );
    v1 -> addLayout( h1 );
    v1 -> addWidget( m_pad,0, Qt::AlignCenter);
    v1 -> addLayout( h3 );

    setLayout(v1);

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
    m_btnLoadPlugin->applySkin();
    m_btnSavePreset->applySkin();
    m_cbxPreset->applySkin();
    m_pad->applySkin();
    m_cbxParam1->applySkin();
    m_cbxParam2->applySkin();

    const auto& skin = App::instance()->currentSkin();
    m_width = (m_baseWidth * skin.zoom) / 100;
    m_height = (m_baseHeight * skin.zoom) / 100;

    // int w1 = m_btnLoadPlugin->width();
    // int w2 = m_btnSavePreset->width();
    // int w3 = m_cbxPreset->width();
    // int w4 = m_pad->width();
    // int w5 = m_cbxParam1->width();
    // int w6 = m_cbxParam2->width();

    // int px = (w-w1-w2-w3) / 2; // buttonSpacing
    // m_btnLoadPlugin->move(0,0);
    // m_btnSavePreset->move(w1+px,0);
    // m_cbxPreset->move(w1+w2+2*px,0);


    // int h1 = m_btnLoadPlugin->height();
    // int h2 = m_btnSavePreset->height();
    // int h3 = m_cbxPreset->height();
    // int h4 = m_pad->height();
    // int h5 = m_cbxParam1->height();
    // int h6 = m_cbxParam2->height();

    // int x4 = (w-w4)/2;
    // int py = (h-h3-h4-h5)/2;
    // m_pad->move(x4,h3+py);

    // m_cbxParam1->move(0,h-1-h5);
    // m_cbxParam2->move(w5+);

    setFixedSize(m_width,m_height);
    //updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}

void Body::paintEvent(QPaintEvent* event)
{
    return;

    // <debug>
    if (!isVisible())
    {
        return;
    }
    // QPainter dc(this);
    // dc.setPen(QPen(QColor(255,55,55)));
    // dc.setBrush(Qt::NoBrush);
    // dc.drawRect(rect().adjusted(1,1,-1,-1));
    // </debug>
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
