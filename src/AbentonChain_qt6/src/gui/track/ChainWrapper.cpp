#include "gui/track/ChainWrapper.h"
#include "App.h"
#include "gui/Skin.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

ChainWrapper::ChainWrapper(QWidget* parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setContentsMargins(0,8,8,8);
    setStyleSheet("background: transparent;");

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setContentsMargins(0,0,0,0);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setAttribute(Qt::WA_NoSystemBackground, true);
    m_scrollArea->setStyleSheet("background: transparent;");

    if (m_scrollArea->viewport())
    {
        //m_scrollArea->viewport()->setFrameShape(QFrame::NoFrame);
        m_scrollArea->viewport()->setAttribute(Qt::WA_NoSystemBackground, true);
        m_scrollArea->viewport()->setStyleSheet("background: transparent;");
    }
    else
    {
        DE_ERROR("No viewport()")
    }

    // verticalScrollBar()->setStyleSheet("QScrollBar { width: 0px; }");
    // horizontalScrollBar()->setStyleSheet("QScrollBar { height: 0px; }");

    // ✔ Connect your scrollbar to the scroll area
    //connect(myScroll, &QSlider::valueChanged, scrollArea->verticalScrollBar(),
    //        &QScrollBar::setValue);

    // ✔ Sync the scroll area back to your custom scrollbar
    //connect(scrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
    //        myScroll, &QSlider::setValue);

    auto track = App::instance()->getAudioCentral().getTrack(0);
    m_track = new Track(track, this);

    m_scrollArea->setWidget(m_track);

    m_midiMeter = new MidiMeter(this);
    m_audioMeter = new AudioMeter(this);
    auto h = new QHBoxLayout;
    h -> setContentsMargins(0,0,0,0);
    h -> setSpacing(0);
    h -> addWidget(m_midiMeter, 0, Qt::AlignVCenter);
    h -> addWidget(m_audioMeter, 0, Qt::AlignVCenter);
    h -> addWidget(m_scrollArea,1);
    setLayout(h);

    connect(App::instance(), &App::skinChanged,
            this, &ChainWrapper::applySkin);

    setAudioOnly(false);

    applySkin();

    m_scrollArea->viewport()->installEventFilter(this);
}

bool ChainWrapper::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::Wheel)
        return true; // block scrolling

    return QWidget::eventFilter(obj, event);
}

void ChainWrapper::setAudioOnly(bool bAudioOnly)
{
    m_bAudioOnly = bAudioOnly;
    m_audioMeter->setVisible( bAudioOnly );
    m_midiMeter->setVisible( !bAudioOnly );
    update();
}

void ChainWrapper::applySkin()
{
    m_midiMeter->applySkin();
    m_audioMeter->applySkin();
    m_track->applySkin();

    //m_scrollArea->setSizeAdjustPolicy(0,m_track->height());
    m_scrollArea->setMinimumSize(0,m_track->height());

    const auto& skin = App::instance()->currentSkin();
    m_windowColor = skin.windowColor;
    m_margin = (m_baseMargin * skin.zoom) / 100;
    m_radius = ((m_baseRadius-2) * skin.zoom) / 100;

    updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}

void ChainWrapper::paintEvent(QPaintEvent* event)
{
    if (!isVisible())
    {
        return;
    }

    QPainter dc(this);
    dc.setRenderHint(QPainter::Antialiasing);

    if (m_bHovered || m_bFocused)
    {
        QColor borderColor = m_bFocused ? QColor(36,36,36)
                                        : QColor(72,72,72);

        auto r = QRect(
            m_scrollArea->mapTo(this, QPoint(0, 0)),
            m_scrollArea->size()
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

void ChainWrapper::focusInEvent(QFocusEvent* event)
{
    m_bFocused = true;
    update();
    QWidget::focusInEvent(event);
}
void ChainWrapper::focusOutEvent(QFocusEvent* event)
{
    m_bFocused = false;
    update();
    QWidget::focusOutEvent(event);
}

void ChainWrapper::enterEvent( QEnterEvent* event )
{
    m_bHovered = true;
    update();
    QWidget::enterEvent( event );
}

void ChainWrapper::leaveEvent( QEvent* event )
{
    m_bHovered = false;
    update();
    QWidget::leaveEvent( event );
}
