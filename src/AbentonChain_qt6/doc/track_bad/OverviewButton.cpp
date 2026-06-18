#include "OverviewButton.h"
#include <QResizeEvent>
#include "App.h"
#include "Footer.h"

QPixmap
OverviewButton::createTextPixmap(int maxHeight, QString text, QColor textColor, QColor fillColor, QFont font)
{
    QFontMetrics fm(font);
    const QRect r_text = fm.tightBoundingRect(text);

    const int w = r_text.width();
    const int h = r_text.height();

    QPixmap pix(w, h);
    pix.fill(fillColor);

    QPainter dc;
    if (dc.begin(&pix))
    {
        dc.setRenderHint(QPainter::Antialiasing);
        dc.setPen(QPen(textColor));
        dc.setBrush(Qt::NoBrush);

        QRect r = pix.rect();
        dc.drawText(r, Qt::AlignCenter, text, &r);
        dc.end();
    }

    return pix;
}

OverviewButton::OverviewButton(QWidget* parent )
    : QWidget(parent)
    , m_btnShow{ nullptr }
    , m_btnName{ nullptr }
{
    setObjectName( "Footer" );
    setContentsMargins( 0,0,0,0 );
    setMouseTracking( true );
    setStyleSheet("background: transparent;"); // border:none;

    m_btnShow = new PixButton(this);
    m_btnShow->setCheckable(true);
    m_btnShow->setChecked(false);

    m_btnName = new PixButton(this);
    m_btnName->setCheckable(true);
    m_btnName->setChecked(false);

    connect(m_btnShow, &PixButton::toggled, this,
            [&] (bool checked) { emit sig_show(checked); });

    connect(m_btnName, &PixButton::toggled, this,
            [&] (bool checked) { emit sig_combo(checked); });

    applySkin();
}

int OverviewButton::computeBestWidth() const
{
    int w = m_btnShow ? m_btnShow->width() : 0;
    w += 1;
    if (m_btnName && m_btnName->isVisible())
    {
        w += m_btnName->width();
    }
    return w + m_pix.width();
}

void OverviewButton::setNameText(QString name)
{
    m_nameText = name;
    //static_cast<Footer*>(parentWidget())->updateLayout();
    updateLayout();
}

void OverviewButton::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    m_zoom = skin.zoom;
    m_radius = (8 * skin.zoom) / 100;
    m_windowColor = skin.windowColor;
    m_panelColor = skin.panelColor;
    m_textColor = skin.textColor;
    m_activeColor = skin.symbolColorActive;

    const int b = (48 * skin.zoom) / 100;
    auto pixShow = createArrowRight(b,b,m_radius,m_windowColor,m_panelColor,m_textColor);
    auto pixHide = createArrowUp(b,b,m_radius,m_windowColor,m_activeColor,m_textColor);
    m_btnShow->setPixmaps(pixHide,pixShow);

    if (m_nameText.isEmpty())
    {
        auto pix = QPixmap();
        m_btnName->setVisible(false);
        m_btnName->setPixmaps(pix,pix);
    }
    else
    {
        auto pix = createTextPixmap(b,m_nameText,m_textColor,Qt::white,font());
        m_btnName->setPixmaps(pix,pix);
        m_btnName->setVisible(true);
    }


    updateLayout();
}

void OverviewButton::updateLayout()
{
    // const int w = width();
    // const int h = height();

    int x = 0;
    int bw = m_btnShow->width();
    int bh = m_btnShow->height();
    m_btnShow->setGeometry(x,0,bw,bh);
    x += bw + 1;

    int nw = m_btnName->width();
    int nh = m_btnName->height();
    m_btnName->setGeometry(x,0,nw,nh);
    x += nw;

    m_posScrollbar = x;
    update();
}

void OverviewButton::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    if (w < 1) return;
    if (h < 1) return;
    updateLayout();
}

void OverviewButton::paintEvent(QPaintEvent* e)
{
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;

    //std::cout << "w = " << w << ", h = " << h << std::endl;

    QPainter dc( this );
    //dc.fillRect( rect(), m_windowColor );

    if (!m_pix.isNull())
    {
        dc.drawPixmap(m_posScrollbar,0,m_pix);
    }

    // //m_fillColor( 200,200,200 )
    // //m_textColor( 46,56,66 )

    // QFont m_font = QFont("FontAwesome", 10, QFont::Normal, false );
    // m_font.setHintingPreference( QFont::PreferFullHinting );
    // m_font.setKerning( true );
    // m_font.setStyleStrategy( QFont::PreferAntialias );

    // dc.setFont( m_font );
    // dc.setPen( QPen( m_textColor ) );
    // dc.setBrush( Qt::NoBrush );

    // QRect r_longText = m_rcLongText.adjusted(10,2,-10,-2);
    // dc.drawText( r_longText,
    //              Qt::AlignVCenter | Qt::AlignLeft,
    //              m_longText,
    //              &r_longText );

    // m_clipOverview.draw(dc);
    // m_trackOverview.draw(dc);
}

void OverviewButton::setOverviewPixmap(QPixmap pix, int visibleWidth, int totalWidth, int xPos)
{
    if (pix.isNull()) { return; }
    m_pix = pix;
    m_viewWidth = visibleWidth;
    m_viewTotal = totalWidth;
    m_viewPos = xPos;
    static_cast<Footer*>(parentWidget())->updateLayout();
}

