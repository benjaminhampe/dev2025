/// (c) 2017 - 20180 Benjamin Hampe <benjaminhampe@gmx.de>

#pragma once
#include <QDebug>
#include <QWidget>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QSpinBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QDial>
#include <QStackedWidget>
#include <QScrollArea>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
//#include <QDesktopWidget>
#include <QThread>
#include <QStandardPaths>
#include <QFontDatabase>
#include <QMimeData>
#include <QDrag>
#include <QDropEvent>
#include <QColor>

#include <QFont5x8.h>       // Benni extra
#include <QImageWidget.h>   // Benni extra
#include <DarkImage.h>      // DarkImage
//#include <de_fontawesome.hpp> // DarkImage
// #include <sstream>
// #include <chrono>
// #include <cmath>
#include <tinyxml2/tinyxml2.h>
#include <de/audio/plugin/PluginInfoDatabase.h>

// #include <de/audio/live/BeatGrid.hpp>
// #include <de/audio/live/Clips.hpp>
// #include <de/audio/live/Spur.hpp>
// #include <de/audio/live/Track.hpp>
// #include <de/audio/live/TrackList.hpp>

// #include <de/audio/engine/MixerStream.hpp>
// #include <de/audio/plugin/GetPluginInfoList.hpp>
// #include <de/audio/dsp/IDspChainElement.hpp>
// #include <de/midi/MidiTools.hpp>
// #include <de/MouseEvent.hpp>

// #include <PluginVST24.hpp>


/*
inline void
drawText( QPainter & dc, int x, int y, QString msg,
          QColor textColor = QColor(255,255,255) )
{
   dc.setPen( QPen( textColor ) );
   dc.setBrush( QBrush( textColor ) );

   dc.drawText( x, y, msg );
}

#include <QProcess>
#include <QProcessEnvironment>
inline void
execCommand( bool checked )
{
   m_textEdit->show();

   QString program = "explorer.exe";
   QStringList arguments;
   std::string winDir = de::FileSystem::fileDir( m_app.m_pluginDbXml );
   dbStrReplace( winDir, "/", "\\");

   std::cout << "Open explorer dir " << winDir << std::endl;

   arguments << QString::fromStdString( winDir );

   QProcess* myProcess = new QProcess( this );
   myProcess->startDetached( program, arguments);
   //system( "explorer.exe /&" );
}
*/

inline QSize
getTextSize( QPainter & dc, QString const & msg )
{
   auto const & fm = dc.fontMetrics();
   //auto ts = dc.fontMetrics().tightBoundingRect( msg ).size();
   auto w = fm.horizontalAdvance( msg );
   auto h = fm.height();
   return QSize( w, h );
}

inline void
drawText( QPainter & dc, int x, int y, QString const & msg,
          QColor color = QColor(255,255,255) )
{
   auto fm = dc.fontMetrics();
   //auto ts = dc.fontMetrics().tightBoundingRect( msg ).size();
//   auto w = fm.horizontalAdvance( msg );
//   auto h = fm.height();
   dc.setPen( QPen( color ) );
   dc.setBrush( QBrush( color ) );
   dc.drawText( x, y + fm.ascent(), msg );
}

inline QImage
createImageFromText( int px, int py,
                     QString const & msg,
                     QFont const & font,
                     QColor const & fg = QColor(255,255,255),
                     QColor const & bg = QColor(0,0,0,0) )
{

   auto fm = QFontMetrics( font );
   auto w = fm.horizontalAdvance( msg ) + 2;
   auto h = fm.height() + 2;
   QImage img( w, h, QImage::Format_ARGB32 );
   img.fill( bg );

   QPainter dc;
   if ( dc.begin( &img ) )
   {
      dc.setFont( font );
      dc.setPen( QPen( fg ) );
      dc.setBrush( QBrush( fg ) );
      dc.drawText( 1, 1 + fm.ascent(), msg );
      dc.end();
   }

   return img;
}

