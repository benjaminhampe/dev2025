#include "PluginManagerWidget.h"
#include <iostream>

// ------------------------------------------------------------
// Utils
// ------------------------------------------------------------
namespace {

    inline bool isMouseOver(const QPoint &pos, const QRect &r)
    {
        const int m = pos.x();
        const int n = pos.y();
        const int x1 = r.x();
        const int y1 = r.y();
        const int x2 = r.x() + r.width() - 1;
        const int y2 = r.y() + r.height() - 1;
        return (m >= x1) && (m <= x2)
            && (n >= y1) && (n <= y2);
    }

    inline QString qstr(const QRect &r)
    {
        const int x = r.x();
        const int y = r.y();
        const int w = r.width();
        const int h = r.height();
        return QString("%1,%2,%3,%4").arg(x).arg(y).arg(w).arg(h);
    }
} // end namespace.

// ------------------------------------------------------------
// PluginManagerWidget
// ------------------------------------------------------------
PluginManagerWidget::PluginManagerWidget(QWidget *parent)
    : QWidget(parent)
{
    setContentsMargins(0,0,0,0);

    setMouseTracking(true);

    setAcceptDrops(true);

    m_scrollTimer = new QTimer(this);

    connect(m_scrollTimer, &QTimer::timeout,
            this, &PluginManagerWidget::autoScroll);
}

PluginManagerWidget::~PluginManagerWidget()
{

}

// ------------------------------------------------------------
// Layout
// ------------------------------------------------------------

void PluginManagerWidget::updateLayout()
{
    const int n = static_cast<int>(m_widgets.size());
    const int ml = contentsMargins().left();
    const int mt = contentsMargins().top();

    int x = ml;
    int y = mt;

    for (int i = 0; i < n; i++)
    {
        auto pWidget = m_widgets[ i ];

        if (m_dropIndex == i)
        {
            x += m_dropIndicatorWidth + m_widgetSpacing;
        }

        pWidget->move(x,y);

        pWidget->setPluginPos(x,y);

        x += pWidget->width() + m_widgetSpacing;
    }

    if (m_dropIndex >= n)
    {
        x += m_dropIndicatorWidth + m_widgetSpacing;
    }

    // setMinimumWidth(x + 20);

    setMinimumWidth(x);

    update();
}

// ------------------------------------------------------------
// Zeichnen
// ------------------------------------------------------------
void PluginManagerWidget::paintEvent(QPaintEvent* e)
{
    // const int ml = contentsMargins().left();
    const int mt = contentsMargins().top();
    // const int mr = contentsMargins().right();
    const int mb = contentsMargins().bottom();

    QPainter dc(this);
    dc.fillRect(rect(),QColor(145,95,25));

    auto s = QString("m_dropIndex(%1), m_dropIndicatorRect(%2)")
         // .arg(m_dragIndex)
        .arg(m_dropIndex)
        .arg(qstr(m_dropIndicatorRect))
    ;

    auto fm = QFontMetrics(font());
    //int w = width();
    int h = height();
    int x = 10;
    int y = h - 10 - 1 - fm.ascent();
    dc.setPen(QPen(Qt::white));
    dc.drawText( x, y, s);

    if (m_dropIndex > -1)
    {
        int x = computeDropX(m_dropIndex);
        m_dropIndicatorRect = QRect(x, 0, m_dropIndicatorWidth, height() - mt - mb);
        dc.fillRect(m_dropIndicatorRect, QColor(0, 0, 255, 120));
    }

    // e->accept();


}
// ------------------------------------------------------------
// Plugin hinzufügen
// ------------------------------------------------------------
void PluginManagerWidget::addPlugin(const QString &name)
{
    insertPlugin(m_widgets.size(), name);
}

void PluginManagerWidget::insertPlugin(int index, const QString &name)
{
    auto w = new PluginWidget(name, this);
    w->setPluginIndex(index);
    connect( w, &PluginWidget::requestRemoval,
             this, &PluginManagerWidget::removePlugin );

    m_widgets.insert(m_widgets.begin()+index, w);
    w->show();
    updateLayout();
}

void PluginManagerWidget::removePlugin(PluginWidget* w)
{
    auto it = std::find(m_widgets.begin(), m_widgets.end(), w);
    if (it != m_widgets.end())
    {
        int index = 1 + std::distance(m_widgets.begin(), it);
        int count = m_widgets.size();
        std::cout << "Delete plugin " << index << " of " << count << std::endl;

        // found it
        m_widgets.erase(it);
    }

    // Update indices...
    for (int i = 0; i < m_widgets.size(); ++i)
    {
        m_widgets[i]->setPluginIndex(i);
    }

    //m_widgets.removeOne(w);
    w->deleteLater();
    updateLayout();
}



// ------------------------------------------------------------
// Drag&Drop
// ------------------------------------------------------------
void PluginManagerWidget::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();

        auto u = e->mimeData()->urls();
        qDebug() << "dragEnterEvent(" << u.size() << ")";
    }
}


int PluginManagerWidget::computeDropIndex(const QPoint &pos)
{
    for (int i = 0; i < m_widgets.size(); ++i)
    {
        auto w = m_widgets[ i ];

        if (pos.x() < w->x() + w->width() / 2)
        {
            return i;
        }
    }

    return m_widgets.size();
}

void PluginManagerWidget::dragMoveEvent(QDragMoveEvent* e)
{
    // qDebug() << "dragMoveEvent()";

    QPoint pos = e->position().toPoint();
    m_lastDragPos = pos;

    m_dropIndex = computeDropIndex(pos);
    startAutoScrollIfNeeded(pos);

    updateLayout();
}

