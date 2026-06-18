#include "Footer.h"
#include <QResizeEvent>
#include "App.h"

Footer::Footer(QWidget* parent )
   : QWidget(parent)
{
    setObjectName( "Footer" );
    setContentsMargins(8,8,8,8);
    setMouseTracking( true );

    m_helpFont = QFont("FontAwesome", 10, QFont::Normal, false );
    m_helpFont.setHintingPreference( QFont::PreferFullHinting );
    m_helpFont.setKerning( true );
    m_helpFont.setStyleStrategy( QFont::PreferAntialias );

    m_clipName.text = "1 - Clip";
    m_trackName.text = "1 - Audio";

    applySkin();
}

int Footer::computeBestHeight() const { return ((48 + 2*8) * m_zoom) / 100; }

void Footer::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    m_zoom = skin.zoom;
    m_windowColor = skin.windowColor;
    m_panelColor = skin.panelColor;
    m_textColor = skin.textColor;
    m_activeColor = skin.symbolColorActive;
    const int b = (48 * skin.zoom) / 100;
    m_padding = (8 * skin.zoom) / 100;
    m_radius = m_padding;

    m_arrowRight = PIX_createArrowRight(b,b,m_radius,m_windowColor,m_panelColor,m_textColor);
    m_arrowUp = PIX_createArrowUp(b,b,m_radius,m_windowColor,m_activeColor,m_textColor);

    m_quickHelp.on = PIX_createQuickHelp(b,b,m_activeColor,m_textColor);
    m_quickHelp.off = PIX_createQuickHelp(b,b,m_panelColor,m_textColor);

    m_midiKeyboard.on = PIX_createMidiKeyboard(2*b,b,Qt::white, Qt::black, Qt::gray, Qt::gray);
    m_midiKeyboard.off = PIX_createMidiKeyboard(2*b,b,Qt::white, Qt::black, Qt::gray, Qt::gray);

    int pixelSize = computeBestFontHeight(m_helpFont,b - m_padding);
    m_helpFont.setPixelSize(pixelSize);

    m_clipName.pix = PIX_createFromText(m_padding,m_padding/2,m_clipName.text,Qt::white,m_textColor,m_helpFont);
    m_trackName.pix = PIX_createFromText(m_padding,m_padding/2,m_trackName.text,Qt::white,m_textColor,m_helpFont);

    m_details.on = PIX_createDetails(b,b,QColor(235,181,22),QColor(255,220,55));
    m_details.off = PIX_createDetails(b,b,m_panelColor,m_textColor);

    updateLayout();
}

void
Footer::updateLayout()
{
    const int w = width();
    const int h = height();

    const int hInner = (48 * m_zoom) / 100;
    m_padding = (height() - hInner) / 2;

    int wHelp = m_quickHelp.on.width();
    int hHelp = m_quickHelp.on.height();

    int wMidi = m_midiKeyboard.on.width();
    int hMidi = m_midiKeyboard.on.height();

    int wTrack = m_arrowUp.width() + 1 + m_trackName.rc.width() + m_trackScroll.rc.width();
    int wClip = m_arrowUp.width() + 1 + m_clipName.rc.width() + m_clipScroll.rc.width();

    int xHelp = m_padding;
    int xMidi = xHelp + wHelp + m_padding;
    int xLong = xMidi + wMidi + m_padding;
    int xLast = w - 1 - m_padding;
    int wDeta = m_details.rc.width();
    int xDeta = xLast - wDeta;
    int xTrack = xDeta - wDeta - m_padding - wTrack;
    int xClip = xTrack - m_padding - wClip;
    int wLong = xClip - xLong - m_padding;

    m_quickHelp.rc = QRect(xHelp, m_padding, wHelp, hHelp);
    m_midiKeyboard.rc = QRect(xMidi,m_padding, wMidi, hMidi);
    m_longText.rc = QRect(xLong, m_padding, wLong, hInner);

    int x = xClip;
    m_clipShow.rc = QRect(x,m_padding, m_arrowUp.width(), m_arrowUp.height());
    x += m_arrowUp.width()+1;
    m_clipName.rc = QRect(x,m_padding, m_clipName.pix.width(), m_clipName.pix.height());
    x += m_clipName.pix.width();
    m_clipScroll.rc = QRect(x,m_padding, m_clipScroll.pix.width(), m_clipScroll.pix.height());

    x = xTrack;
    m_trackShow.rc = QRect(x,m_padding, m_arrowUp.width(), m_arrowUp.height());
    x += m_arrowUp.width()+1;
    m_trackName.rc = QRect(x,m_padding, m_trackName.pix.width(), m_trackName.pix.height());
    x += m_trackName.pix.width();
    m_trackScroll.rc = QRect(x,m_padding, m_trackScroll.pix.width(), m_trackScroll.pix.height());

    m_details.rc = QRect(xDeta,m_padding, m_details.on.width(), m_details.on.height());

    update();
}

void
Footer::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    if (w < 1) return;
    if (h < 1) return;
    updateLayout();
}