QPixmap OverviewButton::createArrowRight(int w, int h, int r,
    QColor windowColor, QColor panelColor, QColor symbolColor)
{
    float fx = 18.f / 48.f;

    int sx = std::lround(fx * w);


    auto SVG_createArrowRight = [](int w, int h, QColor symbolColor) -> QPixmap
    {
        auto s = QString(R"(
    <svg width="%1" height="%2" viewBox="0 0 %1 %2" xmlns="http://www.w3.org/2000/svg" >
        <path d="M0,0 L%1,%3 L0,%2z" fill="%4" />
    </svg>
    )")
        .arg(w)  // %1
        .arg(h)  // %2
        .arg(h/2)  // %3
        .arg(toSvg(symbolColor)) // %4
        ;
        return mkSvg(s,w,h);
    };

    auto symbol = SVG_createArrowRight(sx,sx,symbolColor);

    QPixmap pm(w, h);
    pm.fill(windowColor);

    QPainter dc;
    if (dc.begin(&pm))
    {
        dc.setRenderHint(QPainter::Antialiasing);
        dc.setPen( Qt::NoPen );
        dc.setBrush( QBrush( panelColor ) );
        dc.drawRoundedRect(0,0,w,h, r,r );
        dc.drawRect(w/2,0,w,h);

        int W = symbol.width();
        int H = symbol.height();
        dc.drawPixmap((w-W)/2,(h-H)/2,symbol);

        dc.end();
    }

    return pm;
}

QPixmap OverviewButton::createArrowUp(int w, int h, int r,
    QColor windowColor, QColor panelColor, QColor symbolColor)
{
    float fx = 18.f / 48.f;

    int sx = std::lround(fx * w);

    auto SVG_createArrowUp = [](int w, int h, QColor symbolColor) -> QPixmap
    {
        auto s = QString(R"(
    <svg width="%1" height="%2" viewBox="0 0 %1 %2" xmlns="http://www.w3.org/2000/svg" >
        <path d="M%3,0 L%1,%2 L0,%2z" fill="%4" />
    </svg>
    )")
        .arg(w)  // %1
        .arg(h)  // %2
        .arg(w/2)  // %3
        .arg(toSvg(symbolColor)) // %4
        ;
        return mkSvg(s,w,h);
    };

    auto symbol = SVG_createArrowUp(sx,sx,symbolColor);

    QPixmap pm(w, h);
    pm.fill(windowColor);

    QPainter dc;
    if (dc.begin(&pm))
    {
        dc.setRenderHint(QPainter::Antialiasing);
        dc.setPen( Qt::NoPen );
        dc.setBrush( QBrush( panelColor ) );
        dc.drawRoundedRect(0,0,w,h, r,r );
        dc.drawRect(w/2,0,w,h);

        int W = symbol.width();
        int H = symbol.height();
        dc.drawPixmap((w-W)/2,(h-H)/2,symbol);

        dc.end();
    }

    return pm;
}


/*
void Footer::on_btnShowMidiKeyboard( bool checked )
{
    emit sig_showMidiKeyboard(checked);
}
void Footer::on_btnShowQuickHelp( bool checked )
{
    emit sig_showQuickHelp(checked);
}
void Footer::on_btnShowClipOverview( bool checked )
{
    emit sig_showClipEditor(checked);
}
void Footer::on_btnShowTrackOverview( bool checked )
{
    emit sig_showTrackEditor(checked);
}
void Footer::on_btnShowDetails( bool checked )
{
    emit sig_showArrangement(checked);
}
void Footer::mouseReleaseEvent( QMouseEvent* event )
{
   LiveSkin & skin = m_skin;
   int mx = event->x();
   int my = event->y();
   if ( isMouseOverRect( mx,my, m_rcClipOverviewPanel ) )
   {

   }

   if ( isMouseOverRect( mx,my, m_rcTrackOverviewPanel ) )
   {

   }

   QWidget::mouseReleaseEvent( event );
}

void
Footer::focusInEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusInEvent( event );
}

void
Footer::focusOutEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusOutEvent( event );
}

void
Footer::enterEvent( QEnterEvent* event )
{
   QWidget::enterEvent( event );
}

void
Footer::leaveEvent( QEvent* event )
{
   QWidget::leaveEvent( event );
}

void
Footer::mouseMoveEvent( QMouseEvent* event )
{
//   m_mouseX = event->x();
//   m_mouseY = event->y();
   QWidget::mouseMoveEvent( event );
}

void
Footer::wheelEvent( QWheelEvent* event )
{
   //int wheel = event->angleDelta().y();
   QWidget::wheelEvent( event );
}

void
Footer::mousePressEvent( QMouseEvent* event )
{
   QWidget::mousePressEvent( event );
}

void
Footer::mouseReleaseEvent( QMouseEvent* event )
{

   QWidget::mouseReleaseEvent( event );
}

void
Footer::keyPressEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyPress(",event->key(),")")
   QWidget::keyPressEvent( event );
}

void
Footer::keyReleaseEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyRelease(",event->key(),")")
   QWidget::keyReleaseEvent( event );
}
*/
