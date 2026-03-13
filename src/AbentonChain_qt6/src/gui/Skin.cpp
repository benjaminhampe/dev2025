#include "Track.h"

namespace de {
namespace gui {
namespace track {
	
// ------------------------------------------------------------
// Utils
// ------------------------------------------------------------
namespace
{
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
Shell::Shell(const QString &fileName, QWidget* parent)
// ------------------------------------------------------------
    : QWidget(parent)
    , m_strFileName(fileName)
    , m_isDragging(false)
{
    setFixedSize(300, 200);

    m_skin.fillColor = QColor(25,25,25);

    // m_bg = QColor(
    //     QRandomGenerator::global()->bounded(256),
    //     QRandomGenerator::global()->bounded(256),
    //     QRandomGenerator::global()->bounded(256)
    // );

    // int brightness = (m_bg.red()*299 + m_bg.green()*587 + m_bg.blue()*114) / 1000;
    // m_fg = (brightness < 128) ? Qt::white : Qt::black;

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested,
            this, &Shell::showContextMenu);
}

QString
Shell::fileName() const
{
    return m_strFileName;
}

void
Shell::paintEvent(QPaintEvent *)
{
    QPainter dc(this);

    // [Draw] Panel
    dc.fillRect(rect(), m_skin.fillColor);

    // [Draw] RoundRect
    dc.setPen(m_skin.bodyColor);
    dc.setBrush(m_skin.bodyColor);
    dc.drawRoundedRect(rect(), 16,16);

    // [Draw] Text
    dc.setPen(m_skin.textColor);
    dc.setFont(QFont("Arial", 14, QFont::Bold));
    auto r_body = QRect(rect().x() + 10,
                        rect().y() + 26,
                        rect().width() - 20,
                        rect().height() - 36);

    // [Draw] Text1
    auto s = QString("%1, Pos(%2,%3)")
            .arg(m_strIndex)
            .arg(m_pos.x())
            .arg(m_pos.y());

    dc.drawText(r_body, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap, s);

    // [Draw] Text2
    dc.drawText(rect(), Qt::AlignCenter | Qt::TextWordWrap, m_strFileName);

    // [Draw] Text3
    dc.drawText(rect(), Qt::AlignBottom | Qt::AlignHCenter | Qt::TextWordWrap,
                QString("rect{%1,%2,%3,%4}")
                .arg(rect().x())
                .arg(rect().y())
                .arg(rect().width())
                .arg(rect().height()) );

    if (m_isDragging)
    {
        dc.setBrush(Qt::NoBrush);
        dc.setPen(QPen(QColor(255,255,255,200), 5.0));
        dc.drawRect(rect());
    }
}

void
Shell::setIsDragging(bool isDragging)
{
    if (m_isDragging != isDragging)
    {
        m_isDragging = isDragging;
        update();
    }
}

void
Shell::setPluginPos(int x, int y)
{
    m_pos = QPoint(x,y);
    update();
}

void
Shell::setPluginIndex(int i)
{
    m_strIndex = QString::number(i);
    update();
}


void
Shell::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}

// ------------------------------------------------------------
// Body
// ------------------------------------------------------------
Track::Track(QWidget *parent)
    : QWidget(parent)
{
    setContentsMargins(0,0,0,0);

    setMouseTracking(true);

    setAcceptDrops(true);

    // m_scrollTimer = new QTimer(this);

    // connect(m_scrollTimer, &QTimer::timeout, this, &Track::autoScroll);
}

Track::~Track()
{

}

// ------------------------------------------------------------
// Layout
// ------------------------------------------------------------

void Track::updateLayout()
{
    const int n = static_cast<int>(m_widgets.size());
    const int ml = contentsMargins().left();
    const int mt = contentsMargins().top();

    int x = ml;
    int y = mt;

    const int widgetSpacing = (m_skin.zoom *
                               m_skin.spacing) / 100;
    const int dropIndicatorWidth = (m_skin.zoom *
                                    m_skin.dropIndicatorWidth) / 100;

    for (int i = 0; i < n; i++)
    {
        auto pWidget = m_widgets[ i ];

        if (m_dropIndex == i)
        {
            x += dropIndicatorWidth + widgetSpacing;
        }

        pWidget->move(x,y);

        pWidget->setPluginPos(x,y);

        x += pWidget->width() + widgetSpacing;
    }

    if (m_dropIndex >= n)
    {
        x += dropIndicatorWidth + widgetSpacing;
    }

    // setMinimumWidth(x + 20);

    setMinimumWidth(x);

    update();
}

// ------------------------------------------------------------
// Zeichnen
// ------------------------------------------------------------
void Track::paintEvent(QPaintEvent* e)
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

        // const int widgetSpacing = (m_skin.zoom *
        //                            m_skin.trkChnWidgetSpacing) / 100;
        const int dropIndicatorWidth = (m_skin.zoom *
                                        m_skin.dropIndicatorWidth) / 100;


