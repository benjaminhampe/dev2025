#include "Track.h"
#include "App.h"

// ==================================================
Track::Track(de::audio::ITrack* track, QWidget* parent)
// ==================================================
    : QWidget(parent)
    , m_track(track)
{
    DE_TRACE("")
    //setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setContentsMargins(0,0,0,0);
    setMouseTracking(true);
    setAcceptDrops(true);
    setStyleSheet("background: transparent;");

    // m_scrollTimer = new QTimer(this);
    // connect(App::instance(), &App::skinChanged, this, &Track::applySkin);

    // connect(m_scrollTimer, &QTimer::timeout, this, &Track::autoScroll);

    // m_dropTarget = new DropTarget(this);

    applySkin();
}

Track::~Track()
{
    DE_TRACE("")
    for (auto p : m_plugins)
    {
        delete p;
    }
    m_plugins.clear();
}

// QSize Track::sizeHint() const { return QSize(0, m_height); }
// QSize Track::minimumSizeHint() const { return sizeHint(); }

void Track::applySkin()
{
    for (auto p : m_plugins)
    {
        p->applySkin();
    }

    //qDebug() << "Track::applySkin()";
    const auto& skin = App::instance()->currentSkin();
    m_windowColor = skin.windowColor;
    m_panelColor = skin.panelColor;
    m_textColor = skin.textColor;
    m_height = (m_baseHeight * skin.zoom) / 100;
    m_radius = (m_baseRadius * skin.zoom) / 100;

    m_widgetSpacing = (m_baseWidgetSpacing * skin.zoom) / 100;
    m_dropIndicatorWidth = (m_baseDropIndicatorWidth * skin.zoom) / 100;
    m_dropTargetWidth = (m_baseDropTargetWidth * skin.zoom) / 100;

    updateLayout();
}

void Track::updateLayout()
{
    const int n = static_cast<int>(m_plugins.size());

    int x = 0;
    int y = 0;

    for (int i = 0; i < n; i++)
    {
        auto pWidget = m_plugins[ i ];

        if (m_dropIndex == i)
        {
            x += m_dropIndicatorWidth + m_widgetSpacing;
        }

        pWidget->move(x,y);

        x += pWidget->width() + m_widgetSpacing;
    }

    if (m_dropIndex >= n)
    {
        x += m_dropIndicatorWidth + m_widgetSpacing;
    }

    int remain = parentWidget()->width() - x;
    if (remain < m_dropTargetWidth)
    {
        remain = m_dropTargetWidth;
    }

    x += remain;

    m_width = x;

    m_rcDropTarget = QRect(m_width - remain, 0,
                           remain, m_height);

    // DE_DEBUG("m_rcDropTarget = ",qstr(m_rcDropTarget).toStdString())

    setFixedSize(m_width, m_height);

    // updateGeometry();
    update();
}

void drawDropTarget(QPainter & dc, QRect pos, int radius,
    QColor panelColor, QColor textColor,
    QString msg, QFont font)
{
    // [Draw] panel
    dc.setRenderHint( QPainter::Antialiasing );
    dc.setPen( Qt::NoPen );
    dc.setBrush( QBrush( panelColor ) );
    dc.drawRoundedRect( pos, radius, radius );

    // [Draw] text
    dc.setPen(QPen(textColor));
    dc.setBrush(Qt::NoBrush);
    QRect r_text = QRect( pos.x() + radius,
                          pos.y() + radius,
                          pos.width()-2*radius,
                          pos.height()-2*radius );
    dc.drawText(r_text,
                Qt::TextWordWrap | Qt::AlignCenter,
                msg,
                &r_text );

}

// ------------------------------------------------------------
// Zeichnen
// ------------------------------------------------------------
void Track::resizeEvent(QResizeEvent* e)
{
    updateLayout();

    QWidget::resizeEvent(e);
}

void Track::paintEvent(QPaintEvent* e)
{
    if (!isVisible())
    {
        return;
    }

    QPainter dc(this);

    drawDropTarget(dc,
        m_rcDropTarget, m_radius,
        m_panelColor, m_textColor,
        m_isAudioOnly ? m_msg1 : m_msg2,
        font());

    auto s = QString("m_dropIndex(%1), "
                     "m_dropIndicatorRect(%2)")
        .arg(m_dropIndex)
        .arg(qstr(m_rcDropIndicator));

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

        m_rcDropIndicator = QRect(x, 0, m_dropIndicatorWidth, height());
        dc.fillRect(m_rcDropIndicator, QColor(0, 0, 255, 120));
    }

    // e->accept();
}

// ------------------------------------------------------------
// Plugin hinzufügen
// ------------------------------------------------------------
void Track::addPlugin(const QString &uri)
{
    insertPlugin(m_plugins.size(), uri);
}

void Track::insertPlugin(int index, const QString &uri)
{
    qDebug() << "Dropped file:" << uri;

    if (!m_track)
    {
        DE_ERROR("No track")
        return;
    }

    auto plugin = m_track->createPlugin(uri.toStdString(), index);
    if (!plugin)
    {
        DE_ERROR("No plugin")
        return;
    }

    // Create GUI Shell
    auto w = new Plugin(plugin, this);
    w->show();

    // Connect GUI Shell
    connect(w, &Plugin::requestRemoval, this, &Track::removePlugin);

    // Manage GUI Shell
    m_plugins.insert(m_plugins.begin() + index, w);

    updateLayout();
}

