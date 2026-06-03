#include "FooterPanel.h"
#include <QResizeEvent>
#include "App.h"

FooterPanel::FooterPanel(QWidget* parent )
   : QWidget(parent)
   , m_hasFocus( false )
{
    setObjectName( "FooterPanel" );
    setContentsMargins( 0,0,0,0 );
    setMouseTracking( true );

    m_btnShowQuickHelpPanel = new SvgButton(this);
    m_longText = new LongText( this );
    m_btnShowMidiKeyboard = new SvgButton(this);
    m_btnClipOverview = new ClipOverview(this);
    m_btnTrackOverview = new TrackOverview(this);
    m_btnShowDetailPanel = new SvgButton(this);

    connect( m_btnShowQuickHelpPanel, SIGNAL(toggled(bool)),
            this, SLOT(on_btnShowQuickHelpPanel(bool)) );

    connect( m_btnShowMidiKeyboard, SIGNAL(toggled(bool)),
            this, SLOT(on_btnShowMidiKeyboard(bool)) );

    connect( m_btnClipOverview, SIGNAL(clicked(bool)),
            this, SLOT(on_btnShowClipOverview(bool)) );

    connect( m_btnTrackOverview, SIGNAL(clicked(bool)),
            this, SLOT(on_btnShowTrackOverview(bool)) );

    connect( m_btnShowDetailPanel, SIGNAL(toggled(bool)),
            this, SLOT(on_btnShowDetailPanel(bool)) );

    //   connect( &m_trackList, SIGNAL(currentTrackIdChanged(int)),
    //           this, SLOT(on_currentTrackIdChanged(int)) );

    auto h = new QHBoxLayout;
    h->setContentsMargins(0,0,0,0);
    h->setSpacing(0);
    h->addWidget(m_btnShowQuickHelpPanel);
    h->addWidget(m_longText,1);
    h->addWidget(m_btnShowMidiKeyboard);
    h->addWidget(m_btnClipOverview);
    h->addWidget(m_btnTrackOverview);
    h->addWidget(m_btnShowDetailPanel);
    setLayout(h);

    setMinimumHeight(68);
    setMaximumHeight(68);
}

FooterPanel::~FooterPanel()
{}

/*
inline void
setWidgetBounds( QWidget* p, QRect const & r )
{
    if ( !p ) { DE_ERROR("nullptr") return; }

    if ( (r.width() < 1) || (r.height() < 1) )
    {
        p->setVisible( false );
    }
    else
    {
        p->setVisible( true );
        p->move( r.topLeft() );
        p->setMinimumSize( r.size() );
        p->setMaximumSize( r.size() );
    }

}
*/

void
FooterPanel::updateLayout()
{
    /*
    int w = width();
    int h = height();

    Skin& skin = App::instance()->getSkin();
    int const p = m_padding;
    int const b = m_circleButtonSize;

    //int m_clipOverviewW = 64;
    //int m_trackOverviewW = 64;

    m_rcFooterContent = QRect( p, p, w - 2*p, 12 + 16 );
    int frW = m_rcFooterContent.width();
    int frH = m_rcFooterContent.height();
    int x = m_rcFooterContent.x();
    int y = m_rcFooterContent.y();

    auto co = m_btnClipOverview->computeBestSize();
    auto to = m_btnTrackOverview->computeBestSize();

    int clipW = co.width();
    int trackW = to.width();
    int restW = (4*b + 5*p + clipW + trackW );
    int longW = frW - restW;

    m_rcBtnShowQuickHelpPanel = QRect( x, y+3, b, b ); x += b + p;
    m_rcLongPanel = QRect( x, y, longW, frH ); x += longW + p;
    m_rcBtnShowMidiKeyboardPanel = QRect( x, y+3, 2*b, b ); x += 2*b + p;
    m_rcClipOverviewPanel = QRect( x, y, clipW, frH ); x += clipW + p;
    m_rcTrackOverviewPanel = QRect( x, y, trackW, frH ); x += trackW + p;
    m_rcBtnShowDetailPanel = QRect( x, y+3, b, b ); x += b + p;

    m_rcLongView = QRect(
        m_rcLongPanel.x() + 6,
        m_rcLongPanel.y() + 6,
        m_rcLongPanel.width() - 12,
        m_rcLongPanel.height() - 12 );

    m_rcClipOverview = QRect(
        m_rcClipOverviewPanel.x() + 6,
        m_rcClipOverviewPanel.y() + 6,
        m_rcClipOverviewPanel.width() - 12,
        m_rcClipOverviewPanel.height() - 12 );

    m_rcTrackOverview = QRect( m_rcTrackOverviewPanel.x() + 6,
     m_rcTrackOverviewPanel.y() + 6,
     m_rcTrackOverviewPanel.width() - 12,
     m_rcTrackOverviewPanel.height() - 12 );

    setWidgetBounds( m_btnShowQuickHelpPanel, m_rcBtnShowQuickHelpPanel );
    setWidgetBounds( m_longText, m_rcLongView );
    setWidgetBounds( m_btnShowMidiKeyboard, m_rcBtnShowMidiKeyboardPanel );
    setWidgetBounds( m_btnClipOverview, m_rcClipOverview );
    setWidgetBounds( m_btnTrackOverview, m_rcTrackOverview );
    setWidgetBounds( m_btnShowDetailPanel, m_rcBtnShowDetailPanel );
    update();
    */
}


