#include "TrackWidget.h"
#include "App.h"
#include <de/session/Track.h>

// ==================================================
TrackWidget::TrackWidget(QWidget* parent)
// ==================================================
    : QWidget(parent)
    , m_track{ nullptr }
{
    //setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setContentsMargins(0,0,0,0);
    setMouseTracking(true);
    setAcceptDrops(true);
    setStyleSheet("background: transparent;");

    // m_scrollTimer = new QTimer(this);
    // connect(App::instance(), &App::skinChanged, this, &TrackWidget::applySkin);

    // connect(m_scrollTimer, &QTimer::timeout, this, &TrackWidget::autoScroll);

    // m_dropTarget = new DropTarget(this);

    resize(600,300);
    applySkin();
}

TrackWidget::~TrackWidget()
{
    DE_OK()
}

void TrackWidget::shutdown()
{
    setTrack(nullptr);
    setParent(nullptr);
}

void TrackWidget::setTrack(de::session::Track* track)
{
    setUpdatesEnabled(false);
/*
    if (m_track == track)
    {
        return; // Nothing todo
    }

    if (m_track)
    {
        for (auto p : m_plugins)
        {
            p->setPlugin(nullptr);
            p->deleteLater();
        }
    }

    m_plugins.clear();
*/
    m_track = track;
/*
    if (m_track)
    {
        auto plugins = m_track->getPlugins();
        m_plugins.reserve(plugins.size());
        for (auto & p : plugins)
        {
            if (!p) { DE_ERROR("Got nullptr") continue; }
            auto pluginWidget = new PluginWidget(this);
            pluginWidget->setPlugin(p);
            m_plugins.emplace_back(pluginWidget);
        }
    }
*/
    setUpdatesEnabled(true);
    updateLayout();
}

void TrackWidget::applySkin()
{
    if (m_track)
    {
        for (auto p : m_track->m_pluginWidgets)
        {
            if (p) p->applySkin();
        }
    }

    //qDebug() << "TrackWidget::applySkin()";
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

    //setFixedHeight(m_height);
    updateLayout();
}

void TrackWidget::updateLayout()
{
    if (!m_track)
    {
        DE_ERROR("No track")
        return;
    }

    const auto& pluginWidgets = m_track->m_pluginWidgets;

    const int n = static_cast<int>(pluginWidgets.size());

    int x = 0;
    int y = 0;

    for (int i = 0; i < n; ++i)
    {
        auto pWidget = pluginWidgets[ i ];

        if (m_dropIndex == i)
        {
            x += m_dropIndicatorWidth + m_widgetSpacing;
        }

        int bestWidth = pWidget->computeBestWidth();
        pWidget->setGeometry(x,y,bestWidth,height());

        x += pWidget->width() + m_widgetSpacing;
    }

    if (m_dropIndex >= n)
    {
        x += m_dropIndicatorWidth + m_widgetSpacing;
    }

    m_width = x;

    int dropTargetWidth = std::max(width() - m_width, m_dropTargetWidth);
    m_rcDropTarget = QRect(m_width, 0, dropTargetWidth, height());

    m_bEmitOverview = true;
    update();

}

/*
std::vector<de::audio::SharedPlugin>
TrackWidget::collectPlugins() const
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
*/

// ------------------------------------------------------------
// Zeichnen
// ------------------------------------------------------------
void TrackWidget::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    if (!isVisible()) { return; }
    const int w = e->size().width();
    const int h = e->size().height();
    if (w < 1) return;
    if (h < 1) return;

    // int w = e->size().width();
    // int h = e->size().height();
    // DE_TRACE("w(",w,"), h(",h,")")
    updateLayout();
}

void TrackWidget::paintEvent(QPaintEvent* e)
{
    if (!isVisible()) { return; }
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;

    if (m_bInPaintEvent)
    {
        DE_ERROR("m_bInPaintEvent")
        return;
    }

    m_bInPaintEvent = true;

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
        int y = height() - 11; // - fm.ascent();
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

    m_bInPaintEvent = false;
}


void TrackWidget::emitTrackOverview()
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

    // dst_w = std::clamp(dst_w,8,2*1024);
    // dst_h = std::clamp(dst_h,8,2*1024);
    // DE_BENNI("dst(",dst_w,",",dst_h,")")
    float fx = float(dst_w) / float(src_w);
    float fy = float(dst_h) / float(src_h);
    DE_BENNI("scale(",fx,",",fy,")")
    // float scale_x = de::clampf( float(dst_w) / float(src_w), 0.001f, 1.0f);
    // float scale_y = de::clampf( float(dst_h) / float(src_h), 0.001f, 1.0f);

    QPixmap pixmap(dst_w, dst_h);
    pixmap.fill(Qt::transparent);
    QPainter dc;
    if (dc.begin(&pixmap))
    {
        dc.scale(fx,fy);
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
}

