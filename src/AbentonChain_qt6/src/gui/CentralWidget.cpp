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

    m_arraCentral = new ArraCentral(this);
    m_arraCentral->setVisible(true);

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

    connect(App::instance().get(), &App::skinChanged,
            this, &CentralWidget::applySkin);

}

CentralWidget::~CentralWidget()
{}

void CentralWidget::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    m_zoom = skin.zoom;
    m_header->applySkin();
    m_footer->applySkin();
    m_trackStack->applySkin();
    m_clipEditor->applySkin();
    //m_canvasContainer->applySkin();
    updateLayout();
}

void
CentralWidget::updateLayout()
{
    const int w = width();
    const int h = height();
    int h_remain = h;
    int headerHeight = (48 * m_zoom) / 100;
    int footerHeight = m_footer->computeBestHeight();
    int trackHeight = (376 * m_zoom) / 100;
    int clipHeight = (400 * m_zoom) / 100;
    int canvasHeight = (400 * m_zoom) / 100;
    int arrangeHeight = (48 * m_zoom) / 100;

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

    if (m_arraCentral->isVisible())
    {
        arrangeHeight = h_remain;
        h_remain -= arrangeHeight;
    }
    else
    {
        arrangeHeight = 0;
    }

    if (m_canvasContainer->isVisible())
    {
        canvasHeight = h_remain;
    }
    else
    {
        canvasHeight = 0;
    }

    int x = 0, y = 0;

    y = headerHeight;
    m_canvasContainer->setGeometry(x,y,w,canvasHeight);
    //m_canvasContainer->raise();
    // m_canvas->setGeometry(m_canvasContainer->x(),
    //                       m_canvasContainer->y(),w,canvasHeight);
    //m_canvas->raise();

    y = headerHeight + canvasHeight;
    m_arraCentral->setGeometry(x,y,w,arrangeHeight);
    //m_arraCentral->raise();

    y = h-footerHeight-trackHeight-clipHeight;
    m_clipEditor->setGeometry(x,y,w,clipHeight);
    //m_clipEditor->raise();

    y = h-footerHeight-trackHeight;
    m_trackStack->setGeometry(x,y,w,trackHeight);
    //m_trackStack->raise();

    y = 0;
    m_header->setGeometry(x,y,w,headerHeight);
    //m_header->raise();

    y = h-footerHeight;
    m_footer->setGeometry(x,y,w,footerHeight);
    //m_footer->raise();

    if (headerHeight < 0)
    {
        DE_ERROR("Computed bad headerHeight = ",headerHeight)
    }
    if (arrangeHeight < 0)
    {
        DE_ERROR("Computed bad arrangeHeight = ",arrangeHeight)
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


bool CentralWidget::event(QEvent* e)
{
    if (e->type() == QEvent::LayoutRequest)
    {
        DE_BENNI("Got QEvent::LayoutRequest")
        updateLayout();
        return true;
    }

    return QWidget::event(e);
}

void CentralWidget::resizeEvent( QResizeEvent* event )
{
    QWidget::resizeEvent(event);
    updateLayout();
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
