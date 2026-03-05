#include "FooterPanel.h"
#include <QResizeEvent>
#include "App.h"
#include "Draw.h"

FooterPanel::FooterPanel( App & app, QWidget* parent )
   : QWidget(parent)
   , m_app( app )
   , m_hasFocus( false )
{
   setObjectName( "FooterPanel" );
   setContentsMargins( 0,0,0,0 );
   setMouseTracking( true );

   m_app.m_btnShowQuickHelpPanel = createShowQuickHelpPanelButton();
   m_app.m_longText = new LongText( this );
   m_app.m_btnShowMidiKeyboard = createShowMidiKeyboardButton();
   m_app.m_btnClipOverview = new ClipOverview( m_app, this );
   m_app.m_btnTrackOverview = new TrackOverview( m_app, this );
   m_app.m_btnShowDetailPanel = createShowDetailPanelButton();

   connect( m_app.m_btnShowQuickHelpPanel, SIGNAL(toggled(bool)),
            this, SLOT(on_btnShowQuickHelpPanel(bool)) );

   connect( m_app.m_btnShowMidiKeyboard, SIGNAL(toggled(bool)),
            this, SLOT(on_btnShowMidiKeyboard(bool)) );

   connect( m_app.m_btnClipOverview, SIGNAL(clicked(bool)),
            this, SLOT(on_btnShowClipOverview(bool)) );

   connect( m_app.m_btnTrackOverview, SIGNAL(clicked(bool)),
            this, SLOT(on_btnShowTrackOverview(bool)) );

   connect( m_app.m_btnShowDetailPanel, SIGNAL(toggled(bool)),
            this, SLOT(on_btnShowDetailPanel(bool)) );

//   connect( &m_app.m_trackList, SIGNAL(currentTrackIdChanged(int)),
//           this, SLOT(on_currentTrackIdChanged(int)) );
}


void
FooterPanel::on_btnShowQuickHelpPanel( bool checked )
{
   LiveSkin & skin = m_app.m_skin;

   DE_DEBUG("checked = ",checked, ", "
               "isDetailVisible = ", skin.isDetailVisible, ", "
               "isQuickHelpVisible = ", skin.isQuickHelpVisible )
   if ( checked )
   {
      skin.isDetailVisible = true;
      skin.isQuickHelpVisible = true;

      if ( !m_app.m_btnShowDetailPanel->isChecked() )
      {
         m_app.m_btnShowDetailPanel->blockSignals( true );
         m_app.m_btnShowDetailPanel->setChecked( true );
         m_app.m_btnShowDetailPanel->blockSignals( false );
      }
   }
   else
   {
      skin.isQuickHelpVisible = false;
   }

   m_app.updateLayout();
}


void
FooterPanel::on_btnShowClipOverview( bool checked )
{
   LiveSkin & skin = m_app.m_skin;
   DE_DEBUG("checked = ",checked, ", "
               "isDetailVisible = ", skin.isDetailVisible, ", "
               "isQuickHelpVisible = ", skin.isQuickHelpVisible )

   if ( !skin.isDetailVisible )
   {
      if ( !m_app.m_btnShowDetailPanel->isChecked() )
      {
         m_app.m_btnShowDetailPanel->blockSignals( true );
         m_app.m_btnShowDetailPanel->setChecked( true );
         m_app.m_btnShowDetailPanel->blockSignals( false );
      }

      if ( skin.isQuickHelpVisible )
      {
         if ( !m_app.m_btnShowQuickHelpPanel->isChecked() )
         {
            m_app.m_btnShowQuickHelpPanel->blockSignals( true );
            m_app.m_btnShowQuickHelpPanel->setChecked( true );
            m_app.m_btnShowQuickHelpPanel->blockSignals( false );
         }
      }
   }

   skin.isClipEdVisible = true;


//   if ( !m_app.m_btnShowQuickHelpPanel->isChecked() )
//   {
//      m_app.m_btnShowQuickHelpPanel->blockSignals( true );
//      m_app.m_btnShowQuickHelpPanel->setChecked( true );
//      m_app.m_btnShowQuickHelpPanel->blockSignals( false );
//   }

   m_app.m_clipContent->blockSignals( true );
   m_app.m_clipContent->show();
   m_app.m_clipContent->blockSignals( false );

   m_app.m_detailStack->blockSignals( true );
   m_app.m_detailStack->hide();
   m_app.m_detailStack->blockSignals( false );

   m_app.updateLayout();
}

void
FooterPanel::on_btnShowTrackOverview( bool checked )
{
   LiveSkin & skin = m_app.m_skin;
   DE_DEBUG("checked = ",checked, ", "
               "isDetailVisible = ", skin.isDetailVisible, ", "
               "isQuickHelpVisible = ", skin.isQuickHelpVisible )

   if ( !skin.isDetailVisible )
   {
      if ( !m_app.m_btnShowDetailPanel->isChecked() )
      {
         m_app.m_btnShowDetailPanel->blockSignals( true );
         m_app.m_btnShowDetailPanel->setChecked( true );
         m_app.m_btnShowDetailPanel->blockSignals( false );
      }
      if ( skin.isQuickHelpVisible )
      {
         if ( !m_app.m_btnShowQuickHelpPanel->isChecked() )
         {
            m_app.m_btnShowQuickHelpPanel->blockSignals( true );
            m_app.m_btnShowQuickHelpPanel->setChecked( true );
            m_app.m_btnShowQuickHelpPanel->blockSignals( false );
         }
      }
   }

   skin.isClipEdVisible = false;

   m_app.m_clipContent->blockSignals( true );
   m_app.m_clipContent->hide();
   m_app.m_clipContent->blockSignals( false );

   m_app.m_detailStack->blockSignals( true );
   m_app.m_detailStack->show();
   m_app.m_detailStack->blockSignals( false );

   m_app.updateLayout();
}