        m_dropIndicatorRect = QRect(x, 0, dropIndicatorWidth, height() - mt - mb);
        dc.fillRect(m_dropIndicatorRect, QColor(0, 0, 255, 120));
    }

    // e->accept();


}
// ------------------------------------------------------------
// Plugin hinzufügen
// ------------------------------------------------------------
void Track::addPlugin(const QString &name)
{
    insertPlugin(m_widgets.size(), name);
}

void Track::insertPlugin(int index, const QString &name)
{
    auto w = new Shell(name, this);
    w->setPluginIndex(index);
    connect( w, &Shell::requestRemoval,
             this, &Track::removePlugin );

    m_widgets.insert(m_widgets.begin()+index, w);
    w->show();
    updateLayout();
}

void Track::removePlugin(Shell* w)
{
    auto it = std::find(m_widgets.begin(), m_widgets.end(), w);
    if (it != m_widgets.end())
    {
        // int index = 1 + std::distance(m_widgets.begin(), it);
        // int count = m_widgets.size();
        // std::cout << "Delete plugin " << index << " of " << count << std::endl;

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
void Track::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();

        // auto u = e->mimeData()->urls();
        // qDebug() << "dragEnterEvent(" << u.size() << ")";
    }
}


int Track::computeDropIndex(const QPoint &pos)
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

void Track::dragMoveEvent(QDragMoveEvent* e)
{
    // qDebug() << "dragMoveEvent()";

    QPoint pos = e->position().toPoint();
    m_lastDragPos = pos;

    m_dropIndex = computeDropIndex(pos);
    startAutoScrollIfNeeded(pos);

    updateLayout();
}

void Track::dragLeaveEvent(QDragLeaveEvent*)
{
    // qDebug() << "dragLeaveEvent()";
    m_dragIndex = -1;
    m_dropIndex = -1;
    // m_scrollTimer->stop();
    updateLayout();
}

void Track::dropEvent(QDropEvent* e)
{
    // qDebug() << "dropEvent()";

    // m_scrollTimer->stop();

    if (!e->mimeData()->hasUrls())
    {
        // qDebug() << "No urls.";
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

int Track::computeDropX(int index)
{
    const int n = static_cast<int>(m_widgets.size());

    if (index <= 0 || n < 1)
    {
        return contentsMargins().left();
    }

    const int widgetSpacing = (m_skin.zoom *
                               m_skin.spacing) / 100;
    const int dropIndicatorWidth = (m_skin.zoom *
                                    m_skin.dropIndicatorWidth) / 100;

    if (index >= n)
    {
        if (n > 0)
        {
            auto p = m_widgets.back();
            return p->x() + p->width() + widgetSpacing;
        }
        else
        {
            return contentsMargins().left();
        }
    }

    return m_widgets[index]->x() - widgetSpacing - dropIndicatorWidth;
}

// ------------------------------------------------------------
// Auto-Scroll
// ------------------------------------------------------------
void Track::startAutoScrollIfNeeded(const QPoint &pos)
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

void Track::autoScroll()
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

int Track::computeWidgetIndex(const QPoint &pos)
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

void Track::mouseReleaseEvent(QMouseEvent* e)
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

void Track::mousePressEvent(QMouseEvent* e)
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

void Track::mouseMoveEvent(QMouseEvent* e)
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

void Track::swapWidgets(int drag, int drop)
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
        // qDebug() << "Invalid drag index " << drag << " of " << n;
        return;
    }

    if (drop < 0 || drop >= n)
    {
        // qDebug() << "Invalid drop index " << drop << " of " << n;
        return;
    }

    // qDebug() << "Swap index " << (drag+1) << " <-> "  << (drop+1) << " of " << n;

    std::swap( m_widgets[ drag ], m_widgets[ drop ] );

    emit reorderedWidgets();
}


// ------------------------------------------------------------
// Speicherung / Laden
// ------------------------------------------------------------
void Track::saveState(const QString &path)
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

void Track::loadState(const QString &path)
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

/*
// ============================================================
Track::Track(QWidget *parent)
// ============================================================
    : QWidget(parent)
{
    setContentsMargins(0,0,0,0);

    m_Track = new Track;

    m_scrollArea = new QScrollArea;
    m_scrollArea -> setContentsMargins(0,0,0,0);
    m_scrollArea -> setWidgetResizable(true);
    m_scrollArea -> setWidget( m_Track );
    // m_scrollArea -> setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    auto v = new QVBoxLayout();
    v -> setContentsMargins(0,0,0,0);
    v -> addWidget(m_scrollArea, 1);
    setLayout(v);
}

// ============================================================
Track::~Track()
// ============================================================
{

}
*/

} // end namespace track
} // end namespace gui
} // end namespace de
