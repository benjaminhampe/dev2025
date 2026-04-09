#include "gui/track/Plugin.h"
#include "App.h"
#include "gui/Skin.h"
#include "gui/track/Track.h"

Plugin::Plugin(de::audio::IPlugin* plugin, QWidget* parent)
    : QWidget(parent)
    , m_plugin(nullptr)
{
    DE_TRACE("")
    //setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setStyleSheet("background: transparent;");

    m_btnEnable = new EnableButton(this);
    m_btnExpand = new ArrowButton(this);
    m_btnWrench = new WrenchButton(this);
    m_btnUpdate = new UpdateButton(this);
    m_btnEditor = new EditorButton(this);
    m_body = new Body(this);
    m_audioMeter = new AudioMeter(this);

    applySkin();

    connect(this, &QWidget::customContextMenuRequested,
            this, &Plugin::on_showContextMenu);

    connect(m_btnEnable, &QPushButton::toggled,
            this, &Plugin::on_pressedBtnEnable);

    connect(m_btnExpand, &QPushButton::toggled,
            this, &Plugin::on_pressedBtnExpand);

    connect(m_btnWrench, &QPushButton::toggled,
            this, &Plugin::on_pressedBtnWrench);

    connect(m_btnUpdate, &QPushButton::toggled,
            this, &Plugin::on_pressedBtnUpdate);

    connect(m_btnEditor, &QPushButton::toggled,
            this, &Plugin::on_pressedBtnEditor);

    setPlugin(plugin);
}

Plugin::~Plugin()
{
    DE_TRACE("")
}

QRect Plugin::labelRect() const { return m_rcLabel; }

de::audio::IPlugin* Plugin::getPlugin() { return m_plugin; }

void Plugin::setPlugin(de::audio::IPlugin* plugin)
{
    // Disconnect old
    if (m_plugin)
    {
        auto editor = m_plugin->getEditor();
        if (editor)
        {
            editor->hide();

            DE_TRACE("Disconnect editor")
            disconnect(editor,
                       nullptr,
                       this,
                       nullptr );
        }
    }

    // Transition:
    m_plugin = plugin;

    // Connect new:
    if (m_plugin)
    {
        m_title = QString::fromStdString(m_plugin->name());
        m_btnEnable->setChecked(true);
        m_btnExpand->setChecked(false);
        m_btnWrench->setChecked(true);
        m_btnUpdate->setChecked(false);
        m_btnEditor->setChecked(false);

        auto editor = m_plugin->getEditor();
        if (editor)
        {
            editor->show();
            editor->raise();
            editor->activateWindow();
            editor->setWindowState((editor->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive );

            DE_TRACE("Connect editor")
            QObject::connect(editor,
                             SIGNAL(closed()),
                             this,
                             SLOT(on_editorWindowClosed()),
                             Qt::QueuedConnection );
        }
    }
    else
    {
        m_title = "";
        m_btnEnable->setChecked(false);
        m_btnExpand->setChecked(false);
        m_btnWrench->setChecked(false);
        m_btnUpdate->setChecked(false);
        m_btnEditor->setChecked(false);
    }
}



void Plugin::on_showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}


void Plugin::on_editorWindowClosed()
{
   DE_ERROR("Editor closed")
   m_btnWrench->blockSignals( true );
   m_btnWrench->setChecked( false );
   m_btnWrench->blockSignals( false );
}


void Plugin::on_pressedBtnEnable( bool checked )
{

}
void Plugin::on_pressedBtnExpand( bool checked )
{

}
void Plugin::on_pressedBtnWrench( bool checked )
{
    if (!m_plugin)
    {
        DE_ERROR("No plugin")
        return;
    }

    auto editor = m_plugin->getEditor();

    if (!editor)
    {
        DE_ERROR("No editor")
        return;
    }

    editor->setVisible(checked);
    if (checked)
    {
        editor->raise();
    }
}