/*
// ------------------------------------------------------------
// PluginWidget hinzufügen
// ------------------------------------------------------------
void TrackWidget::addPlugin(const QString &uri)
{
    insertPlugin(m_plugins.size(), uri);
}

void TrackWidget::insertPlugin(int index, const QString &uri)
{
    DE_DEBUG("Dropped index(",index,"), file(",dbFileName(uri.toStdString()),")")

    if (index < 0)
    {
        DE_ERROR("Invalid index ", index)
        return;
    }

    if (!m_track)
    {
        DE_ERROR("No DspTrack")
        return;
    }

    auto plugin = App::instance()->createPlugin(uri.toStdString());
    if (!plugin)
    {
        DE_ERROR("No plugin")
        return;
    }

    App::instance()->stopAudio();

    plugin->setTrack(
    // Create GUI Shell
    auto w = new PluginWidget(this);
    w->setPlugin(plugin);
    w->show();

    // Connect GUI Shell
    connect(w, &PluginWidget::requestRemoval, this, &TrackWidget::removePlugin);

    connect(w, &PluginWidget::collapseChanged, this, &TrackWidget::updateLayout);

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

void TrackWidget::removePlugin(PluginWidget* w)
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
*/

// ------------------------------------------------------------
// Drag&Drop
// ------------------------------------------------------------
void TrackWidget::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();

        // auto u = e->mimeData()->urls();
        // qDebug() << "dragEnterEvent(" << u.size() << ")";
    }
}

int TrackWidget::computeDropIndex(const QPoint &pos)
{
    if (!m_track)
    {
        DE_ERROR("No track")
        return -1;
    }

    const auto& pluginWidgets = m_track->m_pluginWidgets;

    for (int i = 0; i < pluginWidgets.size(); ++i)
    {
        auto w = pluginWidgets[ i ];

        if (pos.x() < w->x() + w->width() / 2)
        {
            return i;
        }
    }

    return pluginWidgets.size();
}

// ------------------------------------------------------------
// Drag&Drop Reorder:
// ------------------------------------------------------------

int TrackWidget::computeDragIndex(const QPoint &pos)
{
    if (!m_track)
    {
        DE_ERROR("No track")
        return -1;
    }

    const auto& pluginWidgets = m_track->m_pluginWidgets;

    for (int i = 0; i < pluginWidgets.size(); ++i)
    {
        auto w = pluginWidgets[ i ];

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

void TrackWidget::dragMoveEvent(QDragMoveEvent* e)
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

void TrackWidget::dragLeaveEvent(QDragLeaveEvent*)
{
    // qDebug() << "dragLeaveEvent()";
    m_dragIndex = -1;
    m_dropIndex = -1;
    // m_scrollTimer->stop();
    //updateLayout();
}

void TrackWidget::dropEvent(QDropEvent* e)
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
            if (!m_track)
            {
                DE_ERROR("No track")
            }
            else
            {
                m_track->insertPlugin(m_dropIndex, url);
            }
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
int TrackWidget::computeDropIndicatorPosX(int dragIndex, int dropIndex)
{
    // if (dragIndex <= 0)
    // {
    //     return -1; // Invalid
    // }

    if (dropIndex < 0)
    {
        return -1; // Invalid
    }

    if (!m_track)
    {
        DE_ERROR("No track")
        return -1;
    }

    const auto& pluginWidgets = m_track->m_pluginWidgets;

    const int n = static_cast<int>(pluginWidgets.size());
    if (n > 0)
    {
        if (dropIndex >= n)
        {
            auto p = pluginWidgets.back();
            return p->x() + p->width() + m_widgetSpacing;
        }
        else
        {
            return pluginWidgets[dropIndex]->x() - m_widgetSpacing - m_dropIndicatorWidth;
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

void TrackWidget::mouseMoveEvent(QMouseEvent* e)
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

void TrackWidget::mousePressEvent(QMouseEvent* e)
{
    if (!m_track)
    {
        DE_ERROR("No track")
        return;
    }

    const auto& pluginWidgets = m_track->m_pluginWidgets;

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

        // If mouse is over PluginWidget->m_rcLabel
        // then start a drag operation...
        int index = computeDragIndex( pos );
        if (index >= 0 && index < int(pluginWidgets.size()))
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

void TrackWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (!m_track)
    {
        DE_ERROR("No track")
        return;
    }

    const auto& pluginWidgets = m_track->m_pluginWidgets;

    if (e->button() == Qt::LeftButton)
    {
        m_isLeftPressed = false;

        bool didSwap = false;

        if (m_isDragging)
        {
            m_isDragInit = false;
            m_isDragging = false;

            didSwap = m_track->swapPlugins( m_dragIndex, m_dropIndex );

            m_dropIndex = -1; // Reset before calling swapPlugins
            m_dragIndex = -1;

            // for (auto p : m_plugins)
            // {
            //     p->setIsDragging(false);
            // }
        }

        if (didSwap)
        {
            updateLayout();
            // if (m_track)
            // {
            //     m_track->setPlugins(collectPlugins());
            // }
        }
    }
}

/*
bool TrackWidget::swapWidgets(int dragIndex, int dropIndex)
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
*/
// ------------------------------------------------------------
// Auto-Scroll
// ------------------------------------------------------------
void TrackWidget::startAutoScrollIfNeeded(const QPoint &pos)
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

void TrackWidget::autoScroll()
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
TrackWidget::TrackWidget(QWidget *parent)
// ============================================================
    : QWidget(parent)
{
    setContentsMargins(0,0,0,0);

    m_Track = new TrackWidget;

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
TrackWidget::~TrackWidget()
// ============================================================
{

}
*/
