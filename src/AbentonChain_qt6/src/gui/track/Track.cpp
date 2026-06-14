#include "Track.h"
#include "App.h"

// ==================================================
Track::Track(de::audio::Track* track, QWidget* parent)
// ==================================================
    : QWidget(parent)
    , m_track(track)
{
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
    for (auto p : m_plugins) { delete p; }
    m_plugins.clear();
}

QSize Track::sizeHint() const { return QSize(0, m_height); }
QSize Track::minimumSizeHint() const { return sizeHint(); }

void Track::applySkin()
{
    for (auto p : m_plugins)
    {
        p->applySkin();
    }

    //qDebug() << "Track::applySkin()";
    const auto& skin = App::instance()->getSkin();
    m_windowColor = skin.windowColor;
    m_panelColor = skin.panelColor;
    m_textColor = skin.textColor;
    m_height = (m_baseHeight * skin.zoom) / 100;
    m_radius = (m_baseRadius * skin.zoom) / 100;
    m_overviewHeight = (48 * skin.zoom) / 100;

    m_widgetSpacing = (m_baseWidgetSpacing * skin.zoom) / 100;
    m_dropIndicatorWidth = (m_baseDropIndicatorWidth * skin.zoom) / 100;
    m_dropTargetWidth = (m_baseDropTargetWidth * skin.zoom) / 100;

    updateLayout();
}

void Track::updateLayout()
{
    //DE_TRACE("updateLayout()")

    const int n = static_cast<int>(m_plugins.size());

    int x = 0;
    int y = 0;

    for (int i = 0; i < n; ++i)
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

    m_width = x;
    m_bEmitOverview = true;
    updateLayoutOfDropTarget();
}

void Track::updateLayoutOfDropTarget()
{
    //DE_TRACE("updateLayoutOfDropTarget()")
    int dropTargetWidth = std::max(width() - m_width, m_dropTargetWidth);
    m_rcDropTarget = QRect(m_width, 0, dropTargetWidth, m_height);
    update();
}

std::vector<de::audio::SharedPlugin>
Track::collectPlugins() const
{
    std::vector<de::audio::SharedPlugin> plugins;
    plugins.reserve( m_plugins.size() );
    for (auto pluginWidget : m_plugins)
    {
        if (pluginWidget)
        {
            de::audio::SharedPlugin audioPlugin = pluginWidget->getPlugin();
            if (audioPlugin)
            {
                plugins.push_back( audioPlugin );
            }
        }
    }

    return plugins;
}

// ------------------------------------------------------------
// Zeichnen
// ------------------------------------------------------------
void Track::resizeEvent(QResizeEvent* e)
{
    //DE_TRACE("resizeEvent(",e->size().width(),",",e->size().height(),")")
    updateLayoutOfDropTarget();
    QWidget::resizeEvent(e);
}

void Track::paintEvent(QPaintEvent* e)
{
    if (m_bInPaintEvent)
    {
        DE_ERROR("m_bInPaintEvent")
        return;
    }

    m_bInPaintEvent = true;

    if (isVisible())
    {
        QPainter dc;
        if (dc.begin(this))
        {
            dc.setRenderHint( QPainter::Antialiasing );

            // [Draw] drawDropTarget.Panel:
            dc.setPen( Qt::NoPen );
            dc.setBrush( QBrush( m_panelColor ) );
            dc.drawRoundedRect( m_rcDropTarget, m_radius, m_radius );

            // [Draw] drawDropTarget.Text:
            dc.setPen( QPen(m_textColor) );
            dc.setBrush(Qt::NoBrush);
            QRect r_text = QRect( m_rcDropTarget.x() + m_radius,
                                  m_rcDropTarget.y() + m_radius,
                                  m_rcDropTarget.width()-2*m_radius,
                                  m_rcDropTarget.height()-2*m_radius );

            dc.drawText(r_text, Qt::TextWordWrap | Qt::AlignCenter,
                        m_msg, &r_text );


            // [Draw] Blue drawDropIndicator stripe:
            int dragIndicatorPosX = computeDropIndicatorPosX(m_dragIndex, m_dropIndex);
            if (dragIndicatorPosX > -1)
            {
                m_rcDropIndicator = QRect(dragIndicatorPosX, 0, m_dropIndicatorWidth, height());
                dc.fillRect(m_rcDropIndicator, QColor(0, 0, 255, 120));
            }

            //<debug>
#if 1
            // [Draw] Drag/Drop indices as white text
            auto s = QString("dragIndex(%1), dropIndex(%2)")
                .arg(m_dragIndex)
                .arg(m_dropIndex)
                //.arg(qstr(m_rcDropIndicator))
            ;

            auto fm = QFontMetrics(font());
            int x = width() - 11 - fm.boundingRect(s).width();
            int y = height() - 11 - fm.ascent();
            dc.setPen(QPen(Qt::white));
            dc.drawText( x, y, s);
#endif
            //</debug>

            dc.end();
        }

        if (m_bEmitOverview)
        {
            m_bEmitOverview = false;
            QMetaObject::invokeMethod(this, "emitTrackOverview", Qt::QueuedConnection);
        }

    }

    m_bInPaintEvent = false;
}


