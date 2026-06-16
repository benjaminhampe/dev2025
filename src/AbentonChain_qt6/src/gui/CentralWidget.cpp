#include "CentralWidget.h"
#include <QResizeEvent>
#include "App.h"

CentralWidget::CentralWidget(QWidget* parent )
   : QWidget(parent)
{
    setObjectName( "CentralWidget" );
    setContentsMargins( 0,0,0,0 );
    setMouseTracking( true );

    m_header = new Header(this);
    m_header->setVisible(false);

    m_canvas = new GL_Canvas(this);
    m_canvasContainer = QWidget::createWindowContainer(m_canvas);
    m_canvasContainer->setParent(this);
    m_canvasContainer->setMinimumSize(320, 240);
    m_canvasContainer->setVisible(false);

    m_clipEditor = new ClipEditor(this);
    m_clipEditor->setVisible(false);

    m_trackStack = new TrackStack(this);
    m_trackStack->setVisible(true);

    m_footer = new Footer(this);
    m_footer->setVisible(true);
/*
    auto v = new QVBoxLayout;
    v->setContentsMargins(0,0,0,0);
    v->setSpacing(0);
    v->addWidget(m_header);
    v->addWidget(m_canvasContainer,1);
    v->addWidget(m_clipEditor);
    v->addWidget(m_trackStack);
    v->addWidget(m_footer);

    auto content = new QWidget(this);
    content->setLayout(v);
*/
    // If you want zoom to work inside central widget too:
    installEventFilter(this);

    applySkin();
}

CentralWidget::~CentralWidget()
{}

bool CentralWidget::event(QEvent* e)
{
    if (e->type() == QEvent::LayoutRequest)
    {
        DE_BENNI("Got QEvent::LayoutRequest")
        //updateLayout();
        return true;
    }

    return QWidget::event(e);
}


void CentralWidget::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    m_zoom = skin.zoom;
    m_header->applySkin();
    m_footer->applySkin();
    m_trackStack->applySkin();
    m_clipEditor->applySkin();
    //m_canvasContainer->applySkin();
/*

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
    auto pixOn = createArrowUp(48,48,m_windowColor,m_activeColor,m_textColor);
    m_clipOverview.btnShow->setPixmaps(pixOn,pixOff);

    pixOff = createFromText(0,48,"1-ClipEditor", m_textColor,m_panelColor);
    pixOn = createFromText(0,48,"1-ClipEditor", m_textColor,m_panelColor);
    m_clipOverview.btnName->setPixmaps(pixOn,pixOff);

    pixOff = createArrowRight(48,48,m_windowColor,m_panelColor,m_textColor);
    pixOn = createArrowUp(48,48,m_windowColor,m_activeColor,m_textColor);
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
*/
    updateLayout();
}

void
CentralWidget::resizeEvent( QResizeEvent* event )
{
    updateLayout();
}

void
CentralWidget::updateLayout()
{
    const int w = width();
    const int h = height();
    int h_remain = h;
    int headerHeight = (32 * m_zoom) / 100;
    int footerHeight = 20 + (48 * m_zoom) / 100;
    int trackHeight = (400 * m_zoom) / 100;
    int clipHeight = (400 * m_zoom) / 100;
    int canvasHeight = (400 * m_zoom) / 100;

    if (m_footer->isVisible())
    {
        footerHeight = std::min(footerHeight,h_remain);
        h_remain -= footerHeight;
    }
    else
    {
        footerHeight = 0;
    }

    if (m_header->isVisible())
    {
        headerHeight = std::min(headerHeight,h_remain);
        h_remain -= headerHeight;
    }
    else
    {
        headerHeight = 0;
    }

    if (m_trackStack->isVisible())
    {
        trackHeight = std::min(trackHeight,h_remain);
        h_remain -= trackHeight;
    }
    else
    {
        trackHeight = 0;
    }

    if (m_clipEditor->isVisible())
    {
        clipHeight = std::min(clipHeight,h_remain);
        h_remain -= clipHeight;
    }
    else
    {
        clipHeight = 0;
    }

    if (m_canvasContainer->isVisible())
    {
        canvasHeight = std::min(canvasHeight,h_remain);
        h_remain -= canvasHeight;
    }
    else
    {
        canvasHeight = 0;
    }

    int x = 0;
    int y = 0;

    m_footer->setGeometry(x,h-footerHeight,w,footerHeight);

    m_header->setGeometry(x,0,w,headerHeight);

    m_trackStack->setGeometry(  x,
                                h-footerHeight-trackHeight,
                                w,
                                trackHeight);

    m_clipEditor->setGeometry(  x,
                                h-footerHeight-trackHeight-clipHeight,
                                w,
                                clipHeight);

    m_canvasContainer->setGeometry(x,headerHeight,w,canvasHeight);
    m_canvas->setGeometry(x,headerHeight,w,canvasHeight);

    if (headerHeight < 0)
    {
        DE_ERROR("Computed bad headerHeight = ",headerHeight)
    }
    if (canvasHeight < 0)
    {
        DE_ERROR("Computed bad canvasHeight = ",canvasHeight)
    }
    if (clipHeight < 0)
    {
        DE_ERROR("Computed bad clipHeight = ",clipHeight)
    }
    if (trackHeight < 0)
    {
        DE_ERROR("Computed bad trackHeight = ",trackHeight)
    }
    if (footerHeight < 0)
    {
        DE_ERROR("Computed bad footerHeight = ",footerHeight)
    }
    update();
}

void CentralWidget::paintEvent( QPaintEvent* event )
{
/*
    int w = width();
    int h = height();
    if (w < 2) return;
    if (h < 2) return;

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
*/
}