void
FooterPanel::on_btnShowDetailPanel( bool checked )
{
   LiveSkin & skin = m_app.m_skin;
   DE_DEBUG("checked = ",checked, ", "
               "isDetailVisible = ", skin.isDetailVisible, ", "
               "isQuickHelpVisible = ", skin.isQuickHelpVisible )

   if ( checked )
   {
      skin.isDetailVisible = true;

      if ( skin.isQuickHelpVisible && !m_app.m_btnShowQuickHelpPanel->isChecked() )
      {
         m_app.m_btnShowQuickHelpPanel->blockSignals( true );
         m_app.m_btnShowQuickHelpPanel->setChecked( true );
         m_app.m_btnShowQuickHelpPanel->blockSignals( false );
      }
   }
   else
   {
      skin.isDetailVisible = false;

      if ( m_app.m_btnShowQuickHelpPanel->isChecked() )
      {
         m_app.m_btnShowQuickHelpPanel->blockSignals( true );
         m_app.m_btnShowQuickHelpPanel->setChecked( false );
         m_app.m_btnShowQuickHelpPanel->blockSignals( false );
      }

   }

   m_app.updateLayout();
}
/*
void
FooterPanel::mouseReleaseEvent( QMouseEvent* event )
{
   LiveSkin & skin = m_app.m_skin;
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
FooterPanel::on_btnShowMidiKeyboard( bool checked )
{
   m_app.m_midiMasterKeyboard->setVisible( checked );
   //m_app.updateLayout();
}

void
FooterPanel::updateLayout()
{
   int w = width();
   int h = height();

   auto const & skin = m_app.m_skin;
   int const p = skin.padding;
   int const b = skin.circleButtonSize;

   //int m_clipOverviewW = 64;
   //int m_trackOverviewW = 64;

   m_rcFooterContent = QRect( p, p, w - 2*p, 12 + 16 );
   int frW = m_rcFooterContent.width();
   int frH = m_rcFooterContent.height();
   int x = m_rcFooterContent.x();
   int y = m_rcFooterContent.y();

   auto co = m_app.m_btnClipOverview->computeBestSize();
   auto to = m_app.m_btnTrackOverview->computeBestSize();

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

   m_rcLongView = QRect( m_rcLongPanel.x() + 6,
                     m_rcLongPanel.y() + 6,
                     m_rcLongPanel.width() - 12,
                     m_rcLongPanel.height() - 12 );

   m_rcClipOverview = QRect( m_rcClipOverviewPanel.x() + 6,
                     m_rcClipOverviewPanel.y() + 6,
                     m_rcClipOverviewPanel.width() - 12,
                     m_rcClipOverviewPanel.height() - 12 );

   m_rcTrackOverview = QRect( m_rcTrackOverviewPanel.x() + 6,
                     m_rcTrackOverviewPanel.y() + 6,
                     m_rcTrackOverviewPanel.width() - 12,
                     m_rcTrackOverviewPanel.height() - 12 );

   setWidgetBounds( m_app.m_btnShowQuickHelpPanel, m_rcBtnShowQuickHelpPanel );
   setWidgetBounds( m_app.m_longText, m_rcLongView );
   setWidgetBounds( m_app.m_btnShowMidiKeyboard, m_rcBtnShowMidiKeyboardPanel );
   setWidgetBounds( m_app.m_btnClipOverview, m_rcClipOverview );
   setWidgetBounds( m_app.m_btnTrackOverview, m_rcTrackOverview );
   setWidgetBounds( m_app.m_btnShowDetailPanel, m_rcBtnShowDetailPanel );
   update();
}

void
FooterPanel::resizeEvent( QResizeEvent* event )
{
   updateLayout();
   QWidget::resizeEvent( event );
}

void FooterPanel::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();
   if ( w < 1 ) return;
   if ( h < 1 ) return;

   //std::cout << "w = " << w << ", h = " << h << std::endl;

   auto const & skin = m_app.m_skin;
   int r = skin.radius;
   int p = skin.padding;

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

   drawRoundRectFill( dc, m_rcLongPanel, skin.panelColor, r, r );
   drawContent( dc, m_rcLongView, skin );
   drawText( dc, m_rcLongView.x() + p,
                 m_rcLongView.y() + 1,
                 "Hello Long Text",
                 skin.symbolColor );

   if ( skin.isClipEdVisible && m_app.m_focusPanel == eLivePanelDetails )
   {
      drawRoundRectFill( dc, m_rcClipOverviewPanel, skin.focusColor, r, r );
   }
   else
   {
      drawRoundRectFill( dc, m_rcClipOverviewPanel, skin.panelColor, r, r );

   }

   if ( !skin.isClipEdVisible && m_app.m_focusPanel == eLivePanelDetails )
   {
      drawRoundRectFill( dc, m_rcTrackOverviewPanel, skin.focusColor, r, r );
   }
   else
   {
      drawRoundRectFill( dc, m_rcTrackOverviewPanel, skin.panelColor, r, r );
   }

   QWidget::paintEvent( event );
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

   LiveSkin const & skin = m_app.m_skin;
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

   LiveSkin const & skin = m_app.m_skin;
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

   LiveSkin const & skin = m_app.m_skin;
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
