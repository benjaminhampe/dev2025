#include "Footer.h"
#include <QResizeEvent>
#include "App.h"

Footer::Footer(QWidget* parent )
   : QWidget(parent)
   , m_hasFocus( false )
{
    setObjectName( "Footer" );
    setContentsMargins( 0,0,0,0 );
    setMouseTracking( true );

    m_btnQuickHelp = new PixButton(this);
    m_btnMidiKeyboard = new PixButton(this);
    m_btnDetails = new PixButton(this);
    m_longText = QString("LongText");

    m_clipOverview.btnShow = new PixButton(this);
    m_clipOverview.btnName = new PixButton(this);
    m_trackOverview.btnShow = new PixButton(this);
    m_trackOverview.btnName = new PixButton(this);

    connect( m_btnQuickHelp, SIGNAL(toggled(bool)),
            this, SLOT(on_btnShowQuickHelpPanel(bool)) );

    connect( m_btnMidiKeyboard, SIGNAL(toggled(bool)),
            this, SLOT(on_btnShowMidiKeyboard(bool)) );

    connect( m_clipOverview.btnShow, SIGNAL(clicked(bool)),
            this, SLOT(on_btnShowClipOverview(bool)) );

    connect( m_trackOverview.btnShow, SIGNAL(clicked(bool)),
            this, SLOT(on_btnShowTrackOverview(bool)) );

    connect( m_btnDetails, SIGNAL(toggled(bool)),
            this, SLOT(on_btnShowDetailPanel(bool)) );

    //   connect( &m_trackList, SIGNAL(currentTrackIdChanged(int)),
    //           this, SLOT(on_currentTrackIdChanged(int)) );

    // auto h = new QHBoxLayout;
    // h->setContentsMargins(0,0,0,0);
    // h->setSpacing(0);
    // h->addWidget(m_btnShowQuickHelpPanel);
    // h->addWidget(m_longText,1);
    // h->addWidget(m_btnShowMidiKeyboard);
    // h->addWidget(m_btnClipOverview);
    // h->addWidget(m_btnTrackOverview);
    // h->addWidget(m_btnShowDetailPanel);
    // setLayout(h);

    setMinimumHeight(68);
    setMaximumHeight(68);
    applySkin();
}

Footer::~Footer()
{}

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

void SVG_createQuickHelp(PixButton* btn, int w, int h)
{
    //const int outlineWidth = (m_baseOutlineWidth * skin.zoom) / 100;
    const auto lineColor = QColor(79,79,79);
    const auto onColor = QColor(255,181,1);
    const auto offColor = QColor(207,207,207);
    // const auto textColor = skin.textColor;

    auto mkSvg_Power = [](int w, int h,
        const QColor& fillColor,
        const QColor& lineColor) -> QPixmap
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

    auto act = mkSvg_Power(w,h, onColor, lineColor );
    auto dea = mkSvg_Power(w,h, offColor, lineColor );

    btn->setPixmaps(act,dea);
}

inline void
drawRect( QPainter & dc, int x, int y, int w, int h, QColor const & color )
{
   if ( w < 1 || h < 1 ) return;
   dc.setPen( Qt::NoPen );
   dc.setBrush( QBrush( color ) );
   dc.drawRect( QRect(x,y,w,h) );
}

inline void
drawFillRect( QPainter & dc, const QRect& pos, QColor const & fillColor )
{
    int w = pos.width();
    int h = pos.height();
    if (w < 1 || h < 1) return;
    int x = pos.x();
    int y = pos.y();
    dc.setPen( Qt::NoPen );
    dc.setBrush( QBrush( fillColor ) );
    dc.drawRect( QRect(x,y,w,h) );
}

inline void
drawLineRect( QPainter & dc, const QRect& pos, QColor const & lineColor )
{
    int w = pos.width();
    int h = pos.height();
    if (w < 1 || h < 1) return;
    int x = pos.x();
    int y = pos.y();
    dc.setPen( QPen(lineColor) );
    dc.setBrush( Qt::NoBrush );
    dc.drawRect( QRect(x,y,w-1,h-1) );
}