void
FooterPanel::on_btnShowMidiKeyboard( bool checked )
{
    //m_midiMasterKeyboard->setVisible( checked );
   //updateLayout();
}

void
FooterPanel::on_btnShowQuickHelpPanel( bool checked )
{
    auto& skin = App::instance()->getSkin();

    DE_DEBUG("checked = ",checked, ", "
             "isDetailsVisible = ", skin.bIsDetailsVisible, ", "
             "isQuickHelpVisible = ", skin.bIsQuickHelpVisible )
    if ( checked )
    {
        skin.bIsDetailsVisible = true;
        skin.bIsQuickHelpVisible = true;

        if ( !m_btnShowDetailPanel->isChecked() )
        {
            m_btnShowDetailPanel->blockSignals( true );
            m_btnShowDetailPanel->setChecked( true );
            m_btnShowDetailPanel->blockSignals( false );
        }
    }
    else
    {
        skin.bIsQuickHelpVisible = false;
    }

    updateLayout();
}


void
FooterPanel::on_btnShowClipOverview( bool checked )
{
    Skin& skin = App::instance()->getSkin();
    DE_DEBUG("checked = ",checked, ", "
             "isDetailsVisible = ", skin.bIsDetailsVisible, ", "
             "isQuickHelpVisible = ", skin.bIsQuickHelpVisible )

    if ( !skin.bIsDetailsVisible )
    {
        if ( !m_btnShowDetailPanel->isChecked() )
        {
            m_btnShowDetailPanel->blockSignals( true );
            m_btnShowDetailPanel->setChecked( true );
            m_btnShowDetailPanel->blockSignals( false );
        }

        if ( skin.bIsQuickHelpVisible )
        {
            if ( !m_btnShowQuickHelpPanel->isChecked() )
            {
                m_btnShowQuickHelpPanel->blockSignals( true );
                m_btnShowQuickHelpPanel->setChecked( true );
                m_btnShowQuickHelpPanel->blockSignals( false );
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

    updateLayout();
}

void
FooterPanel::on_btnShowTrackOverview( bool checked )
{
    Skin& skin = App::instance()->getSkin();
    DE_DEBUG("checked = ",checked, ", "
            "isDetailsVisible = ", skin.bIsDetailsVisible, ", "
            "isQuickHelpVisible = ", skin.bIsQuickHelpVisible )

    if ( !skin.bIsDetailsVisible )
    {
        if ( !m_btnShowDetailPanel->isChecked() )
        {
            m_btnShowDetailPanel->blockSignals( true );
            m_btnShowDetailPanel->setChecked( true );
            m_btnShowDetailPanel->blockSignals( false );
        }
        if ( skin.bIsQuickHelpVisible )
        {
            if ( !m_btnShowQuickHelpPanel->isChecked() )
            {
                m_btnShowQuickHelpPanel->blockSignals( true );
                m_btnShowQuickHelpPanel->setChecked( true );
                m_btnShowQuickHelpPanel->blockSignals( false );
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

    updateLayout();
}


void
FooterPanel::on_btnShowDetailPanel( bool checked )
{
    Skin& skin = App::instance()->getSkin();
    DE_DEBUG("checked = ",checked, ", "
               "isDetailVisible = ", skin.bIsDetailsVisible, ", "
               "isQuickHelpVisible = ", skin.bIsQuickHelpVisible )

    if ( checked )
    {
        skin.bIsDetailsVisible = true;

        if ( skin.bIsQuickHelpVisible && !m_btnShowQuickHelpPanel->isChecked() )
        {
            m_btnShowQuickHelpPanel->blockSignals( true );
            m_btnShowQuickHelpPanel->setChecked( true );
            m_btnShowQuickHelpPanel->blockSignals( false );
        }
    }
    else
    {
        skin.bIsDetailsVisible = false;

        if ( m_btnShowQuickHelpPanel->isChecked() )
        {
            m_btnShowQuickHelpPanel->blockSignals( true );
            m_btnShowQuickHelpPanel->setChecked( false );
            m_btnShowQuickHelpPanel->blockSignals( false );
        }
    }

    updateLayout();
}
/*
void
FooterPanel::mouseReleaseEvent( QMouseEvent* event )
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
FooterPanel::resizeEvent( QResizeEvent* event )
{
    updateLayout();
    QWidget::resizeEvent( event );
}

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

void FooterPanel::paintEvent( QPaintEvent* event )
{
    int w = width();
    int h = height();
    if ( w < 1 ) return;
    if ( h < 1 ) return;

    //std::cout << "w = " << w << ", h = " << h << std::endl;

    int r = m_radius;
    int p = m_padding;

    QPainter dc( this );
    //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
    //dc.fillRect( rect(), skin.panelBlendColor );


    //   int x = 0;
    //   int y = 0;
    //   auto fgColor = hasFocus() ? skin.focusColor : skin.panelColor;
    //   drawRoundRectFill( dc, x,y, w,h, fgColor, r,r );
    //drawRoundRectFill( dc, x, y, w, h, bgColor, rx, ry );
    //drawRoundRectFill( dc, x+1, y+1, w-2, h-2, fgColor, rx, ry );

    // Draw LongText Overview
    //auto bgColor = skin.panelBlendColor;
    //auto fgColor = hasFocus ? skin.focusColor : skin.panelColor;

    QColor m_panelColor(128,128,128);
    QColor m_contentColor(255,255,255);
    QColor m_symbolColor(255,128,0);
    QColor m_focusColor(32,32,32);

    drawRoundRectFill( dc, m_rcLongPanel, m_panelColor, r, r );
    drawContent( dc, m_rcLongView, m_contentColor );
    drawText( dc, m_rcLongView.x() + p,
         m_rcLongView.y() + 1,
         "Hello Long Text",
         m_symbolColor );

    bool m_bIsClipEditorVisible = true;

    //if ( skin.isClipEdVisible && m_focusPanel == eLivePanelDetails )
    if ( m_bIsClipEditorVisible )
    {
    drawRoundRectFill( dc, m_rcClipOverviewPanel, m_focusColor, r, r );
    }
    else
    {
    drawRoundRectFill( dc, m_rcClipOverviewPanel, m_panelColor, r, r );

    }

    //if ( !skin.isClipEdVisible && m_focusPanel == eLivePanelDetails )
    if ( m_bIsClipEditorVisible )
    {
    drawRoundRectFill( dc, m_rcTrackOverviewPanel, m_focusColor, r, r );
    }
    else
    {
    drawRoundRectFill( dc, m_rcTrackOverviewPanel, m_panelColor, r, r );
    }

    // QWidget::paintEvent( event );
}


void
FooterPanel::focusInEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusInEvent( event );
}

void
FooterPanel::focusOutEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusOutEvent( event );
}

#if 0
void
FooterPanel::enterEvent( QEnterEvent* event )
{
   QWidget::enterEvent( event );
}

void
FooterPanel::leaveEvent( QEvent* event )
{
   QWidget::leaveEvent( event );
}

void
FooterPanel::mouseMoveEvent( QMouseEvent* event )
{
//   m_mouseX = event->x();
//   m_mouseY = event->y();
   QWidget::mouseMoveEvent( event );
}

void
FooterPanel::wheelEvent( QWheelEvent* event )
{
   //int wheel = event->angleDelta().y();
   QWidget::wheelEvent( event );
}

void
FooterPanel::mousePressEvent( QMouseEvent* event )
{
   QWidget::mousePressEvent( event );
}

void
FooterPanel::mouseReleaseEvent( QMouseEvent* event )
{

   QWidget::mouseReleaseEvent( event );
}

void
FooterPanel::keyPressEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyPress(",event->key(),")")
   QWidget::keyPressEvent( event );
}

void
FooterPanel::keyReleaseEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyRelease(",event->key(),")")
   QWidget::keyReleaseEvent( event );
}

ImageButton*
FooterPanel::createShowQuickHelpPanelButton()
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
FooterPanel::createShowMidiKeyboardButton()
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
FooterPanel::createShowDetailPanelButton()
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