inline bool
isMouseOverRect( int mx, int my, QRect const & r )
{
   int w = r.width();
   int h = r.height();
   // Check canvas
   if ( w < 1 || h < 1 ) return false;

   int x1 = r.x();
   int x2 = r.x() + r.width() - 1;
   int y1 = r.y();
   int y2 = r.y() + r.height() - 1;

   // Check coords
   if ( mx < x1 || mx > x2 ) return false;
   if ( my < y1 || my > y2 ) return false;
   return true;
}

inline void
setWidgetBounds( QWidget* p, QRect const & r )
{
   if ( !p )
   {
       DE_ERROR("Got nullptr")
      return;
   }

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

inline QWidget*
createWidget( QWidget* parent, int cm1 = 0, int cm2 = 0, int cm3 = 0, int cm4 = 0 )
{
   auto w = new QWidget( parent );
   w->setContentsMargins( cm1,cm2,cm3,cm4 );
   return w;
}

inline QHBoxLayout*
createHBox( int spacing = 0, int cm1 = 0, int cm2 = 0, int cm3 = 0, int cm4 = 0 )
{
   auto h = new QHBoxLayout();
   h->setContentsMargins( cm1,cm2,cm3,cm4 );
   h->setSpacing( spacing );
   return h;
}

inline QVBoxLayout*
createVBox( int spacing = 0, int cm1 = 0, int cm2 = 0, int cm3 = 0, int cm4 = 0 )
{
   auto v = new QVBoxLayout();
   v->setContentsMargins( cm1,cm2,cm3,cm4 );
   v->setSpacing( spacing );
   return v;
}

inline QFont getFontAwesome( int size = 10, bool bold = false )
{
   QFont font("FontAwesome", size, bold ? QFont::Bold : QFont::Normal, false );
   font.setHintingPreference( QFont::PreferFullHinting );
   font.setKerning( true );
   font.setStyleStrategy( QFont::PreferAntialias );
   return font;
}

inline void addFontAwesome463()
{
   int fontAwesome = QFontDatabase::addApplicationFont( "fontawesome463.ttf" );
   //int fontAwesome = QFontDatabase::addApplicationFont( "la-regular-400.ttf" );
   auto names = QFontDatabase::applicationFontFamilies( fontAwesome );
   for ( int i = 0; i < names.size(); ++i )
   {
       DE_DEBUG("Font[",i,"] = ", names.at( i ).toStdString())
   }
}

inline void setFontAwesome( QWidget* widget, int size = 10 )
{
   if ( !widget ) return;
   widget->setFont( getFontAwesome( size ) );
}

inline void setFontAwesome( QPainter & dc, int size = 10 )
{
   dc.setFont( getFontAwesome( size ) );
}

inline void
drawLine( QPainter & dc, int x1, int y1, int x2, int y2, QColor const & color )
{
   //dc.setBrush( Qt::NoBrush );
   dc.setPen( QPen( color ) );
   dc.drawLine( x1, y1, x2, y2 );
}

inline void
drawRectBorder( QPainter & dc, int x, int y, int w, int h, QColor const & color )
{
   if ( w < 1 || h < 1 ) return;
   dc.setPen( QPen( color ) );
   dc.setBrush( Qt::NoBrush );
   dc.drawRect( QRect(x,y,w-1,h-1) );
}

inline void
drawRectBorder( QPainter & dc, QRect const & pos, QColor const & color )
{
   int x = pos.x();
   int y = pos.y();
   int w = pos.width();
   int h = pos.height();
   drawRectBorder( dc, x,y,w,h, color );
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


//inline void
//fillRect( QPainter & dc, QRect pos, QColor color )
//{
//   fillRect( dc, pos.x(), pos.y(), pos.width(), pos.height(), color );
//}

inline void
drawRect( QPainter & dc, int x, int y, int w, int h, QColor const & fillColor, QColor const & penColor )
{
   dc.setPen( QPen( penColor ) );
   dc.setBrush( QBrush( fillColor ) );
   dc.drawRect( QRect(x,y,w-1,h-1) );
}



inline QImage
createRectImage( int w, int h,
                 QColor const & fg,
                 QColor const & bg,
                 QImage const & symbol = QImage(),
                 int symX = 0, int symY = 0 )
{
   QImage img( w, h, QImage::Format_ARGB32 );
   img.fill( fg );

   QPainter dc;
   if ( dc.begin( &img ) )
   {
      //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
      dc.fillRect( QRect(1,1,w-2,h-2), bg );

      if ( !symbol.isNull() )
      {
         int x = symX + ( w - symbol.width() ) / 2;
         int y = symY + ( h - symbol.height() ) / 2;
         dc.drawImage( x, y, symbol );
      }

      dc.end();
   }

   return img;
}


inline QImage
createCircleImage( int w, int h, QColor const & groundColor, QColor const & shapeColor,
             QImage const & icon = QImage(), int icoX = 0, int icoY = 0 )
{
   QImage img( w, h, QImage::Format_ARGB32 );
   //img.fill( groundColor );
   img.fill( groundColor );

   QPainter dc;
   if ( dc.begin( &img ) )
   {
      dc.setRenderHint( QPainter::Antialiasing ); // NonCosmeticDefaultPen
      dc.setPen( Qt::NoPen );
      dc.setBrush( QBrush( shapeColor ) );
      dc.drawEllipse( QRect(0,0,w,h) );

      if ( !icon.isNull() )
      {
         int x = icoX + ( w - icon.width() ) / 2;
         int y = icoY + ( h - icon.height() ) / 2;
         dc.drawImage( x, y, icon ); //, Qt::NoOpaqueDetection | Qt::NoFormatConversion );
      }

      dc.end();
   }

   return img;
}

inline QImage
createRoundRectImage( int w, int h,
                 QColor const & fg,
                 QColor const & bg,
                 QImage const & symbol = QImage(),
                 int symX = 0, int symY = 0 )
{
   QImage img( w, h, QImage::Format_ARGB32 );
   img.fill( fg );

   int r = std::min( w/2, h/2 );

   QPainter dc;
   if ( dc.begin( &img ) )
   {
      //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

      dc.setPen( Qt::NoPen );
      dc.setBrush( QBrush( bg ) );
      dc.drawRoundedRect( QRect(0,0,w,h),r,r );

      if ( !symbol.isNull() )
      {
         int x = symX + ( w - symbol.width() ) / 2;
         int y = symY + ( h - symbol.height() ) / 2;
         dc.drawImage( x, y, symbol );
      }

      dc.end();
   }

   return img;
}

inline QImage
loadImage( QString const & uri )
{
   QImage img;
   img.load( uri );
   return img;
}

inline bool
saveImage( QImage const & img, QString const & uri )
{
   return img.save( uri );
}
/*
inline QImage
createTransparencyArt(
      QColor fg = QColor(255,255,255),
      QColor bg = QColor(0,0,0,0),
      std::vector< std::vector< uint8_t > > const & pixmap )
{
   int w = 0;
   int h = 0;
   int m = pixmap.size();
   for ( int j = 0; j < m; ++j )
   {
      auto const & line = pixmap[ j ];
      int n = line.size();
      w = std::max( w, n );
      if ( n > 0 )
      {
         h = j + 1; // this line is new end of image.
      }
   }

   std::cout << "Create ascii image(" << w << "," << h << ") from " << m << " lines." << std::endl;

   QImage img( w, h, QImage::Format_ARGB32 );
   img.fill( bg );

   for ( int j = 0; j < m; ++j )
   {
      auto const & line = lines[ j ];
      int n = line.size();
      for ( int i = 0; i < n; ++i )
      {
         if ( line[ i ] != ' ' )
         {
            img.setPixelColor( i,j, fg );
         }
      }
   }

   return img;
}
*/

inline QImage
createAsciiArt(
      std::vector< std::string > const & lines,
      QColor fg = QColor(255,255,255),
      QColor bg = QColor(0,0,0,0) )
{
   int w = 0;
   int h = 0;
   int m = lines.size();
   for ( int j = 0; j < m; ++j )
   {
      auto const & line = lines[ j ];
      int n = line.size();
      w = std::max( w, n );
      if ( n > 0 )
      {
         h = j + 1; // this line is new end of image.
      }
   }

   //std::cout << "Create ascii image(" << w << "," << h << ") from " << m << " lines." << std::endl;

   QImage img( w, h, QImage::Format_ARGB32 );
   img.fill( bg );

   for ( int j = 0; j < m; ++j )
   {
      auto const & line = lines[ j ];
      int n = line.size();
      for ( int i = 0; i < n; ++i )
      {
         if ( line[ i ] != ' ' )
         {
            img.setPixelColor( i,j, fg );
         }
      }
   }

   return img;
}

inline QImage
createAsciiArt15(
      QColor fg = QColor(255,255,255),
      QColor bg = QColor(0,0,0,0),
      std::string const & s0 = "",
      std::string const & s1 = "",
      std::string const & s2 = "",
      std::string const & s3 = "",
      std::string const & s4 = "",
      std::string const & s5 = "",
      std::string const & s6 = "",
      std::string const & s7 = "",
      std::string const & s8 = "",
      std::string const & s9 = "",
      std::string const & s10 = "",
      std::string const & s11 = "",
      std::string const & s12 = "",
      std::string const & s13 = "",
      std::string const & s14 = "" )
{
   return createAsciiArt( std::vector< std::string >{ s0, s1, s2, s3, s4,
                                                      s5, s6, s7, s8, s9,
                                                      s10, s11, s12, s13, s14 }, fg, bg );
}

inline QImage
createAsciiArt( QColor fg, QColor bg, std::string const & multiLineText )
{
   return createAsciiArt( dbStrSplit( multiLineText, '\n' ), fg, bg );
}


// namespace de {

inline QColor
toQColor( uint32_t color )
{
   int32_t r = dbRGB_R(color);
   int32_t g = dbRGB_G(color);
   int32_t b = dbRGB_B(color);
   int32_t a = dbRGB_A(color);
   return QColor( r,g,b,a );
}

inline void
drawKey( QPainter & dc, QRect pos, QColor brushColor, QColor penColor )
{
   int x = pos.x();
   int y = pos.y();
   int w = pos.width()-1;
   int h = pos.height()-1;

   if ( h < 6 )
   {
      dc.setPen( Qt::NoPen );
      dc.setBrush( QBrush( brushColor ) );
      dc.drawRect( pos );
   }
   else
   {
      dc.setPen( Qt::NoPen );
      dc.setBrush( QBrush( brushColor ) );
      dc.drawRect( QRect(x+1,y+1,w-2,h-2) );

      dc.setPen( QPen( penColor ) );
      dc.drawLine( x+1,y, x+w-2, y );
      dc.drawLine( x+1,y+h-1, x+w-2, y+h-1 );
      dc.drawLine( x,y+1, x, y+h-2 );
      dc.drawLine( x+w-1,y+1, x+w-1, y+h-2 );
   }
}

inline QColor
blendRGB( QColor from, QColor to, float t )
{
   int32_t r = from.red();
   int32_t g = from.green();
   int32_t b = from.blue();
   //int32_t a = from.alpha();
   int32_t dr = int( to.red() ) - r;
   int32_t dg = int( to.green() ) - g;
   int32_t db = int( to.blue() ) - b;
   //int32_t da = int( to.alpha() ) - a;

   r = std::clamp( int32_t( t * dr + float( r ) ), 0, 255 );
   g = std::clamp( int32_t( t * dg + float( g ) ), 0, 255 );
   b = std::clamp( int32_t( t * db + float( b ) ), 0, 255 );
   //a = std::clamp( int32_t( t * da + float( a ) ), 0, 255 );
   return QColor( r, g, b );
}

// } end namespace de
