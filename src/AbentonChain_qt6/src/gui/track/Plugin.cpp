#include "gui/track/Plugin.h"
#include "App.h"
#include "gui/Skin.h"
#include "gui/track/Track.h"

namespace {

    void
    bringToFront(QWidget* w)
    {
        if (!w) { DE_ERROR("nullptr") return; }
        w->show();
        w->raise();
        w->activateWindow();
        w->setWindowState((w->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive );
    }

    std::string
    createPerfStr( double nSeconds )
    {
        constexpr uint64_t nanos_per_sec = 1000000000ull;

        uint64_t ns = uint64_t( 0.5 + ( 1.0e9 * std::abs( nSeconds ) ) );

        const uint64_t hh = ns / (nanos_per_sec * 3600);
        ns -= (nanos_per_sec * hh * 3600);
        const uint64_t mm = ns / (nanos_per_sec * 60);
        ns -= (nanos_per_sec * mm * 60);
        const uint64_t ss = ns / nanos_per_sec;
        ns -= (nanos_per_sec * ss);
        const uint64_t ms = ns / 1000000ull;
        ns -= (1000000ull * ms);
        const uint64_t us = ns / 1000ull;
        ns -= (1000ull * us);

        std::ostringstream o;

        if (nSeconds < 0.0) o << "-";

        if (hh > 0) { o << hh << "h "; }
        if (mm > 0) { o << mm << "min "; }
        if (ss > 0) { o << ss << "sec "; }
        if (ms > 0) { o << ms << "ms "; }
        if (us > 0) { o << us << "µs "; }
        //if (ns > 0) { o << ns << "ns"; }

        return o.str();
    }
}

Plugin::Plugin(de::audio::SharedPlugin plugin, QWidget* parent)
    : QWidget(parent)
    , m_plugin(nullptr)
{
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

    setPlugin(plugin);
}

Plugin::~Plugin()
{
    DE_TRACE("")
}

QRect Plugin::labelRect() const { return m_rcLabel; }

de::audio::SharedPlugin Plugin::getPlugin() { return m_plugin; }

void Plugin::unloadPlugin()
{
    if (!m_plugin)
    {
        return;
    }

    DE_TRACE(m_plugin->getName())

    setUpdatesEnabled(false); // Disable paintEvent()

    auto plugin = m_plugin; // Copy pointer and make m_plugin nullptr for internal slots using m_plugin.
    m_plugin = nullptr;

    m_audioMeter->stopUpdateTimer();
    m_audioMeter->setPlugin(nullptr);

    auto editor = plugin->getEditor();
    if (editor)
    {
        editor->hide();

        DE_TRACE("Disconnect editor")
        disconnect(editor,
                   nullptr,
                   this,
                   nullptr );
    }

    // disconnect(nullptr, nullptr, this, nullptr ); // Disconnect signals;
    disconnect(this, nullptr, nullptr, nullptr ); // Disconnect slots;
    disconnect(m_body->getPad(), nullptr, nullptr, nullptr ); // Disconnect slots;
    disconnect(m_body->getComboPreset(), nullptr, nullptr, nullptr ); // Disconnect slots;
    disconnect(m_body->getComboParam1(), nullptr, nullptr, nullptr ); // Disconnect slots;
    disconnect(m_body->getComboParam2(), nullptr, nullptr, nullptr ); // Disconnect slots;

    m_title = "";
    m_btnEnable->blockSignals(true);
    m_btnEnable->setChecked(false);
    m_btnEnable->blockSignals(false);

    m_btnExpand->blockSignals(true);
    m_btnExpand->setChecked(false);
    m_btnExpand->blockSignals(false);

    m_btnWrench->blockSignals(true);
    m_btnWrench->setChecked(false);
    m_btnWrench->blockSignals(false);

    m_btnUpdate->blockSignals(true);
    m_btnUpdate->setChecked(false);
    m_btnUpdate->blockSignals(false);

    m_btnEditor->blockSignals(true);
    m_btnEditor->setChecked(false);
    m_btnEditor->blockSignals(false);

    // Fill ProgramCombo:
    auto combo0 = m_body->getComboPreset();
    auto combo1 = m_body->getComboParam1();
    auto combo2 = m_body->getComboParam2();
    combo0->clear();
    combo1->clear();
    combo2->clear();

    auto pad = m_body->getPad();
    pad->setText(Pad::eT_Type, "");
    pad->setText(Pad::eT_Runtime, "");
    pad->setText(Pad::eT_Name, "");
    pad->setText(Pad::eT_Vendor, "");
    //pad->setText(Pad::eT_Version, "");
    pad->setValueXY(0.0f,0.0f);
/*
    auto trackWidget = static_cast<Track*>(parentWidget());
    if (trackWidget)
    {
        auto track = trackWidget->getTrack();
        if (track)
        {
            track->markObsolete(plugin);
        }
    }
*/
    // plugin->closePlugin(); -> Too early, leads to segfault, collect in trashbin.

    setUpdatesEnabled(true); // Enable paintEvent()
}

void Plugin::loadPlugin(de::audio::SharedPlugin plugin)
{
    if (!plugin)
    {
        // DE_ERROR("No plugin")
        return;
    }

    setUpdatesEnabled(false); // Disable paintEvent()

    // Transition:
    m_plugin = plugin;

    m_audioMeter->setPlugin(m_plugin.get());
    m_audioMeter->playUpdateTimer();

    m_title = QString::fromStdString(m_plugin->getName());
    m_btnEnable->setChecked(true);
    m_btnExpand->setChecked(false);
    m_btnWrench->setChecked(true);
    m_btnUpdate->setChecked(false);
    m_btnEditor->setChecked(false);

    auto editor = m_plugin->getEditor();
    if (editor)
    {
        auto title = QString("%1 | %2 | %3 | %4 | x64 | AbentonLive_qt6")
            .arg(QString::fromStdString(m_plugin->getName()))
            .arg(QString::fromStdString(m_plugin->getVersion()))
            .arg(QString::fromStdString(m_plugin->getVendor()))
            .arg(QString::fromStdString(m_plugin->getTypeStr()))
        ;
        editor->setWindowTitle(title);

        bringToFront(editor);

        DE_TRACE("Connect editor")
        QObject::connect(editor,
                         SIGNAL(closed()),
                         this,
                         SLOT(on_editorWindowClosed()),
                         Qt::QueuedConnection );
    }

    auto pad = m_body->getPad();
    pad->setText(Pad::eT_Type, QString("%1 %2 %3")
            .arg(QString::fromStdString(m_plugin->getTypeStr()))
            .arg(m_plugin->isSynth() ? "Synth" : "Effect")
            .arg(QString::fromStdString(m_plugin->getVersion())));
    pad->setText(Pad::eT_Runtime, QString::fromStdString(createPerfStr(m_plugin->getRuntime())));
    pad->setText(Pad::eT_Name, QString::fromStdString(m_plugin->getName()));
    pad->setText(Pad::eT_Vendor, QString::fromStdString(m_plugin->getVendor()));
    //pad->setText(Pad::eT_Version, QString::fromStdString(m_plugin->getVersion()));


    // Fill ProgramCombo:
    auto combo0 = m_body->getComboPreset();
    combo0->clear();

    const de::audio::Programs& progs = m_plugin->getPrograms();
    for (uint32_t i = 0; i < progs.size(); ++i)
    {
        const de::audio::Program& pi = progs[i];
        auto name = QString::fromStdString(pi.m_name);
        auto text = QString("%1: %2").arg(i).arg(name);
        combo0->addItem(text, i);
    }

    // Fill ParameterCombos:
    auto combo1 = m_body->getComboParam1();
    auto combo2 = m_body->getComboParam2();
    combo1->clear();
    combo2->clear();
    combo1->addItem("Disabled", UINT32_MAX);
    combo2->addItem("Disabled", UINT32_MAX);

    const de::audio::Parameters& params = m_plugin->getParameters();
    for (uint32_t i = 0; i < params.size(); ++i)
    {
        const de::audio::Parameter& pi = params[i];
        if (pi.m_flags & de::audio::Parameter::kCanAutomate)
        {
            auto name = QString::fromStdString(pi.m_name);
            auto text = QString("%1: %2").arg(i).arg(name);
            combo1->addItem(text, pi.m_id);
            combo2->addItem(text, pi.m_id);
        }
    }


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

    connect(m_body->getPad(), &Pad::onParamChanged,
            this, &Plugin::on_pad);

    connect(m_body->getComboPreset(), &ComboBox::currentIndexChanged,
            this, &Plugin::on_comboPreset);

    connect(m_body->getComboParam1(), &ComboBox::currentIndexChanged,
            this, &Plugin::on_comboParam1);

    connect(m_body->getComboParam2(), &ComboBox::currentIndexChanged,
            this, &Plugin::on_comboParam2);

    setUpdatesEnabled(true); // Enable paintEvent()
}

void Plugin::setPlugin(de::audio::SharedPlugin plugin)
{
    DE_OK()

    unloadPlugin();

    loadPlugin(plugin);
}

void Plugin::on_showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Delete/Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}

