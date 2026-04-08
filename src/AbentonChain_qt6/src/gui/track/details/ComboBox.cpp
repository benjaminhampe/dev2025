#include "gui/track/details/ComboBox.h"
#include "App.h"
#include "gui/Skin.h"

ComboBox::ComboBox(QWidget* parent)
    : QComboBox(parent)
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    // setContextMenuPolicy(Qt::CustomContextMenu);

    // setStyleSheet("background:transparent; border:none;");

    applySkin();

    // connect(this, &QWidget::customContextMenuRequested,
            // this, &ComboBox::showContextMenu);

    // connect(this, &QPushButton::toggled,
            // this, &ComboBox::onToggled);

    addItem("Default",-1);
}

QSize ComboBox::sizeHint() const
{
    return QSize(32, m_height);
}

QSize ComboBox::minimumSizeHint() const
{
    return sizeHint();
}

void ComboBox::applySkin()
{
    // DE_DEBUG("")
    const auto& skin = App::instance()->currentSkin();
    m_height = (m_baseHeight * skin.zoom) / 100;

    updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}


/*
void ComboBox::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/
