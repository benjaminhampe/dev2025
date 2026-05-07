#include "gui/track/details/ComboBox.h"
#include "App.h"
#include "gui/Skin.h"

ComboBox::ComboBox(QWidget* parent)
    : QComboBox(parent)
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    // setContextMenuPolicy(Qt::CustomContextMenu);

    setStyleSheet(R"(
        QComboBox {
            background-color: #2b2b2b;
            color: #ffffff;
            border: 1px solid #555;
            padding: 4px 8px;
            min-height: 24px;
            min-width: 80px;
        }

        QComboBox::drop-down {
            width: 20px;
            border-left: 1px solid #555;
            background-color: #3a3a3a;
        }

        /*
        QComboBox::down-arrow {
            image: url(:/icons/arrow_down.svg);
            width: 10px;
            height: 10px;
        }
        */

        QComboBox QAbstractItemView {
            background-color: #1e1e1e;
            color: #ffffff;
            selection-background-color: #3a6ea5;
            selection-color: white;
            border: 1px solid #555;
        }

        /* Vertical scrollbar */
        QComboBox QAbstractItemView QScrollBar:vertical {
            width: 28px;                /* <-- make scrollbar wider */
            background: #2b2b2b;
            margin: 0px;
        }

        QComboBox QAbstractItemView QScrollBar::handle:vertical {
            background: #5a5a5a;
            min-height: 40px;           /* touch-friendly handle */
            border-radius: 6px;
        }

        QComboBox QAbstractItemView QScrollBar::add-line:vertical,
        QComboBox QAbstractItemView QScrollBar::sub-line:vertical {
            height: 0px;                /* remove arrow buttons */
        }

        QComboBox QAbstractItemView QScrollBar::add-page:vertical,
        QComboBox QAbstractItemView QScrollBar::sub-page:vertical {
            background: none;
        }
    )");
/*
    setStyleSheet(R"(
        QComboBox {
            background-color: #2b2b2b;
            color: #ffffff;
            border: 1px solid #555;
            padding: 4px 8px;
            min-height: 24px;
            min-width: 80px;
        }

        QComboBox::drop-down {
            width: 20px;
            border-left: 1px solid #555;
            background-color: #3a3a3a;
        }

        QComboBox::down-arrow {
            image: url(:/icons/arrow_down.svg);
            width: 10px;
            height: 10px;
        }

        QComboBox QAbstractItemView {
            background-color: #1e1e1e;
            color: #ffffff;
            selection-background-color: #3a6ea5;
            selection-color: white;
            border: 1px solid #555;
        }
    )");
*/
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

void ComboBox::showPopup()
{
    QComboBox::showPopup();

    // The popup container (QComboBoxPrivateContainer)
    auto popup = this->findChild<QFrame*>();
    if (!popup)
        return;

    int popupWidth = std::min(400, width() * 4);  // or any width you want

    QRect r = popup->geometry();
    r.setWidth(popupWidth);
    popup->setGeometry(r);
}