void PIX_createMidiKeyboard(PixButton* btn, int w, int h)
{
    //const int outlineWidth = (m_baseOutlineWidth * skin.zoom) / 100;
    const auto lineColor = QColor(79,79,79);
    const auto onColor = QColor(255,181,1);
    const auto offColor = QColor(207,207,207);
    // const auto textColor = skin.textColor;

    QPixmap pm(w, h);
    pm.fill(Qt::transparent);

    int ww = w / 7;
    int cw = w - 6 * ww;

    int bw = 2*ww/3;
    int bh = 5*h/8;

    {
        QPainter dc(&pm);

        int x = 0;
        int y = 0;
        drawFillRect(dc,QRect(0,0,cw,h),Qt::white);
        drawLineRect(dc,QRect(0,0,cw,h),Qt::red);
        x += cw;

        for (int i = 0; i < 6; ++i)
        {
            drawFillRect(dc,QRect(x,y,ww,h),Qt::white);
            drawLineRect(dc,QRect(x,y,ww,h),Qt::red);
            x += ww;
        }

        x = cw - bw/2;
        drawFillRect(dc,QRect(x,y,bw,bh),Qt::black);
        drawLineRect(dc,QRect(x,y,bw,bh),Qt::blue);
        x += ww;
        drawFillRect(dc,QRect(x,y,bw,bh),Qt::black);
        drawLineRect(dc,QRect(x,y,bw,bh),Qt::blue);
        x += 2*ww;
        drawFillRect(dc,QRect(x,y,bw,bh),Qt::black);
        drawLineRect(dc,QRect(x,y,bw,bh),Qt::blue);
        x += ww;
        drawFillRect(dc,QRect(x,y,bw,bh),Qt::black);
        drawLineRect(dc,QRect(x,y,bw,bh),Qt::blue);
        x += ww;
        drawFillRect(dc,QRect(x,y,bw,bh),Qt::black);
        drawLineRect(dc,QRect(x,y,bw,bh),Qt::blue);
        // x += ww;
    }

    pm.save("A_createMidiKeyboard.png");

    btn->setPixmaps(pm,pm);
}

void PIX_createClipOverview(PixButton* btn, int w, int h)
{
    //const int outlineWidth = (m_baseOutlineWidth * skin.zoom) / 100;
    const auto lineColor = QColor(79,79,79);
    const auto onColor = QColor(255,181,1);
    const auto offColor = QColor(207,207,207);
    // const auto textColor = skin.textColor;

    QPixmap pm(w, h);
    pm.fill(Qt::transparent);

    {
        QPainter dc(&pm);

        drawFillRect(dc,QRect(0,0,w,h),Qt::white);
        drawLineRect(dc,QRect(0,0,w,h),Qt::black);
    }

    btn->setPixmaps(pm,pm);
}


void PIX_createTrackOverview(PixButton* btn, int w, int h)
{
    //const int outlineWidth = (m_baseOutlineWidth * skin.zoom) / 100;
    const auto lineColor = QColor(79,79,79);
    const auto onColor = QColor(255,181,1);
    const auto offColor = QColor(207,207,207);
    // const auto textColor = skin.textColor;

    QPixmap pm(w, h);
    pm.fill(Qt::transparent);

    {
        QPainter dc(&pm);

        drawFillRect(dc,QRect(0,0,w,h),Qt::white);
        drawLineRect(dc,QRect(0,0,w,h),Qt::black);
    }

    btn->setPixmaps(pm,pm);
}

