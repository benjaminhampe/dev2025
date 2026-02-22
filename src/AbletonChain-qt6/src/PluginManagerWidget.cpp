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
    int ml = contentsMargins().left();
    int mt = contentsMargins().top();
    //int mr = contentsMargins().right();
    int mb = contentsMargins().bottom();

    int x = ml;
    int y = mt;

    for (int i = 0; i < m_widgets.size(); ++i)
    {
        auto pWidget = m_widgets[ i ];

        m_rcDropIndicator = QRect(x, y, m_dropWidth, height() - mt - mb);

        if (m_dropIndex == i)
        {
            x += m_dropWidth;
        }

        pWidget->move(x,y);

        pWidget->setPluginPos(x,y);

        x += pWidget->width() + m_spacing;
    }

    /*
    if (m_dropIndex == m_widgets.size())
    {
        x += m_dropWidth;
    }

    setMinimumWidth(x + 20);
    */
    setMinimumWidth(x);

    update();
}

// ------------------------------------------------------------
// Zeichnen
// ------------------------------------------------------------
void PluginManagerWidget::paintEvent(QPaintEvent* e)
{
    QPainter dc(this);
    dc.fillRect(rect(),QColor(95,95,105));

    auto s = QString("m_dragIndex{%1}, m_dragRect{%2}, m_dropIndex{%3}")
        .arg(m_dragIndex)
        .arg(qstr(m_dragRect))
        .arg(m_dropIndex);

    auto fm = QFontMetrics(font());
    int w = width();
    int h = height();
    int x = 10;
    int y = h - 10 - 1 - fm.ascent();
    dc.setPen(QPen(Qt::white));
    dc.drawText( x, y, s);

    if (m_dropIndex > -1)
    {
        //int x = computeDropX(m_dropIndex);
        dc.fillRect(m_rcDropIndicator, QColor(0, 0, 255, 120));
    }

    e->accept();


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
    int x = contentsMargins().left();

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
    QPoint pos = e->position().toPoint();
    m_lastDragPos = pos;

    m_dropIndex = computeDropIndex(pos);
    startAutoScrollIfNeeded(pos);

    updateLayout();
}

void PluginManagerWidget::dragLeaveEvent(QDragLeaveEvent*)
{
    m_dropIndex = -1;
    m_scrollTimer->stop();
    updateLayout();
}

void PluginManagerWidget::dropEvent(QDropEvent* e)
{
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

    m_dropIndex = -1;
    // repositionWidgets();
    // update();
    updateLayout();
}



// ------------------------------------------------------------
// Positionierung
// ------------------------------------------------------------
/*
void PluginManagerWidget::repositionWidgets()
{
    int x = 10;

    for (int i = 0; i < m_widgets.size(); ++i)
    {
        PluginWidget* w = m_widgets[ i ];

        if (m_dropIndex == i)
        {
            x += m_dropWidth;
        }
        w->move(x, 10);
        x += w->width() + 10;
    }

    if (m_dropIndex == m_widgets.size())
    {
        x += m_dropWidth;
    }

    setMinimumWidth(x + 20);
}
*/

int PluginManagerWidget::computeDropX(int index)
{
    if (index == 0)
    {
        return 10;
    }

    if (index >= m_widgets.size())
    {
        PluginWidget *last = m_widgets.back();
        return last->x() + last->width() + 10;
    }

    return m_widgets[index]->x();
}

// ------------------------------------------------------------
// Auto-Scroll
// ------------------------------------------------------------
void PluginManagerWidget::startAutoScrollIfNeeded(const QPoint &pos)
{
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
}

void PluginManagerWidget::autoScroll()
{
    auto scrollArea = qobject_cast<QScrollArea*>(parentWidget()->parentWidget());
    if (!scrollArea)
    {
        return;
    }

    QScrollBar* bar = scrollArea->horizontalScrollBar();
    bar->setValue(bar->value() + m_scrollDirection * 10);

    m_dropIndex = computeDropIndex(m_lastDragPos);
    updateLayout();
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

void PluginManagerWidget::mousePressEvent(QMouseEvent* e)
{
    if (m_dragIndex < 0)
    {
        for (auto& w : m_widgets)
        {
            w->setIsDragging(false);
        }

        int i = computeWidgetIndex(e->pos());
        if (i > -1)
        {
            m_dragIndex = i;
            m_dragRect = m_widgets[i]->rect();
            m_widgets[i]->setIsDragging(true);
            updateLayout();
        }
    }
}

void PluginManagerWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (m_dragIndex > -1)
    {
        m_dragIndex = -1;

        for (auto& w : m_widgets)
        {
            w->setIsDragging(false);
        }

        updateLayout();
    }
}

void PluginManagerWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (m_dragIndex > -1)
    {
        updateLayout();
    }
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