void PluginManagerWidget::dragLeaveEvent(QDragLeaveEvent*)
{
    qDebug() << "dragLeaveEvent()";
    m_dragIndex = -1;
    m_dropIndex = -1;
    m_scrollTimer->stop();
    updateLayout();
}

void PluginManagerWidget::dropEvent(QDropEvent* e)
{
    qDebug() << "dropEvent()";

    m_scrollTimer->stop();

    if (!e->mimeData()->hasUrls())
    {
        qDebug() << "No urls.";
        return;
    }

    const auto& liste = e->mimeData()->urls();
    for (const auto& url : liste)
    {
        QFileInfo fileInfo(url.toLocalFile());
        if (fileInfo.exists())
        {
            insertPlugin(m_dropIndex, fileInfo.fileName());
        }
    }

    m_dragIndex = -1;
    m_dropIndex = -1;
    updateLayout();
}

// ------------------------------------------------------------
// Positionierung
// ------------------------------------------------------------

int PluginManagerWidget::computeDropX(int index)
{
    const int n = static_cast<int>(m_widgets.size());

    if (index <= 0 || n < 1)
    {
        return contentsMargins().left();
    }

    if (index >= n)
    {
        if (n > 0)
        {
            auto p = m_widgets.back();
            return p->x() + p->width() + m_widgetSpacing;
        }
        else
        {
            return contentsMargins().left();
        }
    }

    return m_widgets[index]->x() - m_widgetSpacing - m_dropIndicatorWidth;
}

// ------------------------------------------------------------
// Auto-Scroll
// ------------------------------------------------------------
void PluginManagerWidget::startAutoScrollIfNeeded(const QPoint &pos)
{
/*
    int margin = 40;

    if (pos.x() < margin)
    {
        m_scrollDirection = -1;
        if (!m_scrollTimer->isActive())
        {
            m_scrollTimer->start(30);
        }
    }
    else if (pos.x() > width() - margin)
    {
        m_scrollDirection = +1;
        if (!m_scrollTimer->isActive())
        {
            m_scrollTimer->start(30);
        }
    }
    else
    {
        m_scrollTimer->stop();
    }
*/
}

void PluginManagerWidget::autoScroll()
{
/*
    auto scrollArea = qobject_cast<QScrollArea*>(parentWidget()->parentWidget());
    if (!scrollArea)
    {
        return;
    }

    QScrollBar* bar = scrollArea->horizontalScrollBar();
    bar->setValue(bar->value() + m_scrollDirection * 10);

    m_dropIndex = computeDropIndex(m_lastDragPos);
    updateLayout();
*/
}

// ------------------------------------------------------------
// Drag&Drop Reorder:
// ------------------------------------------------------------

int PluginManagerWidget::computeWidgetIndex(const QPoint &pos)
{
    for (int i = 0; i < m_widgets.size(); ++i)
    {
        auto w = m_widgets[ i ];

        auto r_childInParent = QRect( w->mapTo(this, QPoint(0,0)), w->size() );

        if ( isMouseOver(pos, r_childInParent) )
        {
            return i;
        }
    }

    return -1;
}

void PluginManagerWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_isLeftPressed = false;

        if (m_isDragging)
        {
            swapWidgets( m_dragIndex, m_dropIndex );

            for (auto p : m_widgets)
            {
                p->setIsDragging(false);
            }
        }

        m_isDragging = false;
        m_dropIndex = -1;
        m_dragIndex = -1;
        updateLayout();
    }

}

void PluginManagerWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_isLeftPressed = true;
        m_isDragging = false;
        m_dragIndex = -1;
        m_dropIndex = -1;
        m_lastDragPos = e->position().toPoint();

        for (auto p : m_widgets)
        {
            p->setIsDragging(false);
        }

        updateLayout();
    }
}

void PluginManagerWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (m_isLeftPressed)
    {
        auto v = m_lastDragPos - e->position().toPoint();
        auto d = v.x() * v.x() + v.y() * v.y();
        // Init drag only when mouse moved atleast 10px (=100 squared)
        if (d >= 100)
        {
            if (!m_isDragging)
            {
                m_dropIndex = -1;
                m_dragIndex = computeWidgetIndex( m_lastDragPos );

                if (m_dragIndex > -1 && m_dragIndex < int(m_widgets.size()))
                {
                    m_widgets[ m_dragIndex ]->setIsDragging(true);
                }

                m_isDragging = true;
            }
            else
            {
                m_dropIndex = computeDropIndex(e->position().toPoint());

                updateLayout();
            }
        }
    }
}

void PluginManagerWidget::swapWidgets(int drag, int drop)
{
    if (drag == drop)
    {
        return;
    }

    const int n = static_cast<int>(m_widgets.size());

    if (n < 2)
    {
        return;
    }

    if (drag < 0 || drag >= n)
    {
        qDebug() << "Invalid drag index " << drag << " of " << n;
        return;
    }

    if (drop < 0 || drop >= n)
    {
        qDebug() << "Invalid drop index " << drop << " of " << n;
        return;
    }

    qDebug() << "Swap index " << (drag+1) << " <-> "  << (drop+1) << " of " << n;

    std::swap( m_widgets[ drag ], m_widgets[ drop ] );

    emit reorderedWidgets();
}


// ------------------------------------------------------------
// Speicherung / Laden
// ------------------------------------------------------------
void PluginManagerWidget::saveState(const QString &path)
{
    QJsonArray arr;
    for (auto *w : m_widgets)
    {
        arr.append(w->fileName());
    }

    QFile f(path);
    if (f.open(QIODevice::WriteOnly))
    {
        f.write(QJsonDocument(arr).toJson());
    }
}

void PluginManagerWidget::loadState(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
    {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    for (const auto& v : doc.array())
    {
        addPlugin(v.toString());
    }
    updateLayout();
}
