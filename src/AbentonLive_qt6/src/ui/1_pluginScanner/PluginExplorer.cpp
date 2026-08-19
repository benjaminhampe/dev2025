#include "PluginExplorer.h"
#include "App.h"
#include "Draw.h"
#include <de_fontawesome.h>

PluginExplorer::PluginExplorer( App & app, QWidget* parent )
   : QWidget( parent )
   , m_app( app )
{
   setObjectName( "PluginExplorer" );
   setContentsMargins(0,0,0,0);
   setAcceptDrops( true );
   setMouseTracking( true );

   m_app.m_btnShowPluginDirs = createShowMoreButton();
   m_app.m_pluginScanner = new PluginScanner( m_app, this );
   m_app.m_pluginInfoTree = new PluginInfoTree( m_app, this );

//   auto v = createVBox();
//   v->setContentsMargins(6,28,6,6);
//   v->addWidget( m_pluginDirPanel );
//   v->addWidget( m_pluginInfoTree );
//   setLayout( v );
   connect( m_app.m_btnShowPluginDirs, SIGNAL(clicked(bool)), this, SLOT(on_btnShowPluginDirs(bool)) );
}

void
PluginExplorer::on_btnShowPluginDirs( bool checked )
{
    DE_TRACE("(",checked,")")
   updateLayout();
}

void
PluginExplorer::updateLayout()
{
   int w = width();
   int h = height();

   LiveSkin const & skin = m_app.m_skin;
   int sh = skin.searchBarH;

   m_clipRect = QRect( 0,0,w,h );

   int x = 0;
   int y = 0;
   if ( m_app.m_btnShowPluginDirs->isChecked() )
   {
      int dh = 200;
      int th = h - sh - dh - 12;
      m_rcPluginScanner = QRect( x,y,w,dh ); y += dh;
      y += 6;
      m_rcSearchBar = QRect( x,y,w,sh ); y += sh;
      y += 6;
      m_rcPluginInfoTree = QRect( x+6,y,w-12,th ); y += th;
   }
   else
   {
      int th = h - sh - 6;
      m_rcPluginScanner = QRect();
      m_rcSearchBar = QRect( x,y,w,sh ); y += sh;
      y += 6;
      m_rcPluginInfoTree = QRect( x+6,y,w-12,th ); y += th;
   }

   int bw = 24;
   int bx = w - bw;
   int by = m_rcSearchBar.y();
   m_rcBtnMore = QRect( bx,by, bw,bw );

   setWidgetBounds( m_app.m_pluginScanner, m_rcPluginScanner );
   setWidgetBounds( m_app.m_pluginInfoTree, m_rcPluginInfoTree );
   setWidgetBounds( m_app.m_btnShowPluginDirs, m_rcBtnMore );
   update();
}

void
PluginExplorer::resizeEvent( QResizeEvent* event )
{
   QWidget::resizeEvent( event );
   updateLayout();
}


void
PluginExplorer::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();

   if ( w > 1 && h > 1 )
   {
      LiveSkin const & skin = m_app.m_skin;

      QPainter dc( this );
      //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

      // Draw background
      dc.fillRect( rect(), skin.contentColor );

      // Draw title rect

      int bx = m_rcSearchBar.x();
      int by = m_rcSearchBar.y();
      int bw = m_rcSearchBar.width() - 23;
      int bh = m_rcSearchBar.height();
      QRect r_searchBar( bx,by, bw,bh );
      dc.fillRect( r_searchBar, skin.titleColor );

      QFont searchFont( "Arial Black", 10, 900, true );
      QFontMetrics fm( searchFont );
      dc.setFont( searchFont );
      dc.drawText( bx + 10, by + 2 + fm.ascent(), "PluginExplorer" );

      // Draw table header
//      QRect r_tableHeader(0, , w-1, skin.tdH);
//      dc.fillRect( r_tableHeader, skin.tdColor );

   }

   QWidget::paintEvent( event );
}




inline QFont getShowMoreButtonFont()
{
   QFont font("FontAwesome", 9, QFont::Bold, false );
   font.setHintingPreference( QFont::PreferFullHinting );
   font.setKerning( true );
   font.setStyleStrategy( QFont::PreferAntialias );
   return font;
}

ImageButton*
PluginExplorer::createShowMoreButton()
{
   auto btn = new ImageButton(this);
   btn->setCheckable( true );
   btn->setChecked( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = 24;
   int bh = bw;

   QFont font = getShowMoreButtonFont();
   QString msg = QChar(static_cast<ushort>(fa::plus));

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(79,254,29), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}
