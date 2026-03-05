#include "ScrollBar.h"
#include "App.h"
#include "Draw.h"

// ============================================================================
ScrollHandle::ScrollHandle( App & app, QWidget* parent )
// ============================================================================
   : QWidget( parent )
   , m_app( app )
   , m_isHovered( false )
{
   setObjectName( "ScrollBar" );
   setContentsMargins(0,0,0,0);

   LiveSkin const & skin = m_app.m_skin;
   int cbs = skin.getInt( LiveSkin::ScrollButtonSize );

   setMinimumWidth( cbs );
   setMaximumWidth( cbs );
}

void
ScrollHandle::enterEvent( QEnterEvent* event )
{
   m_isHovered = true;
   QWidget::enterEvent( event );
}

void
ScrollHandle::leaveEvent( QEvent* event )
{
   m_isHovered = false;
   QWidget::leaveEvent( event );
}

void
ScrollHandle::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();

   if ( w > 0 && h > 0 )
   {
      QPainter dc( this );
      //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
      if ( m_isHovered )
      {
         drawRectFill( dc, rect(), m_app.m_skin.titleColor );
      }
      else
      {
         drawRectFill( dc, rect(), m_app.m_skin.symbolColor );
      }
   }
}

// ============================================================================
ScrollBar::ScrollBar( App & app, QWidget* parent )
// ============================================================================
   : QWidget( parent )
   , m_app( app )
   , m_isHovered( false )
{
   setObjectName( "ScrollBar" );
   setContentsMargins(0,0,0,0);

   LiveSkin const & skin = m_app.m_skin;
   int cbs = skin.getInt( LiveSkin::ScrollButtonSize );

   setMinimumWidth( cbs );
   setMaximumWidth( cbs );

   m_btnUp = createScrollUpButton();
   m_btnDown = createScrollDownButton();
   m_handle = new ScrollHandle( m_app, this );

   auto v = createVBox();
   v->setSpacing( 3 );
   v->addWidget( m_btnUp );
   v->addWidget( m_handle,1 );
   v->addWidget( m_btnDown );
   setLayout( v );
}

void
ScrollBar::enterEvent( QEnterEvent* event )
{
   m_isHovered = true;
   QWidget::enterEvent( event );
}

void
ScrollBar::leaveEvent( QEvent* event )
{
   m_isHovered = false;
   QWidget::leaveEvent( event );
}

ImageButton*
ScrollBar::createScrollUpButton()
{
   auto btn = new ImageButton( this );
   LiveSkin const & skin = m_app.m_skin;
   int cbs = skin.getInt( LiveSkin::ScrollButtonSize );

   btn->setCheckable( false );
   btn->setChecked( false );

   auto symColor = skin.symbolColor;
//   auto bgColor = skin.windowColor;
//   auto fgColor = skin.focusColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "    #\n"
         "   ###\n"
         "   ###\n"
         "  #####\n"
         "  #####\n"
         " #######\n"
         " #######\n"
         "#########\n"
         "#########\n";

   // [idle]
   QImage ico = createAsciiArt( skin.symbolColor, QColor(0,0,0,0), msg );
   //QImage img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 0, ico );
   // [idle_hover]
   btn->setImage( 1, ico );

   // [active]
   ico = createAsciiArt( skin.titleColor, QColor(0,0,0,0), msg );
   //img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 2, ico );
   // [active_hover]
   btn->setImage( 3, ico );
   return btn;
}

ImageButton*
ScrollBar::createScrollDownButton()
{
   auto btn = new ImageButton( this );
   LiveSkin const & skin = m_app.m_skin;
   int cbs = skin.getInt( LiveSkin::ScrollButtonSize );

   btn->setCheckable( false );
   btn->setChecked( false );

   std::string
   msg = "#########\n"
         "#########\n"
         " #######\n"
         " #######\n"
         "  #####\n"
         "  #####\n"
         "   ###\n"
         "   ###\n"
         "    #\n";

   // [idle]
   QImage ico = createAsciiArt( skin.symbolColor, QColor(0,0,0,0), msg );
   //QImage img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 0, ico );
   // [idle_hover]
   btn->setImage( 1, ico );

   // [active]
   ico = createAsciiArt( skin.titleColor, QColor(0,0,0,0), msg );
   //img = createCircleImage( cbs,cbs, bgColor, fgColor, ico );
   btn->setImage( 2, ico );
   // [active_hover]
   btn->setImage( 3, ico );
   return btn;
}