void Track::removePlugin(Plugin* w)
{
    if (!w)
    {
        DE_ERROR("Got nullptr")
        return;
    }

    auto it = std::find(m_plugins.begin(), m_plugins.end(), w);
    if (it != m_plugins.end())
    {
        // int index = 1 + std::distance(m_plugins.begin(), it);
        // int count = m_plugins.size();
        // std::cout << "Delete plugin " << index << " of " << count << std::endl;

        // found it
        m_plugins.erase(it);
        //m_layout->removeWidget(w);
    }

    if (m_track)
    {
        m_track->removePlugin( w->getPlugin() );
    }
    else
    {
        DE_ERROR("No audio track")
    }

    // Update indices...
    // for (int i = 0; i < m_plugins.size(); ++i)
    // {
    //     m_plugins[i]->setPluginIndex(i);
    // }

    //m_plugins.removeOne(w);
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
    for (int i = 0; i < m_plugins.size(); ++i)
    {
        auto w = m_plugins[ i ];

        if (pos.x() < w->x() + w->width() / 2)
        {
            return i;
        }
    }

    return m_plugins.size();
}

// ------------------------------------------------------------
// Drag&Drop Reorder:
// ------------------------------------------------------------

int Track::computeWidgetIndex(const QPoint &pos)
{
    for (int i = 0; i < m_plugins.size(); ++i)
    {
        auto w = m_plugins[ i ];

        auto r_childInParent = QRect( w->mapTo(this, QPoint(0,0)), w->size() );

        if ( isMouseOver(pos, r_childInParent) )
        {
            return i;
        }
    }

    return -1;
}


void Track::dragMoveEvent(QDragMoveEvent* e)
{
    // qDebug() << "dragMoveEvent()";
    QPoint pos = e->position().toPoint();
    m_posDragInit = pos;
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
    for (const auto& item : liste)
    {
        QString url = item.toLocalFile();
        if (QFileInfo::exists(url))
        {
            insertPlugin(m_dropIndex, url);
        }
        else
        {
            DE_ERROR("File not exist: ", url.toStdString())
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
    const int n = static_cast<int>(m_plugins.size());

    if (index <= 0 || n < 1)
    {
        return contentsMargins().left();
    }

    if (index >= n)
    {
        if (n > 0)
        {
            auto p = m_plugins.back();
            return p->x() + p->width() + m_widgetSpacing;
        }
        else
        {
            return contentsMargins().left();
        }
    }

    return m_plugins[index]->x() - m_widgetSpacing - m_dropIndicatorWidth;
}



void Track::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_isLeftPressed = true;
        m_isDragInit = false;
        m_isDragging = false;
        m_dragIndex = -1;
        m_dropIndex = -1;
        m_posDragInit = QPoint();

        auto pos = e->position().toPoint();
        m_posMouse = pos;

        // If mouse is over Plugin->m_rcLabel
        // then start a drag operation...
        int index = computeWidgetIndex( pos );
        if (index >= 0 && index < int(m_plugins.size()))
        {
            auto w = m_plugins[index];
            if (w && w->labelRect().contains(pos))
            {
                m_isDragInit = true;
                m_posDragInit = pos;
                m_dragIndex = index;
            }
        }

        // for (auto p : m_plugins)
        // {
        //     p->setIsDragging(false);
        // }

        updateLayout();
    }
}

void Track::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_isLeftPressed = false;

        if (m_isDragging)
        {
            swapWidgets( m_dragIndex, m_dropIndex );

            // for (auto p : m_plugins)
            // {
            //     p->setIsDragging(false);
            // }
        }

        m_isDragInit = false;
        m_isDragging = false;
        m_dropIndex = -1;
        m_dragIndex = -1;
        updateLayout();
    }

}


void Track::mouseMoveEvent(QMouseEvent* e)
{
    m_posMouse = e->position().toPoint();
    if (m_isDragInit)
    {
        auto v = m_posDragInit - m_posMouse;
        auto d = v.x() * v.x() + v.y() * v.y();
        // Init drag only when mouse moved atleast 10px (=100 squared)
        if (d >= 100)
        {
            if (!m_isDragging)
            {
                m_dropIndex = -1;
                //m_dragIndex = computeWidgetIndex(m_posDragInit);

                // if (m_dragIndex > -1 && m_dragIndex < int(m_plugins.size()))
                // {
                //     m_plugins[ m_dragIndex ]->setIsDragging(true);
                // }

                m_isDragging = true;
            }
            else
            {
                m_dropIndex = computeDropIndex(m_posMouse);

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

    const int n = static_cast<int>(m_plugins.size());

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

    std::swap( m_plugins[ drag ], m_plugins[ drop ] );

    emit reorderedWidgets();
}


// ------------------------------------------------------------
// Speicherung / Laden
// ------------------------------------------------------------
void Track::saveState(const QString &path)
{
    QJsonArray arr;
    for (Plugin* w : m_plugins)
    {
        auto plugin = w->getPlugin();
        if (!plugin)
        {
            DE_ERROR("No plugin")
            continue;
        }

        arr.append(QString::fromStdString(plugin->uri()));
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
