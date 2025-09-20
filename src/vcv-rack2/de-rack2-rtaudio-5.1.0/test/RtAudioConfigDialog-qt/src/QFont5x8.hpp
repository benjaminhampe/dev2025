#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <benni_common.hpp>

// A glyph is a 5x8 pixel image with black and white ( bool )
struct QFont5x8Matrix
{
   QFont5x8Matrix() {}

   void clear()
   {
      for ( size_t i = 0; i < dots.size(); ++i )
      {
         dots[ i ] = false;
      }
   }

   void set( int x, int y, bool state )
   {
      size_t i = size_t(x) + 5*size_t(y);
      if ( i >= dots.size() )
      {
         return;
      }
      dots[ i ] = state;
   }


   void off( int x, int y ) { set( x,y,false ); }
   void on( int x, int y ) { set( x,y,true ); }

   void clearRow( int y )
   {
      for ( size_t i = 0; i < 8; ++i )
      {
         set( i,y, false );
      }
   }

   void setRow( int y, std::string cc )
   {
      clearRow( y );
      for ( size_t i = 0; i < std::min( cc.size(), size_t(5) ); ++i )
      {
         if ( cc[ i ] != ' ' )
         {
            set( i,y, true );
         }
      }
   }

   bool get( int x, int y ) const
   {
      size_t i = size_t(x) + 5*size_t(y);
      if ( i >= dots.size() )
      {
         return false;
      }
      return dots[ i ];
   }

   std::array< bool, 40 > dots; // 5x8

};

// ===========================================================================
struct QFont5x8
// ===========================================================================
{
   QFont5x8();

   // Operator needed by <map>
   uint64_t operator< ( QFont5x8 const & o ) const;

   static QSize
   getTextSize( QString const & msg, QFont5x8 const & font );

   static void
   createGlyphCache( std::map< uint32_t, QFont5x8Matrix > & cache );

   QSize
   getTextSize( QString const & msg ) const;

   void
   drawText( QPainter & dc, int x, int y, QString const & msg,
         QColor const & color, de::Align::EAlign align = de::Align::TopLeft );

   void
   drawText( QImage & o, int x, int y, QString const & msg,
         QColor const & color, de::Align::EAlign align = de::Align::TopLeft );

   //   void
   //   add2DText( SMeshBuffer & o, int x, int y,
   //            std::string const & msg,
   //            uint32_t color,
   //            de::Align::EAlign align );
   //DE_CREATE_LOGGER("QFont5x8")

   int quadWidth;
   int quadHeight;
   int quadSpacingX;
   int quadSpacingY;
   int glyphSpacingX;
   int glyphSpacingY;

   std::map< uint32_t, QFont5x8Matrix > m_GlyphCache;

};
