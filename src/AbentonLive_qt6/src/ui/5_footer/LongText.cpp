#include "LongText.h"
#include <QResizeEvent>
#include "Draw.h"

LongText::LongText( QWidget* parent )
   : QWidget(parent)
   , m_fillColor( 200,200,200 )
   , m_textColor( 46,56,66 )
   , m_ascent( 15 )
   , m_longText("LongText")
{
   setObjectName( "LongText" );
   setContentsMargins( 0,0,0,0 );
   //setMouseTracking( true );

   m_font = QFont("FontAwesome", 11, QFont::Bold, false );
   m_font.setHintingPreference( QFont::PreferFullHinting );
   m_font.setKerning( true );
   m_font.setStyleStrategy( QFont::PreferAntialias );

   m_ascent = QFontMetrics( m_font ).ascent();
}

// void
// LongText::resizeEvent( QResizeEvent* event )
// {
   // updateLayout();
   // QWidget::resizeEvent( event );
// }


inline QFont getPluginDirButtonFont()
{
   QFont font("FontAwesome", 11, QFont::Bold, false );
   font.setHintingPreference( QFont::PreferFullHinting );
   font.setKerning( true );
   font.setStyleStrategy( QFont::PreferAntialias );
   return font;
}


void LongText::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();
   if ( w < 1 ) return;
   if ( h < 1 ) return;

   QPainter dc( this );
   dc.setFont( m_font );
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   dc.fillRect( rect(), m_fillColor );
   drawText( dc, 6, 2+ m_ascent, m_longText, m_textColor );
}