void PIX_createDetails(PixButton* btn, int w, int h)
{
    //const int outlineWidth = (m_baseOutlineWidth * skin.zoom) / 100;
    //const auto lineColor = QColor(79,79,79);
    const auto onColor = QColor(255,181,1);
    const auto offColor = QColor(207,207,207);
    // const auto textColor = skin.textColor;

    QPixmap pm(w, h);
    pm.fill(Qt::transparent);

    QColor fillColor(235,181,22);
    QColor lineColor(255,220,55);

    int s = 8;
    int bw = (w - 4*s) / 5;
    {
        QPainter dc(&pm);

        int x = 0;
        int y = 2;
        drawFillRect(dc,QRect(x,y,bw,h-y-1),fillColor);
        drawLineRect(dc,QRect(x,y,bw,h-y-1),lineColor); x += bw + s;
        y = 12;
        drawFillRect(dc,QRect(x,y,bw,h-y-1),fillColor);
        drawLineRect(dc,QRect(x,y,bw,h-y-1),lineColor); x += bw + s;
        y = 5;
        drawFillRect(dc,QRect(x,y,bw,h-y-1),fillColor);
        drawLineRect(dc,QRect(x,y,bw,h-y-1),lineColor); x += bw + s;
        y = 14;
        drawFillRect(dc,QRect(x,y,bw,h-y-1),fillColor);
        drawLineRect(dc,QRect(x,y,bw,h-y-1),lineColor); x += bw + s;
        y = 8;
        drawFillRect(dc,QRect(x,y,bw,h-y-1),fillColor);
        drawLineRect(dc,QRect(x,y,bw,h-y-1),lineColor);
        //x += bw + s;
    }

    btn->setPixmaps(pm,pm);
}


void Footer::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    m_windowColor = skin.windowColor;
    m_panelColor = skin.panelColor;
    m_textColor = skin.textColor;
    m_activeColor = skin.symbolColorActive;
    m_radius = (m_baseRadius * skin.zoom) / 100;
    m_padding = (m_basePadding * skin.zoom) / 100;
    m_buttonHeight = (m_baseButtonHeight * skin.zoom) / 100;

    SVG_createQuickHelp(m_btnQuickHelp, m_buttonHeight,m_buttonHeight);
    PIX_createMidiKeyboard(m_btnMidiKeyboard, 2*m_buttonHeight,m_buttonHeight);

    auto pixOff = createArrowRight(48,48,m_windowColor,m_panelColor,m_textColor);
    auto pixOn = createArrowRight(48,48,m_windowColor,m_activeColor,m_textColor);
    m_clipOverview.btnShow->setPixmaps(pixOn,pixOff);

    pixOff = createFromText(0,48,"1-ClipEditor", m_textColor,m_panelColor);
    pixOn = createFromText(0,48,"1-ClipEditor", m_textColor,m_panelColor);
    m_clipOverview.btnName->setPixmaps(pixOn,pixOff);

    pixOff = createArrowRight(48,48,m_windowColor,m_panelColor,m_textColor);
    pixOn = createArrowRight(48,48,m_windowColor,m_activeColor,m_textColor);
    m_trackOverview.btnShow->setPixmaps(pixOn,pixOff);

    pixOff = createFromText(0,48,"1-AudioTrack", m_textColor,m_panelColor);
    pixOn = createFromText(0,48,"1-AudioTrack", m_textColor,m_panelColor);
    m_trackOverview.btnName->setPixmaps(pixOn,pixOff);

    PIX_createDetails(m_btnDetails, 2*m_buttonHeight,m_buttonHeight);

    // QColor m_panelColor(128,128,128);
    // QColor m_contentColor(255,255,255);
    // QColor m_symbolColor(255,128,0);
    // QColor m_focusColor(32,32,32);
    updateLayout();
}

void
Footer::updateLayout()
{
    int w = width();
    int h = height();
    int p = 10;

    int xHelp = p;
    int xMidi = xHelp + m_btnQuickHelp->width() + p;
    int xLong = xMidi + m_btnMidiKeyboard->width() + p;

    int xLast = w - 1 - p;
    int xDeta = xLast - m_btnDetails->width();
    int xTrack = xDeta - p - m_trackOverview.width();
    int xClip = xTrack - p - m_clipOverview.width();
    int wLong = xClip - xLong - p;

    m_btnQuickHelp->move(xHelp,p);
    m_btnMidiKeyboard->move(xMidi,p);
    m_rcLongText = QRect(xLong, p, wLong, h-2*p);
    m_clipOverview.move(xClip,p);
    m_trackOverview.move(xTrack,p);
    m_btnDetails->move(xDeta,p);

    update();
}