void Plugin::on_pressedBtnUpdate( bool checked )
{

}
void Plugin::on_pressedBtnEditor( bool checked )
{
    m_bCollapsed = checked;
    applySkin();
    static_cast<Track*>(parent())->updateLayout();
}

void Plugin::on_doubleClickedLabel()
{

}
void Plugin::on_dragStarted(QPoint dragStart)
{

}

// QSize Plugin::sizeHint() const { return QSize(m_width, m_height); }
// QSize Plugin::minimumSizeHint() const { return sizeHint(); }

void Plugin::applySkin()
{
    // DE_DEBUG("")
    m_btnEnable->applySkin();
    m_btnExpand->applySkin();
    m_btnWrench->applySkin();
    m_btnUpdate->applySkin();
    m_btnEditor->applySkin();
    m_body->applySkin();
    m_audioMeter->applySkin();

    const int bw = m_btnEnable->width();
    const int bh = m_btnEnable->height();

    const int aw = m_audioMeter->width();
    const int ah = m_audioMeter->height();

    DE_TRACE("b(",bw,",",bh,"), a(",aw,",",ah,")")

    const auto& skin = App::instance()->currentSkin();
    m_windowColor = skin.windowColor;
    m_panelColor = skin.panelColor;
    m_headerColor = skin.headerColor;
    m_headerColorActive = skin.headerColorActive;
    m_textColor = skin.textColor;

    m_radius = (m_baseRadius * skin.zoom) / 100;
    m_headerHeight = (m_baseHeaderHeight * skin.zoom) / 100;
    m_spacing2 = (m_baseSpacing2 * skin.zoom) / 100;
    m_spacing4 = (m_baseSpacing4 * skin.zoom) / 100;

    if (m_bCollapsed)
    {
        m_btnExpand->hide();
        m_body->hide();

        m_width = (m_baseHeaderWidth * skin.zoom) / 100;
        m_height = (m_baseHeight * skin.zoom) / 100;
        setFixedSize( m_width + aw, m_height );
        m_rcHeader = QRect(0,0,m_width,m_headerHeight);

        int x = (m_rcHeader.width() - bw)/2;
        int y = (m_rcHeader.height() - bh)/2;
        m_btnEnable->move(x,y);

        int s2 = m_spacing2;
        int s4 = m_spacing4;
        int s6 = s2 + s4;

        y = m_height - (s6 + bh);
        m_btnEditor->move(x,y);
        y -= s4 + bh;
        m_btnUpdate->move(x,y);
        y -= s4 + bh;
        m_btnWrench->move(x,y);

        int lh = m_height - m_headerHeight - 3*bh - 4*s4 - s6;
        int ly = m_headerHeight + s4;
        m_rcLabel = QRect(3,ly,m_width-5,lh);

        m_rcAudioMeter = QRect(m_width,0,aw,m_height);

        int ay = (m_height - m_audioMeter->height())/2;
        m_audioMeter->move(m_width,ay);
    }
    else // Normal
    {
        m_btnExpand->show();
        m_body->show();

        m_width = (m_baseWidth * skin.zoom) / 100;
        m_height = (m_baseHeight * skin.zoom) / 100;
        setFixedSize( m_width + aw, m_height );
        m_rcHeader = QRect(0,0,m_width,m_headerHeight);

        int x = m_spacing4;
        int y = (m_headerHeight - bh)/2;
        m_btnEnable->move(x,y);
        x += bw + m_spacing2;

        m_btnExpand->move(x,y);
        x += bw + m_spacing2;

        m_btnWrench->move(x,y);
        x += bw + m_spacing2;

        int lx1 = x;

        x = m_width - m_spacing4 - bw;
        m_btnEditor->move(x,y);
        x -= m_spacing4 + bw;
        m_btnUpdate->move(x,y);
        x -= m_spacing4;

        int lx2 = x;

        m_rcLabel = QRect(lx1,2,lx2-lx1,m_headerHeight-4);

        m_body->move( 0,m_headerHeight );

        m_rcAudioMeter = QRect(m_width,0,aw,m_height);

        int ay = (m_height - m_audioMeter->height())/2;
        m_audioMeter->move(m_width,ay);
    }

    //updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}

void drawShell(QPainter & dc, QRect pos,
    int headerHeight, QColor panelColor, int panelRadius,
    QColor headerColor)
{
    dc.setRenderHint(QPainter::Antialiasing);
    dc.setPen(Qt::NoPen);

    dc.setBrush(panelColor);
    int d = headerHeight/2;
    int r = panelRadius;
    int x = pos.x();
    int y = pos.y();
    int w = pos.width();
    int h = pos.height();
    dc.drawRoundedRect(QRect(x,y+d,w,h-d), r,r);

    dc.setBrush(QBrush(headerColor));
    dc.drawRoundedRect(QRect(x,y,w,headerHeight-2), r,r);
    dc.drawRect(QRect(x,y+r+2,w,headerHeight-2-r));
}

void drawLabelH(QPainter & dc, QRect pos,
                QColor textColor, QString fullText, QFont font )
{
    dc.fillRect(pos, QColor(255,255,255));

    dc.setBrush(Qt::NoBrush);
    dc.setPen(QPen(textColor));
    dc.setFont(font);

    QFontMetrics fm(font);
    QString s = fm.elidedText(fullText,
                              Qt::ElideRight, pos.width());

    dc.drawText(pos, Qt::AlignLeft | Qt::AlignVCenter,
                s, &pos);
}

void drawLabelV(QPainter & dc, QRect pos,
    QColor textColor, QString fullText, QFont font )
{
    dc.fillRect(pos, QColor(255,255,255));

    dc.setPen(QPen(textColor));
    dc.setBrush(Qt::NoBrush);

    QFontMetrics fm(font);
    QString s = fm.elidedText(fullText,
        Qt::ElideRight, pos.height());

    dc.save(); // <--- save state
    dc.setFont(font);
    dc.translate(pos.left() + fm.ascent(),pos.bottom());
    dc.rotate(-90); // ccw
    dc.drawText(0, 0, s);
    dc.restore();
}

void Plugin::paintEvent(QPaintEvent *)
{
    QPainter dc(this);

    // [Draw] Shell
    QRect r_shell(0,0,m_width,m_height);
    drawShell(dc,
              r_shell,
              m_headerHeight,
              m_panelColor,
              m_radius,
              m_bFocused ? m_headerColorActive
                         : m_headerColor);

    // <debug>
    dc.setPen(QPen(QColor(255,55,55)));
    dc.setBrush(Qt::NoBrush);
    dc.drawRect(m_rcLabel);
    // </debug>

    // [Draw] Label
    auto f = QFont("Noto Sans", 10, QFont::Bold);

    if (m_bCollapsed)
    {
        drawLabelV(dc, m_rcLabel, m_textColor,
                   m_title, f);
    }
    else
    {
        drawLabelH(dc, m_rcLabel, m_textColor,
                   m_title, f);
    }
}

// void
// Plugin::setIsDragging(bool isDragging)
// {
//     if (m_isDragging != isDragging)
//     {
//         m_isDragging = isDragging;
//         update();
//     }
// }

void Plugin::focusInEvent(QFocusEvent* event)
{
    m_bFocused = true;
    update();
    QWidget::focusInEvent(event);
}
void Plugin::focusOutEvent(QFocusEvent* event)
{
    m_bFocused = false;
    update();
    QWidget::focusOutEvent(event);
}
void Plugin::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
}
void Plugin::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
}
void Plugin::mouseDoubleClickEvent(QMouseEvent* event)
{
    auto pos = event->position().toPoint();

    QRect r_label = labelRect();
    if (r_label.contains(pos))
    {
        m_bCollapsed = !m_bCollapsed;
        applySkin();
    }
    QWidget::mouseDoubleClickEvent(event);
}
void Plugin::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
}