void Footer::paintEvent(QPaintEvent* e)
{
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;

    //std::cout << "w = " << w << ", h = " << h << std::endl;

    QPainter dc( this );
    dc.fillRect( rect(), m_windowColor );

    //<QuickHelp>
    dc.drawPixmap(m_quickHelp.rc, m_bHelpVisible ? m_quickHelp.on : m_quickHelp.off);
    //</QuickHelp>

    //<MidiKeyboard>
    dc.drawPixmap(m_midiKeyboard.rc, m_bMidiVisible ? m_midiKeyboard.on : m_midiKeyboard.off);
    //</MidiKeyboard>

    //<LongText>
    dc.setPen( Qt::NoPen );
    dc.setBrush( QBrush( m_panelColor ) );
    dc.drawRoundedRect( m_longText.rc, m_radius,m_radius );

    dc.setFont( m_helpFont );
    dc.setPen( QPen( m_textColor ) );
    dc.setBrush( Qt::NoBrush );

    QRect r_longText = m_longText.rc.adjusted(10,2,-10,-2);
    dc.drawText( r_longText,
                 Qt::AlignVCenter | Qt::AlignLeft,
                 m_longText.text,
                 &r_longText );

    //</LongText>

    //<ClipOverview>
    dc.drawPixmap(m_clipShow.rc, m_bClipVisible ? m_arrowUp : m_arrowRight);
    dc.drawPixmap(m_clipName.rc, m_clipName.pix);
    dc.drawPixmap(m_clipScroll.rc, m_clipScroll.pix);
    //</ClipOverview>

    //<TrackOverview>
    dc.drawPixmap(m_trackShow.rc, m_bTrackVisible ? m_arrowUp : m_arrowRight);
    dc.drawPixmap(m_trackName.rc, m_trackName.pix);
    dc.drawPixmap(m_trackScroll.rc, m_trackScroll.pix);
    //</TrackOverview>

    //<Details>
    dc.drawPixmap(m_details.rc, m_bArraVisible ? m_details.on : m_details.off);
    //</Details>
}

void
Footer::setTrackOverview(QPixmap pix, int visibleWidth, int totalWidth, int xPos)
{
    m_trackScroll.pix = pix;
    m_trackScroll.view = visibleWidth;
    m_trackScroll.total = totalWidth;
    m_trackScroll.pos = xPos;
    updateLayout();
}

bool Footer::event(QEvent* e)
{
    if (e->type() == QEvent::LayoutRequest)
    {
        DE_BENNI("Got QEvent::LayoutRequest")
        //updateLayout();
        return true;
    }

    return QWidget::event(e);
}

void Footer::mousePressEvent( QMouseEvent* event )
{
    auto mp = event->pos();

    if (isMouseOver(mp,m_quickHelp.rc))
    {
        m_bHelpVisible = !m_bHelpVisible;
        emit sig_showQuickHelp(m_bHelpVisible);
        update();
    }
    else if (isMouseOver(mp,m_midiKeyboard.rc))
    {
        m_bMidiVisible = !m_bMidiVisible;
        emit sig_showMidiKeyboard(m_bMidiVisible);
        update();
    }
    else if (isMouseOver(mp,m_clipShow.rc))
    {
        m_bClipVisible = !m_bClipVisible;
        emit sig_showClipEditor(m_bClipVisible);
        update();
    }
    else if (isMouseOver(mp,m_trackShow.rc))
    {
        m_bTrackVisible = !m_bTrackVisible;
        emit sig_showTrackEditor(m_bTrackVisible);
        update();
    }
    else if (isMouseOver(mp,m_details.rc))
    {
        m_bArraVisible = !m_bArraVisible;
        emit sig_showArrangement(m_bArraVisible);
        update();
    }

    QWidget::mousePressEvent( event );
}

void Footer::mouseReleaseEvent( QMouseEvent* event )
{

   QWidget::mouseReleaseEvent( event );
}


// static
QPixmap Footer::PIX_createFromText(int padd_x, int padd_y, QString text, QColor fillColor, QColor textColor, QFont font)
{
    QFontMetrics fm(font);
    const QRect r_text = fm.boundingRect(text);

    const int w = r_text.width();
    const int h = r_text.height();

    QPixmap pix(w + padd_x * 2, h + padd_y * 2);
    pix.fill(fillColor);

    QPainter dc;
    if (dc.begin(&pix))
    {
        dc.setFont(font);
        dc.setRenderHint(QPainter::Antialiasing);
        dc.setPen(QPen(textColor));
        dc.setBrush(Qt::NoBrush);

        QRect r = pix.rect();
        dc.drawText(r, Qt::AlignCenter, text, &r);
        dc.end();
    }

    return pix;
}