void
Footer::resizeEvent( QResizeEvent* event )
{
    updateLayout();
    QWidget::resizeEvent( event );
}

void Footer::paintEvent( QPaintEvent* event )
{
    int w = width();
    int h = height();
    if ( w < 2 ) return;
    if ( h < 2 ) return;

    //std::cout << "w = " << w << ", h = " << h << std::endl;

    QPainter dc( this );
    dc.fillRect( rect(), m_windowColor );

    dc.setPen( Qt::NoPen );
    dc.setBrush( QBrush( m_panelColor ) );
    dc.drawRoundedRect( m_rcLongText, m_radius,m_radius );

    //m_fillColor( 200,200,200 )
    //m_textColor( 46,56,66 )

    QFont m_font = QFont("FontAwesome", 10, QFont::Normal, false );
    m_font.setHintingPreference( QFont::PreferFullHinting );
    m_font.setKerning( true );
    m_font.setStyleStrategy( QFont::PreferAntialias );

    dc.setFont( m_font );
    dc.setPen( QPen( m_textColor ) );
    dc.setBrush( Qt::NoBrush );

    QRect r_longText = m_rcLongText.adjusted(10,2,-10,-2);
    dc.drawText( r_longText,
                 Qt::AlignVCenter | Qt::AlignLeft,
                 m_longText,
                 &r_longText );

    m_clipOverview.draw(dc);
    m_trackOverview.draw(dc);
}

void
Footer::setTrackOverview(QPixmap pix, int visibleWidth, int totalWidth, int xPos)
{
    if (!pix)
    {
        return;
    }

    m_trackOverview.pix = pix;
    m_trackOverview.viewWidth = visibleWidth;
    m_trackOverview.totalWidth = totalWidth;
    m_trackOverview.viewPos = xPos;

    //m_trackOverview->setPixmaps(m_trackOverviewPixmap,m_trackOverviewPixmap);

    updateLayout();
}

QPixmap SVG_createSymbolArrowRight(int w, int h, QColor symbolColor)
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
}

QPixmap Footer::createArrowRight(int w, int h,
    QColor windowColor, QColor panelColor, QColor symbolColor)
{
    // const auto lineColor = QColor(79,79,79);
    // const auto onColor = QColor(255,181,1);
    // const auto offColor = QColor(207,207,207);
    // const auto textColor = skin.textColor;

    auto svgArrow = SVG_createSymbolArrowRight(9,9,symbolColor);

    QPixmap pm(w, h);
    pm.fill(windowColor);

    QPainter dc;
    if (dc.begin(&pm))
    {
        dc.setRenderHint(QPainter::Antialiasing);
        dc.setPen( Qt::NoPen );
        dc.setBrush( QBrush( panelColor ) );
        dc.drawRoundedRect(0,0,w,h, m_radius,m_radius );
        dc.drawRect(w/2,0,w,h);

        int W = svgArrow.width();
        int H = svgArrow.height();
        dc.drawPixmap((w-W)/2,(h-H)/2,svgArrow);

        dc.end();
    }

    return pm;
}

QPixmap Footer::createFromText(int w, int h, QString text, QColor textColor, QColor fillColor)
{
    QFontMetrics fm(font());
    QRect r = fm.tightBoundingRect(text);

    int tw = r.width() + 10;
    int th = r.height() + 2;

    if (w > 0)
    {
        w = std::max(w,tw);
    }
    else
    {
        w = tw;
    }

    if (h > 0)
    {
        h = std::max(h,th);
    }
    else
    {
        h = th;
    }

    QPixmap pm(w, h);
    pm.fill(fillColor);

    QPainter dc;
    if (dc.begin(&pm))
    {
        dc.setRenderHint(QPainter::Antialiasing);
        dc.setPen(QPen(textColor));
        dc.setBrush(Qt::NoBrush);
        dc.drawText(10,(h-th)/2,text);
        dc.end();
    }

    return pm;
}