/*
void
LongText::focusInEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusInEvent( event );
}

void
LongText::focusOutEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusOutEvent( event );
}

void
LongText::enterEvent( QEvent* event )
{
   QWidget::enterEvent( event );
}

void
LongText::leaveEvent( QEvent* event )
{
   QWidget::leaveEvent( event );
}

void
LongText::mouseMoveEvent( QMouseEvent* event )
{
//   m_mouseX = event->x();
//   m_mouseY = event->y();
   QWidget::mouseMoveEvent( event );
}

void
LongText::wheelEvent( QWheelEvent* event )
{
   //int wheel = event->angleDelta().y();
   QWidget::wheelEvent( event );
}

void
LongText::mousePressEvent( QMouseEvent* event )
{
   QWidget::mousePressEvent( event );
}

void
LongText::mouseReleaseEvent( QMouseEvent* event )
{
   int mx = event->x();
   int my = event->y();
   if ( isMouseOverRect( mx,my, m_rcClipPanel ) )
   {
      m_app.m_isClipEditorVisible = true;
      if ( !m_app.m_isDetailVisible )
      {
         m_app.m_isDetailVisible = true;
         if ( !m_btnShowDetailPanel->isChecked() )
         {
            m_btnShowDetailPanel->blockSignals( true );
            m_btnShowDetailPanel->setChecked( true );
            m_btnShowDetailPanel->blockSignals( false );
         }

         if ( m_app.m_isQuickHelpVisible )
         {
            if ( !m_btnShowQuickHelpPanel->isChecked() )
            {
               m_btnShowQuickHelpPanel->blockSignals( true );
               m_btnShowQuickHelpPanel->setChecked( true );
               m_btnShowQuickHelpPanel->blockSignals( false );
            }
         }
      }

      m_app.updateLayout();
   }

   if ( isMouseOverRect( mx,my, m_rcSpurPanel ) )
   {
      m_app.m_isClipEditorVisible = false;
      if ( !m_app.m_isDetailVisible )
      {
         m_app.m_isDetailVisible = true;
         if ( !m_btnShowDetailPanel->isChecked() )
         {
            m_btnShowDetailPanel->blockSignals( true );
            m_btnShowDetailPanel->setChecked( true );
            m_btnShowDetailPanel->blockSignals( false );
         }
         if ( m_app.m_isQuickHelpVisible )
         {
            if ( !m_btnShowQuickHelpPanel->isChecked() )
            {
               m_btnShowQuickHelpPanel->blockSignals( true );
               m_btnShowQuickHelpPanel->setChecked( true );
               m_btnShowQuickHelpPanel->blockSignals( false );
            }
         }
      }
      m_app.updateLayout();
   }

   QWidget::mouseReleaseEvent( event );
}

void
LongText::keyPressEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyPress(",event->key(),")")
   QWidget::keyPressEvent( event );
}

void
LongText::keyReleaseEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyRelease(",event->key(),")")
   QWidget::keyReleaseEvent( event );
}

void
LongText::on_toggleQuickHelpPanelVisible( bool checked )
{
   std::cout << "LongText::on_toggleQuickHelpPanelVisible( " << checked << " )" << std::endl;

   m_app.m_isQuickHelpVisible = checked;

   if ( m_app.m_isQuickHelpVisible )
   {
      if ( !m_app.m_isDetailVisible )
      {
         m_app.m_isDetailVisible = true;
         m_btnShowDetailPanel->blockSignals( true );
         m_btnShowDetailPanel->setChecked( true );
         m_btnShowDetailPanel->blockSignals( false );
      }
   }

   m_app.updateLayout();
}

void
LongText::on_toggleDetailPanelVisible( bool checked )
{
   m_app.m_isDetailVisible = checked;

   if ( !m_app.m_isDetailVisible )
   {
      if ( m_app.m_isQuickHelpVisible )
      {
         m_btnShowQuickHelpPanel->blockSignals( true );
         m_btnShowQuickHelpPanel->setChecked( false );
         m_btnShowQuickHelpPanel->blockSignals( false );
      }
   }
   else
   {
      if ( m_app.m_isQuickHelpVisible )
      {
         m_btnShowQuickHelpPanel->blockSignals( true );
         m_btnShowQuickHelpPanel->setChecked( true );
         m_btnShowQuickHelpPanel->blockSignals( false );
      }
   }

   std::cout << "LongText::on_toggleDetailPanelVisible( " << checked << " )" << std::endl;

   m_app.updateLayout();
}

ImageButton*
LongText::createShowQuickHelpPanelButton()
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
LongText::createShowMidiKeyboardButton()
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
//   msg = "       ##\n"
//         "     ####\n"
//         "   ######\n"
//         " ########\n"
//         "#########\n"
//         " ########\n"
//         "   ######\n"
//         "     ####\n"
//         "       ##\n";

   msg = "       ## ooooooooooooooooooooooooo\n"
         "     #### o o#o o#o o o#o o#o o#o o\n"
         "   ###### o o#o o#o o o#o o#o o#o o\n"
         " ######## o o#o o#o o o#o o#o o#o o\n"
         "######### o  o   o  o  o   o   o  o\n"
         " ######## o  o   o  o  o   o   o  o\n"
         "   ###### o  o   o  o  o   o   o  o\n"
         "     #### o  o   o  o  o   o   o  o\n"
         "       ## ooooooooooooooooooooooooo\n";

//   msg = "       ## \n"
//         "     #### \n"
//         "   ###### ###########\n"
//         " ######## # # # # # #\n"
//         "######### # # # # # #\n"
//         " ######## # # # # # #\n"
//         "   ###### ###########\n"
//         "     ####\n"
//         "       ##\n";

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   msg = "######### ooooooooooooooooooooooooo\n"
         "######### o o#o o#o o o#o o#o o#o o\n"
         " #######  o o#o o#o o o#o o#o o#o o\n"
         " #######  o o#o o#o o o#o o#o o#o o\n"
         "  #####   o  o   o  o  o   o   o  o\n"
         "  #####   o  o   o  o  o   o   o  o\n"
         "   ###    o  o   o  o  o   o   o  o\n"
         "   ###    o  o   o  o  o   o   o  o\n"
         "    #     ooooooooooooooooooooooooo\n";

   ico = createAsciiArt( symColor, fgColor, msg );
   img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
LongText::createShowDetailPanelButton()
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
*/