void Plugin::on_pad(float x, float y)
{
    if (!m_plugin)
    {
        DE_ERROR("No plugin")
        return;
    }

    const uint32_t paramIdX = m_body->getComboParam1()->currentData().toUInt();
    const uint32_t paramIdY = m_body->getComboParam2()->currentData().toUInt();

    if (paramIdX != UINT32_MAX)
    {
        //DE_TRACE("paramId.X = ", paramIdX)
        m_plugin->setParameterValue(paramIdX, x);
    }

    if (paramIdY != UINT32_MAX)
    {
        //DE_TRACE("paramId.Y = ", paramIdY)
        m_plugin->setParameterValue(paramIdY, y);
    }
}

// On comboBox1 currentIndexChanged we set Pad.X to current value of selected Param1
void Plugin::on_comboPreset(int index)
{
    if (!m_plugin)
    {
        DE_ERROR("No plugin")
        return;
    }

    // const auto & progs = m_plugin->getPrograms();

    // if (index < 0 || index >= int(progs.size()))
    // {
    //     DE_ERROR("Invalid index ",index)
    //     return;
    // }

    m_plugin->setProgram(index);
}

// On comboBox1 currentIndexChanged we set Pad.X to current value of selected Param1
void Plugin::on_comboParam1(int index)
{
    if (!m_plugin)
    {
        DE_ERROR("No plugin")
        return;
    }

    const uint32_t paramId = m_body->getComboParam1()->currentData().toUInt();
    if (paramId != UINT32_MAX)
    {
        float normValue = m_plugin->getParameterValue(paramId);
        m_body->getPad()->setValueX(normValue);
    }
}