/*
inline void
drawRoundRectFill( QPainter & dc, QRect const & rect, QColor const & color, int rx = 6, int ry = 6 )
{
   dc.setPen( Qt::NoPen );
   dc.setBrush( QBrush( color ) );
   dc.drawRoundedRect( rect, rx,ry );
}

inline void
drawRoundRectFill( QPainter & dc, int x, int y, int w, int h, QColor const & color, int rx = 6, int ry = 6 )
{
   drawRoundRectFill( dc, QRect(x,y,w,h), color, rx,ry );
}


inline void
drawRectFill( QPainter & dc, int x, int y, int w, int h, QColor const & color )
{
   if ( w < 1 || h < 1 ) return;
   dc.setPen( Qt::NoPen );
   dc.setBrush( QBrush( color ) );
   dc.drawRect( QRect(x,y,w,h) );
}

inline void
drawRectFill( QPainter & dc, QRect const & pos, QColor const & color )
{
   int x = pos.x();
   int y = pos.y();
   int w = pos.width();
   int h = pos.height();
   drawRectFill( dc, x,y,w,h, color );
}

inline void
drawContent( QPainter & dc, QRect pos, QColor const & contentColor )
{
   int x = pos.x();
   int y = pos.y();
   int w = pos.width();
   int h = pos.height();
   if ( w < 2 || h < 2 ) return;

   // Draw background
   drawRectFill( dc, QRect( x, y, w, h ), contentColor );
}


inline void
drawHelpView( QPainter & dc, QRect pos, int titleH, int tdH, QColor titleColor, QColor tdColor, QColor contentColor )
{
   int x = pos.x();
   int y = pos.y();
   int w = pos.width();
   int h = pos.height();
   if ( w < 2 || h < 2 ) return;

   // Draw background
   drawRectFill( dc, QRect( x, y, w, h ), contentColor );

   // Draw title rect
   if ( h > titleH )
   {
      drawRectFill( dc, QRect( x, y, w, titleH ), titleColor );
      if ( h > titleH + tdH )
      {
         drawRectFill( dc, QRect( x, y + titleH, w, tdH ), tdColor );
      }
      else
      {
         drawRectFill( dc, QRect( x, y + titleH, w, h - titleH ), tdColor );
      }
   }
   else
   {
      drawRectFill( dc, QRect( x, y, w, h ), titleColor );
   }

   // Draw table header
   if ( h > titleH + tdH )
   {

   }

}

inline void
drawText( QPainter & dc, int x, int y, QString const & msg, QColor textColor )
{
   auto fm = dc.fontMetrics();
   //auto ts = dc.fontMetrics().tightBoundingRect( msg ).size();
//   auto w = fm.horizontalAdvance( msg );
//   auto h = fm.height();
   dc.setPen( QPen( textColor ) );
   dc.setBrush( Qt::NoBrush );
   dc.drawText( x, y + fm.ascent(), msg );
}

*/

void
Footer::on_btnShowMidiKeyboard( bool checked )
{
    //m_midiMasterKeyboard->setVisible( checked );
   //updateLayout();
}

void
Footer::on_btnShowQuickHelpPanel( bool checked )
{
    auto& skin = App::instance()->getSkin();

    DE_DEBUG("checked = ",checked, ", "
             "isDetailsVisible = ", skin.bIsDetailsVisible, ", "
             "isQuickHelpVisible = ", skin.bIsQuickHelpVisible )
    if ( checked )
    {
        skin.bIsDetailsVisible = true;
        skin.bIsQuickHelpVisible = true;

        if ( !m_btnDetails->isChecked() )
        {
            m_btnDetails->blockSignals( true );
            m_btnDetails->setChecked( true );
            m_btnDetails->blockSignals( false );
        }
    }
    else
    {
        skin.bIsQuickHelpVisible = false;
    }

    // updateLayout();
}