void Track::emitTrackOverview()
{
    if (m_width < 1)
    {
        emit newOverview(QPixmap());
        return;
    }

    // QPainter p(&pm);
    // render(&p, QPoint(), QRegion(), QWidget::DrawChildren);

    // src_w            dst_w                       src_w
    // ----- = aspect = -----  ==>  dst_w = dst_h * -----
    // src_h            dst_h                       src_h

    int src_w = m_width;
    int src_h = height();
    int dst_h = m_overviewHeight > 0 ? m_overviewHeight : 48;
    int dst_w = std::lround((float(src_w) / float(src_h)) * float(dst_h));

    // DE_BENNI("dst(",dst_w,",",dst_h,")")

    QPixmap pixmap(dst_w, dst_h);
    pixmap.fill(Qt::transparent);
    QPainter dc;
    if (dc.begin(&pixmap))
    {
        float scale_x = float(dst_w) / float(src_w);
        float scale_y = float(dst_h) / float(src_h);

        // DE_BENNI("scale(",scale_x,",",scale_y,")")
        dc.scale(scale_x,scale_y);
        dc.setRenderHint(QPainter::Antialiasing, true);
        dc.setRenderHint(QPainter::TextAntialiasing, true);
        render(&dc);
        dc.end();

        emit newOverview(pixmap);
    }
    else
    {
        DE_ERROR("dc inactive")
    }

    // final downscale
    //QPixmap final = pm.scaled(targetW, targetH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);


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
    DE_DEBUG("Dropped index(",index,"), file(",dbFileName(uri.toStdString()),")")

    if (index < 0)
    {
        DE_ERROR("Invalid index ", index)
        return;
    }

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

    App::instance()->stopAudio();

    // Create GUI Shell
    auto w = new Plugin(plugin, this);
    w->show();

    // Connect GUI Shell
    connect(w, &Plugin::requestRemoval, this, &Track::removePlugin);

    connect(w, &Plugin::collapseChanged, this, &Track::updateLayout);

    // Manage GUI Shell
    m_plugins.insert(m_plugins.begin() + index, w);

    // Update DSP Chain...
    m_track->setPlugins(collectPlugins());

    // Update GUI Layout...
    m_dragIndex = -1;
    m_dropIndex = -1;
    updateLayout();

    App::instance()->playAudio();
}