// On comboBox2 currentIndexChanged we set Pad.Y to current value of selected Param2
void Plugin::on_comboParam2(int index)
{
    if (!m_plugin)
    {
        DE_ERROR("No plugin")
        return;
    }

    const uint32_t paramId = m_body->getComboParam2()->currentData().toUInt();
    if (paramId != UINT32_MAX)
    {
        float normValue = m_plugin->getParameterValue(paramId);
        m_body->getPad()->setValueY(normValue);
    }
}

void Plugin::on_editorWindowClosed()
{
    //DE_ERROR("Editor closed")
    m_btnWrench->blockSignals( true );
    m_btnWrench->setChecked( false );
    m_btnWrench->blockSignals( false );
}

void Plugin::on_pressedBtnEnable( bool checked )
{
    if (!m_plugin)
    {
        DE_ERROR("No plugin")
    }

    if (!checked)
    {
        // Panic
        for (int i = 0; i < 16; ++i) // For all channels
        {
            auto a = de::midi::ShortMidiMessage::CC64_sustainPedal(i,false);
            auto b = de::midi::ShortMidiMessage::allNotesOff(i);
            auto c = de::midi::ShortMidiMessage::resetAllControllers(i);
            //auto d = de::midi::ShortMidiMessage::allSoundsOff(i);
            m_plugin->onShortMidiMessage(0,a);
            m_plugin->onShortMidiMessage(0,b);
            m_plugin->onShortMidiMessage(0,c);
        }
    }

    m_plugin->setBypassed(!checked);
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

    if (checked)
    {
        auto screenSize = editor->screen()->geometry().size();
        int scrW = screenSize.width();
        int scrH = screenSize.height();
        QRect r = editor->frameGeometry();
        int w = r.width();
        int h = r.height();
        int x = r.x();
        int y = r.y();

        if (w > scrW - 200) { w = scrW - 200; }
        if (h > scrH - 200) { h = scrH - 200; }
        if (w < 100) { w = 100; }
        if (h < 100) { h = 100; }
        if (x < 0) { x = 0; }
        if (y < 0) { y = 0; }
        if (x + w > scrW) { x = scrW - w; }
        if (y + h > scrH) { y = scrH - h; }

        if (w != r.width() || h != r.height() || x != r.x() || y != r.y())
        {
            editor->setGeometry(x,y,w,h);
        }

        bringToFront(editor);
    }
    else
    {
        editor->hide();
    }
}

void Plugin::on_pressedBtnUpdate( bool checked )
{

}
void Plugin::on_pressedBtnEditor( bool checked )
{
    // m_bCollapsed = checked;
    // applySkin();
    // static_cast<Track*>(parent())->updateLayout();
}

// void Plugin::on_doubleClickedLabel()
// {

// }
// void Plugin::on_dragStarted(QPoint dragStart)
// {

// }

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

    const auto& skin = App::instance()->getSkin();
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

        //setFixedSize( m_width + aw, m_height );
        setMinimumSize( m_width + aw, m_height );
        setMaximumSize( m_width + aw, m_height );
    }
    else // Normal
    {
        m_btnExpand->show();
        m_body->show();

        m_width = (m_baseWidth * skin.zoom) / 100;
        m_height = (m_baseHeight * skin.zoom) / 100;

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

        m_body->move( 0,m_headerHeight+1 );

        m_rcAudioMeter = QRect(m_width,0,aw,m_height);

        int ay = (m_height - m_audioMeter->height())/2;
        m_audioMeter->move(m_width,ay);

        //setFixedSize( m_width + aw, m_height );
        setMinimumSize( m_width + aw, m_height );
        setMaximumSize( m_width + aw, m_height );
    }

    updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}

void Plugin::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    // const int w = e->size().width();
    // const int h = e->size().height();
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

    if (m_plugin && m_body)
    {
        auto pad = m_body->getPad();
        if (pad)
        {
            double runTime = 0.0;
            if (m_plugin)
            {
                runTime = m_plugin->getRuntime();
            }
            pad->setText(Pad::eT_Runtime, QString::fromStdString(createPerfStr(runTime)));
        }
    }

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
        emit collapseChanged();
    }
    QWidget::mouseDoubleClickEvent(event);
}
void Plugin::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
}