void
Footer::on_btnShowClipOverview( bool checked )
{
    Skin& skin = App::instance()->getSkin();
    DE_DEBUG("checked = ",checked, ", "
             "isDetailsVisible = ", skin.bIsDetailsVisible, ", "
             "isQuickHelpVisible = ", skin.bIsQuickHelpVisible )

    if ( !skin.bIsDetailsVisible )
    {
        if ( !m_btnDetails->isChecked() )
        {
            m_btnDetails->blockSignals( true );
            m_btnDetails->setChecked( true );
            m_btnDetails->blockSignals( false );
        }

        if ( skin.bIsQuickHelpVisible )
        {
            if ( !m_btnQuickHelp->isChecked() )
            {
                m_btnQuickHelp->blockSignals( true );
                m_btnQuickHelp->setChecked( true );
                m_btnQuickHelp->blockSignals( false );
            }
        }
    }

    skin.bIsClipEditorVisible = true;


    //   if ( !m_btnShowQuickHelpPanel->isChecked() )
    //   {
    //      m_btnShowQuickHelpPanel->blockSignals( true );
    //      m_btnShowQuickHelpPanel->setChecked( true );
    //      m_btnShowQuickHelpPanel->blockSignals( false );
    //   }

    // m_clipContent->blockSignals( true );
    // m_clipContent->show();
    // m_clipContent->blockSignals( false );

    // m_detailStack->blockSignals( true );
    // m_detailStack->hide();
    // m_detailStack->blockSignals( false );
}

void
Footer::on_btnShowTrackOverview( bool checked )
{
    Skin& skin = App::instance()->getSkin();
    DE_DEBUG("checked = ",checked, ", "
            "isDetailsVisible = ", skin.bIsDetailsVisible, ", "
            "isQuickHelpVisible = ", skin.bIsQuickHelpVisible )

    if ( !skin.bIsDetailsVisible )
    {
        if ( !m_btnDetails->isChecked() )
        {
            m_btnDetails->blockSignals( true );
            m_btnDetails->setChecked( true );
            m_btnDetails->blockSignals( false );
        }
        if ( skin.bIsQuickHelpVisible )
        {
            if ( !m_btnQuickHelp->isChecked() )
            {
                m_btnQuickHelp->blockSignals( true );
                m_btnQuickHelp->setChecked( true );
                m_btnQuickHelp->blockSignals( false );
            }
        }
    }

    skin.bIsClipEditorVisible = false;

    // m_clipContent->blockSignals( true );
    // m_clipContent->hide();
    // m_clipContent->blockSignals( false );

    // m_detailStack->blockSignals( true );
    // m_detailStack->show();
    // m_detailStack->blockSignals( false );

    //updateLayout();
}


void
Footer::on_btnShowDetailPanel( bool checked )
{
    Skin& skin = App::instance()->getSkin();
    DE_DEBUG("checked = ",checked, ", "
               "isDetailVisible = ", skin.bIsDetailsVisible, ", "
               "isQuickHelpVisible = ", skin.bIsQuickHelpVisible )

    if ( checked )
    {
        skin.bIsDetailsVisible = true;

        if ( skin.bIsQuickHelpVisible && !m_btnQuickHelp->isChecked() )
        {
            m_btnQuickHelp->blockSignals( true );
            m_btnQuickHelp->setChecked( true );
            m_btnQuickHelp->blockSignals( false );
        }
    }
    else
    {
        skin.bIsDetailsVisible = false;

        if ( m_btnQuickHelp->isChecked() )
        {
            m_btnQuickHelp->blockSignals( true );
            m_btnQuickHelp->setChecked( false );
            m_btnQuickHelp->blockSignals( false );
        }
    }

    //updateLayout();
}
/*
void
Footer::mouseReleaseEvent( QMouseEvent* event )
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
*/

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

#if 0
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

ImageButton*
Footer::createShowQuickHelpPanelButton()
{
   auto btn = new ImageButton( this );

   LiveSkin const & skin = m_skin;
   int cbs = skin.getInt( LiveSkin::CircleButtonSize );

   btn->setCheckable( true );
   btn->setChecked( true );

   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor;

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "##\n"
         "####\n"
         "######\n"
         "########\n"
         "#########\n"
         "########\n"
         "######\n"
         "####\n"
         "##\n";

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   msg = "    #\n"
         "   ###\n"
         "   ###\n"
         "  #####\n"
         "  #####\n"
         " #######\n"
         " #######\n"
         "#########\n"
         "#########\n";

   ico = createAsciiArt( symColor, fgColor, msg );
   img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}


