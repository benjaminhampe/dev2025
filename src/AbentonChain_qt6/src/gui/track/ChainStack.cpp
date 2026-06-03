#include "gui/track/ChainStack.h"
#include "App.h"
#include "gui/Skin.h"

ChainStack::ChainStack(QWidget* parent)
    : QWidget(parent)
{
    DE_TRACE("")

    setMinimumWidth(100);
    setContentsMargins(0,0,0,0);

    // QStackedWidget holds multiple pages; only one is visible at a time
    m_stack = new QStackedWidget;
    m_stack->setStyleSheet("background: transparent;");

    // // External scrollbar that will control the visible child widget
    m_scrollBar = new QScrollBar(Qt::Horizontal);

    // Layout: stacked widget on the left, scrollbar on the right
    auto v = new QVBoxLayout;
    v->setContentsMargins(0,0,0,0);
    v->setSpacing(1);
    v->addWidget(m_stack);
    v->addWidget(m_scrollBar);
    v->addStretch(1);

    setLayout(v);

    // When the scrollbar moves, reposition the visible child widget
    connect(m_scrollBar, &QScrollBar::valueChanged,
            this, &ChainStack::onUpdateScrollPosition);

    // When the visible page changes, recalculate scroll range
    connect(m_stack, &QStackedWidget::currentChanged,
            this, &ChainStack::onUpdateScrollRange);

    addPage( new ChainWrapper(this) );

    m_scrollBar->setVisible( false );

    applySkin();
}

ChainStack::~ChainStack()
{
    DE_TRACE("")
}

void ChainStack::applySkin()
{
    if (m_stack->currentWidget())
    {
        static_cast<ChainWrapper*>(m_stack->currentWidget())->applySkin();
    }

    const auto& skin = App::instance()->getSkin();
    m_windowColor = skin.windowColor;
    // int w = (m_baseWidth * skin.zoom) / 100;
    // int h = (m_baseHeight * skin.zoom) / 100;
    // setFixedSize(w,h);

    update();
}

void ChainStack::addPage(ChainWrapper* wrapper)
{
    if (!wrapper)
    {
        DE_ERROR("No wrapper")
        return;
    }

    // Add page to the stack
    m_stack->addWidget(wrapper);

    // Install event filter so we detect when
    // the child resizes
    // (needed because sizeHint changes dynamically)
    wrapper->installEventFilter(this);

    connect(wrapper->trackWidget(), &Track::newOverview,
        this, [=] (QPixmap pix)
        {
            DE_BENNI("newTrackOverview")
            emit newTrackOverview(pix); // for Footer overview/scrollbar
        });

}

bool ChainStack::eventFilter(QObject *obj, QEvent *event)
{
    // If the child widget changes size, update scroll range
    if (event->type() == QEvent::Resize)
        onUpdateScrollRange();

    return QWidget::eventFilter(obj, event);
}

void ChainStack::paintEvent(QPaintEvent* event)
{
    if (!isVisible())
    {
        return;
    }

    QPainter dc(this);
    dc.fillRect(rect(), m_windowColor);

    // dc.setBrush(Qt::NoBrush);
    // dc.setPen(QPen(QColor(255,0,0)));
    // dc.drawRect(rect().adjusted(1,1,-1,-1));
}

void ChainStack::onUpdateScrollRange()
{
    // Get currently visible page
    QWidget *page = m_stack->currentWidget();
    if (!page)
    {
        DE_ERROR("No currentWidget")
        return;
    }

    // Height of the child widget (content)
    int childWidth = page->width();

    // Height of the visible area (the stacked widget)
    int viewportWidth = m_stack->width();

    // Scroll range = how much content exceeds the visible area
    int range = qMax(0, childWidth - viewportWidth);

    // DE_WARN("childWidth = ",childWidth)
    // DE_WARN("viewportWidth = ",viewportWidth)
    // DE_WARN("range = ",range)

    // Set scrollbar range dynamically
    m_scrollBar->setRange(0, range);

    // Page step = how much scrolling happens per wheel/page action
    m_scrollBar->setPageStep(viewportWidth);
}

void ChainStack::onUpdateScrollPosition(int value)
{
    // Move the child widget upward by the scrollbar value
    // Negative Y moves content up, revealing lower parts
    QWidget* page = m_stack->currentWidget();
    if (!page) return;

    page->move(-value, 0);
}


/*
void ChainStack::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/