void Track::removePlugin(Plugin* w)
{
    setUpdatesEnabled(false);

    if (!w)
    {
        DE_ERROR("Got nullptr")
        return;
    }

    App::instance()->stopAudio();

    w->setPlugin(nullptr);

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
        m_track->setPlugins(collectPlugins());
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

    //delete w;

    w->deleteLater();

    setUpdatesEnabled(true);

    updateLayout();

    App::instance()->playAudio();
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

int Track::computeDragIndex(const QPoint &pos)
{
    for (int i = 0; i < m_plugins.size(); ++i)
    {
        auto w = m_plugins[ i ];

        QRect r_label = w->labelRect();
        QRect r_parentRect = r_label.translated( w->pos() );
        //auto r_childInParent = QRect( w->mapTo(this, QPoint(0,0)), w->size() );

        if ( isMouseOver(pos, r_parentRect) )
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
    int dropIndex = computeDropIndex(pos);
    if (dropIndex != m_dropIndex)
    {
        m_dropIndex = dropIndex;
        //startAutoScrollIfNeeded(pos);
        updateLayout();
    }
}

void Track::dragLeaveEvent(QDragLeaveEvent*)
{
    // qDebug() << "dragLeaveEvent()";
    m_dragIndex = -1;
    m_dropIndex = -1;
    // m_scrollTimer->stop();
    //updateLayout();
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
}

// ------------------------------------------------------------
// Positionierung
// ------------------------------------------------------------
int Track::computeDropIndicatorPosX(int dragIndex, int dropIndex)
{
    // if (dragIndex <= 0)
    // {
    //     return -1; // Invalid
    // }

    if (dropIndex < 0)
    {
        return -1; // Invalid
    }

    const int n = static_cast<int>(m_plugins.size());
    if (n > 0)
    {
        if (dropIndex >= n)
        {
            auto p = m_plugins.back();
            return p->x() + p->width() + m_widgetSpacing;
        }
        else
        {
            return m_plugins[dropIndex]->x() - m_widgetSpacing - m_dropIndicatorWidth;
        }
    }
    else
    {
        return 0; // Indicate begin as initial drop pos.
    }
}

bool isValidDropIndex(int dragIndex, int dropIndex)
{
    if (dragIndex < 0)
    {
        return true;
    }

    if (dropIndex - dragIndex >= 2)
    {
        return true; // swap(dragIndex, dropIndex - 1);
    }

    if (dragIndex - dropIndex >= 1)
    {
        return true; // swap(dragIndex, dropIndex);
    }

    return false;
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
                if (m_dropIndex != -1)
                {
                    m_dropIndex = -1;
                    //updateLayout();
                }

                // if (m_dragIndex > -1 && m_dragIndex < int(m_plugins.size()))
                // {
                //     m_plugins[ m_dragIndex ]->setIsDragging(true);
                // }

                m_isDragging = true;
            }
            else
            {
                int dropIndex = computeDropIndex(m_posMouse);
                if (!isValidDropIndex(m_dragIndex,dropIndex))
                {
                    dropIndex = -1;
                }

                if (m_dropIndex != dropIndex)
                {
                    m_dropIndex = dropIndex;
                    updateLayout();
                }
            }
        }
    }
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
        int index = computeDragIndex( pos );
        if (index >= 0 && index < int(m_plugins.size()))
        {
            m_isDragInit = true;
            m_posDragInit = pos;
            m_dragIndex = index;
        }

        // for (auto p : m_plugins)
        // {
        //     p->setIsDragging(false);
        // }

        //updateLayout();
    }
}

void Track::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_isLeftPressed = false;

        bool didSwap = false;

        if (m_isDragging)
        {
            didSwap = swapWidgets( m_dragIndex, m_dropIndex );

            // for (auto p : m_plugins)
            // {
            //     p->setIsDragging(false);
            // }
        }

        m_isDragInit = false;
        m_isDragging = false;
        m_dropIndex = -1;
        m_dragIndex = -1;

        if (didSwap)
        {
            updateLayout();

            if (m_track)
            {
                m_track->setPlugins(collectPlugins());
            }
        }
    }
}

bool Track::swapWidgets(int dragIndex, int dropIndex)
{
    if (dragIndex == dropIndex)
    {
        return false;
    }

    const int n = static_cast<int>(m_plugins.size());
    if (n < 2)
    {
        return false;
    }

    if (dropIndex - dragIndex >= 2)
    {
        dropIndex--; // swap(dragIndex, dropIndex - 1);
    }

    // if (dragIndex - dropIndex >= 1)
    // {
    //     return true; // swap(dragIndex, dropIndex);
    // }

    if (dragIndex < 0 || dragIndex >= n)
    {
        // qDebug() << "Invalid drag index " << drag << " of " << n;
        return false;
    }

    if (dropIndex < 0 || dropIndex >= n)
    {
        // qDebug() << "Invalid drop index " << drop << " of " << n;
        return false;
    }

    // qDebug() << "Swap index " << (drag+1) << " <-> "  << (drop+1) << " of " << n;

    std::swap( m_plugins[ dragIndex ], m_plugins[ dropIndex ] );

    // emit reorderedWidgets();

    return true;
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

        arr.append(QString::fromStdString(plugin->getUri()));
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
