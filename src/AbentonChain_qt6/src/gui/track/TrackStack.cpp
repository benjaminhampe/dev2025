#include "TrackStack.h"
#include "App.h"
#include "gui/Skin.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

TrackStack::TrackStack(QWidget* parent)
    : QWidget(parent)
    , m_quickHelp(nullptr)
    , m_midiMeter(nullptr)
    , m_audioMeter(nullptr)
    , m_trackWidget(nullptr)
{
    DE_TRACE("")
    setObjectName("TrackStack");
    setContentsMargins(0,8,8,8);
    setStyleSheet("background: transparent;");
    //setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    m_quickHelp = new QWidget(this);
    m_quickHelp->setMinimumWidth(300);
    m_quickHelp->setMaximumWidth(300);
    m_midiMeter = new MidiMeter(this);
    m_audioMeter = new AudioMeter(this);
    m_trackWidget = new Track(App::instance()->getTrack(0), this);

    // Install event filter so we detect when
    // the child resizes
    // (needed because sizeHint changes dynamically)
    //m_trackWidget->installEventFilter(this);

    connect(m_trackWidget, &Track::newOverview,
        this, [=] (QPixmap pix)
        {
            // DE_BENNI("newTrackOverview")
            emit newTrackOverview(pix); // for Footer overview/scrollbar
        });

    // auto h = new QHBoxLayout;
    // h -> setContentsMargins(0,0,0,0);
    // h -> setSpacing(0);
    // h -> addWidget(m_quickHelp, 0, Qt::AlignVCenter);
    // h -> addWidget(m_midiMeter, 0, Qt::AlignVCenter);
    // h -> addWidget(m_audioMeter, 0, Qt::AlignVCenter);
    // h -> addWidget(m_trackWidget,1);
    // setLayout(h);

    connect(App::instance().get(), &App::skinChanged,
            this, &TrackStack::applySkin);

    setAudioOnly(false);

    applySkin();
}

TrackStack::~TrackStack()
{
    DE_TRACE("")
}

// QSize TrackStack::sizeHint() const { return QSize(0,m_height); }
// QSize TrackStack::minimumSizeHint() const { return QSize(0,m_height); }

void TrackStack::applySkin()
{
    DE_TRACE()
    //m_quickHelp->applySkin();
    m_midiMeter->applySkin();
    m_audioMeter->applySkin();
    m_trackWidget->applySkin();

    const auto& skin = App::instance()->getSkin();
    m_windowColor = skin.windowColor;
    m_zoom = skin.zoom;
    //m_height = (m_baseHeight * skin.zoom) / 100;
    m_margin = (m_baseMargin * skin.zoom) / 100;
    m_radius = ((m_baseRadius-2) * skin.zoom) / 100;
    //setFixedHeight(m_height);
    // setMinimumHeight(h);
    // setMaximumHeight(h);
    // m_scrollArea->setMinimumSize(0,h);
    // m_scrollArea->setMaximumSize(0,h);
    // updateGeometry(); // tells Qt: “my sizeHint() changed”
    //resize(width(),m_height);
    updateLayout();
}

void TrackStack::updateLayout()
{
    const int w = width();
    const int h = height();
    const int l = contentsMargins().left();
    const int t = contentsMargins().top();
    const int r = contentsMargins().right();
    const int b = contentsMargins().bottom();

    int x = l;
    int y = t;
    if (m_quickHelp->isVisible())
    {
        int w1 = m_quickHelp->width();
        m_quickHelp->setGeometry(x,y,w1,h - (t+b));
        x += w1;
    }

    if (m_audioMeter->isVisible())
    {
        int w1 = (m_zoom * 18) / 100;
        m_audioMeter->setGeometry(x,y,w1,h - (t+b));
        x += w1;
    }

    if (m_midiMeter->isVisible())
    {
        int w1 = (m_zoom * 18) / 100;
        m_midiMeter->setGeometry(x,y,w1,h - (t+b));
        x += w1;
    }

    // if (m_trackWidget->isVisible())
    {
        int w1 = w - r - x;
        m_trackWidget->setGeometry(x,y,w1,h - (t+b));
        //x += w1;
    }

    update();
}

void TrackStack::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    if (w < 1) return;
    if (h < 1) return;
    updateLayout();
}

void TrackStack::paintEvent(QPaintEvent* event)
{
    if (!isVisible()) { return; }
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;

    QPainter dc(this);
    dc.fillRect(rect(), QColor(255,200,0));

    dc.setRenderHint(QPainter::Antialiasing);

    if (m_bHovered || m_bFocused)
    {
        QColor borderColor = m_bFocused ? QColor(36,36,36)
                                        : QColor(72,72,72);

        auto r = QRect(
            m_trackWidget->mapTo(this, QPoint(0, 0)),
            m_trackWidget->size()
            );

        dc.setBrush(Qt::NoBrush);
        dc.setPen(QPen(borderColor,m_margin));
        auto r2 = QRect(
            r.x() - m_margin/2,
            r.y() - m_margin/2,
            r.width() + m_margin,
            r.height() + m_margin);
        dc.drawRoundedRect(r2, m_radius, m_radius);

        // DE_WARN("m_radius = ",m_radius,", m_margin = ",m_margin)
    }

    // dc.setBrush(Qt::NoBrush);
    // dc.setPen(QPen(QColor(255,0,0)));
    // dc.drawRect(rect().adjusted(1,1,-1,-1));
}

bool TrackStack::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::Wheel)
        return true; // block scrolling

    return QWidget::eventFilter(obj, event);
}

void TrackStack::focusInEvent(QFocusEvent* event)
{
    m_bFocused = true;
    update();
    QWidget::focusInEvent(event);
}
void TrackStack::focusOutEvent(QFocusEvent* event)
{
    m_bFocused = false;
    update();
    QWidget::focusOutEvent(event);
}

void TrackStack::enterEvent( QEnterEvent* event )
{
    m_bHovered = true;
    setFocus(Qt::MouseFocusReason);
    update();
    QWidget::enterEvent( event );
}

void TrackStack::leaveEvent( QEvent* event )
{
    m_bHovered = false;
    update();
    QWidget::leaveEvent( event );
}

void TrackStack::setAudioOnly(bool bAudioOnly)
{
    m_bAudioOnly = bAudioOnly;
    m_audioMeter->setVisible( bAudioOnly );
    m_midiMeter->setVisible( !bAudioOnly );
    update();
}