ImageButton*
Footer::createShowMidiKeyboardButton()
{
   auto btn = new ImageButton( this );

   LiveSkin const & skin = m_skin;
   int bw = 2*skin.getInt( LiveSkin::CircleButtonSize );
   int bh = skin.getInt( LiveSkin::CircleButtonSize );

   btn->setCheckable( true );
   btn->setChecked( true );

   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor;

/*
 *    //QFont font = getFontAwesome( 14 );
   std::string
   msg = "ooooooooooooooooooooooooo        ##\n"
         "o o#o o#o o o#o o#o o#o o      ####\n"
         "o o#o o#o o o#o o#o o#o o    ######\n"
         "o o#o o#o o o#o o#o o#o o  ########\n"
         "o  o   o  o  o   o   o  o #########\n"
         "o  o   o  o  o   o   o  o  ########\n"
         "o  o   o  o  o   o   o  o    ######\n"
         "o  o   o  o  o   o   o  o      ####\n"
         "ooooooooooooooooooooooooo        ##\n";
   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createRoundRectImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   msg = "ooooooooooooooooooooooooo #########\n"
         "o o#o o#o o o#o o#o o#o o #########\n"
         "o o#o o#o o o#o o#o o#o o  ####### \n"
         "o o#o o#o o o#o o#o o#o o  ####### \n"
         "o  o   o  o  o   o   o  o   #####  \n"
         "o  o   o  o  o   o   o  o   #####  \n"
         "o  o   o  o  o   o   o  o    ###   \n"
         "o  o   o  o  o   o   o  o    ###   \n"
         "ooooooooooooooooooooooooo     #    \n";

*/
   std::string
   msg = "ooooooooooooooooooooooooo\n"
         "o o#o o#o o o#o o#o o#o o\n"
         "o o#o o#o o o#o o#o o#o o\n"
         "o o#o o#o o o#o o#o o#o o\n"
         "o  o   o  o  o   o   o  o\n"
         "o  o   o  o  o   o   o  o\n"
         "o  o   o  o  o   o   o  o\n"
         "o  o   o  o  o   o   o  o\n"
         "ooooooooooooooooooooooooo\n";

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createRoundRectImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   msg = "ooooooooooooooooooooooooo\n"
         "o o#o o#o o o#o o#o o#o o\n"
         "o o#o o#o o o#o o#o o#o o\n"
         "o o#o o#o o o#o o#o o#o o\n"
         "o o#o o#o o o#o o#o o#o o\n"
         "o o#o o#o o o#o o#o o#o o\n"
         "o o#o o#o o o#o o#o o#o o\n"
         "o  o   o  o  o   o   o  o\n"
         "o  o   o  o  o   o   o  o\n"
         "o  o   o  o  o   o   o  o\n"
         "o  o   o  o  o   o   o  o\n"
         "o  o   o  o  o   o   o  o\n"
         "ooooooooooooooooooooooooo\n";

   ico = createAsciiArt( symColor, fgColor, msg );
   img = createRoundRectImage( bw,bh, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
Footer::createShowDetailPanelButton()
{
   auto btn = new ImageButton( this );

   LiveSkin const & skin = m_skin;
   int cbs = skin.getInt( LiveSkin::CircleButtonSize );

   btn->setCheckable( true );
   btn->setChecked( true );

   auto symColor = skin.symbolColor;
   auto bgColor = skin.windowColor;
   auto fgColor = skin.panelColor;

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "       ##\n"
         "     ####\n"
         "   ######\n"
         " ########\n"
         "#########\n"
         " ########\n"
         "   ######\n"
         "     ####\n"
         "       ##\n";

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   msg = "    #\n"
         "   ###\n"
         "   ###\n"
         "  #####\n"
         "  #####\n"
         " #######\n"
         " #######\n"
         "#########\n"
         "#########\n";

   ico = createAsciiArt( symColor, fgColor, msg );
   img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}
#endif