// static
QPixmap Footer::PIX_createQuickHelp(int w, int h, const QColor& fillColor, const QColor& lineColor)
{
    if (w<1 || h<1) return QPixmap();

    auto mkSvg_Power = [](int w, int h,
            const QColor& fillColor, const QColor& lineColor) -> QPixmap
    {
        auto s = QString(R"(
<svg width="30" height="30" viewBox="0 0 30 30" xmlns="http://www.w3.org/2000/svg" >
<circle cx="15" cy="15" r="12" fill="%1" stroke="%2" stroke-width="2" />
<line x1="15" y1="6" x2="15" y2="9" stroke="%2" stroke-width="3" />
<line x1="15" y1="11" x2="15" y2="24" stroke="%2" stroke-width="3" />
</svg>
)")
        .arg(toSvg(fillColor))  // %1
        .arg(toSvg(lineColor))  // %2
        ;
        return mkSvg(s,w,h);
    };

    return mkSvg_Power(w,h, fillColor, lineColor );
}

// static
QPixmap Footer::PIX_createMidiKeyboard(int w, int h, QColor whiteColor, QColor blackColor, QColor redColor, QColor blueColor)
{
    if (w<1 || h<1) return QPixmap();

    QPixmap pm(w, h);
    pm.fill(Qt::transparent);

    auto drawBar = [] (QPainter& dc, int x, int y, int w, int h,
            QColor fillColor, QColor lineColor)
    {
        dc.setPen(Qt::NoPen);
        dc.setBrush(QBrush(fillColor));
        dc.drawRect(x,y,w,h);

        dc.setPen(QPen(lineColor));
        dc.setBrush(Qt::NoBrush);
        dc.drawRect(x,y,w-1,h-1);
    };

    QPainter dc;
    if (dc.begin(&pm))
    {
        int ww = w / 7;
        int cw = w - 6 * ww;
        int bw = 2*ww/3;
        int bh = 5*h/8;

        int x = 0;
        int y = 0;
        drawBar(dc,0,0,cw,h,whiteColor,redColor);
        x += cw;

        for (int i = 0; i < 6; ++i)
        {
            drawBar(dc,x,y,ww,h,whiteColor,redColor);
            x += ww;
        }

        x = cw - bw/2;
        drawBar(dc,x,y,bw,bh,blackColor,blueColor);
        x += ww;
        drawBar(dc,x,y,bw,bh,blackColor,blueColor);
        x += 2*ww;
        drawBar(dc,x,y,bw,bh,blackColor,blueColor);
        x += ww;
        drawBar(dc,x,y,bw,bh,blackColor,blueColor);
        x += ww;
        drawBar(dc,x,y,bw,bh,blackColor,blueColor);
        // x += ww;
    }

    //pm.save("A_createMidiKeyboard.png");
    return pm;
}

// static
QPixmap Footer::PIX_createArrowRight(int w, int h, int r, QColor windowColor, QColor panelColor, QColor symbolColor)
{
    if (w<1 || h<1) return QPixmap();

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
        dc.setPen(Qt::NoPen);
        dc.setBrush(QBrush(panelColor));
        dc.drawRoundedRect(0,0,w,h,r,r);
        dc.drawRect(w/2,0,w,h);

        int W = symbol.width();
        int H = symbol.height();
        dc.drawPixmap((w-W)/2,(h-H)/2,symbol);

        dc.end();
    }

    return pm;
}

// static
QPixmap Footer::PIX_createArrowUp(int w, int h, int r, QColor windowColor, QColor panelColor, QColor symbolColor)
{
    if (w<1 || h<1) return QPixmap();

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
        dc.setPen(Qt::NoPen);
        dc.setBrush(QBrush(panelColor));
        dc.drawRoundedRect(0,0,w,h,r,r);
        dc.drawRect(w/2,0,w,h);

        int W = symbol.width();
        int H = symbol.height();
        dc.drawPixmap((w-W)/2,(h-H)/2,symbol);

        dc.end();
    }

    return pm;
}

// static
QPixmap Footer::PIX_createDetails(int w, int h, QColor fillColor, QColor lineColor)
{
    if (w<1 || h<1) return QPixmap();

    auto drawBar = [] (QPainter& dc, int x, int y, int w, int h, QColor fillColor, QColor lineColor)
    {
        dc.setPen(Qt::NoPen);
        dc.setBrush(QBrush(fillColor));
        dc.drawRect(x,y,w,h);

        dc.setPen(QPen(lineColor));
        dc.setBrush(Qt::NoBrush);
        dc.drawRect(x,y,w-1,h-1);
    };

    QPixmap pm(w, h);
    pm.fill(Qt::transparent);

    QPainter dc;
    if (dc.begin(&pm))
    {
        int s = 8;
        int bw = (w - 4*s) / 5;

        int x = 0;
        int y = 2;
        drawBar(dc,x,y,bw,h-y-1,fillColor,lineColor);
        x += bw + s;
        y = 12;
        drawBar(dc,x,y,bw,h-y-1,fillColor,lineColor);
        x += bw + s;
        y = 5;
        drawBar(dc,x,y,bw,h-y-1,fillColor,lineColor);
        x += bw + s;
        y = 14;
        drawBar(dc,x,y,bw,h-y-1,fillColor,lineColor);
        x += bw + s;
        y = 8;
        drawBar(dc,x,y,bw,h-y-1,fillColor,lineColor);
        //x += bw + s;
    }

    return pm;
}
