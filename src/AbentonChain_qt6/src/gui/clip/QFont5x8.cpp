#include "QFont5x8.h"
#include <QPainter>
#include <QDebug>

void
drawText( de::Font5x8 font, QPainter & dc, int x, int y, QString const & msg,
        QColor const & color, de::Align::EAlign align )
{
   int cx = font.quadWidth + font.quadSpacingX;
   int cy = font.quadHeight + font.quadSpacingY;
   int gw = 5 * cx + font.glyphSpacingX;
   int gh = 8 * cy + font.glyphSpacingY;

   // Align: default is TopLeft
   glm::ivec2 aligned_pos(x,y);

   if ( align != de::Align::TopLeft )
   {
      auto ts = font.getTextSize( msg.toStdWString() ); // Optimize here for single lines, default: multiline but slower.
      float tw = ts.width;
      float th = ts.height;

           if ( align & de::Align::Center ){ aligned_pos.x -= tw/2; }
      else if ( align & de::Align::Right ) { aligned_pos.x -= tw;   }
      else {}
           if ( align & de::Align::Middle ){ aligned_pos.y -= th/2; }
      else if ( align & de::Align::Bottom ){ aligned_pos.y -= th;   }
      else {}
   }

   dc.setBrush( Qt::NoBrush );
   dc.setPen( QPen( color ) );

   auto g = aligned_pos;

   for ( int u = 0; u < msg.size(); ++u )
   {
      uint32_t ch = msg.at( u ).unicode();
      if ( ch == '\r' )  // Mac or Windows line breaks.
      {
         g.x = aligned_pos.x;
         g.y += gh;
         continue;
      }
      if ( ch == '\n' )	// Mac or Windows line breaks.
      {
         g.x = aligned_pos.x;
         g.y += gh;
         continue;
      }

        auto face = de::getFontFace5x8();

      //char ch = char( std::clamp( unicode, uint32_t(30), uint32_t(127) ) );

      if ( ch < face->m_glyphCache.size() )
      {
         auto const & m = face->m_glyphCache[ ch ];

         // Add dot matrix 5mal8 as max 49 quads
         for ( int j = 0; j < 8; ++j )
         {
            for ( int i = 0; i < 5; ++i )
            {
               if ( !m.get( i,j ) ) continue; // is white or black.

               int x1 = g.x + i * cx;
               int y1 = g.y + j * cy;
   //            dc.drawPoint( x1,y1 );

               for ( int ky = 0; ky < font.quadHeight; ++ky )
               {
                  for ( int kx = 0; kx < font.quadWidth; ++kx )
                  {
                     int x2 = x1 + kx;
                     int y2 = y1 + ky;
                     dc.drawPoint( x2,y2 );
                  }
               }
            }
         }

      }

      g.x += gw;
   }
}



#if 0
QFont5x8::QFont5x8()
   : quadWidth( 1 )
   , quadHeight( 1 )
   , quadSpacingX( 0 )
   , quadSpacingY( 0 )
   , glyphSpacingX( 1 )
   , glyphSpacingY( 1 )
   , m_GlyphCache( getGlyphCache() ) // Holds a local pointer to a static
{}

uint64_t
QFont5x8::operator< ( QFont5x8 const & o ) const
{
   if ( quadWidth < o.quadWidth ) return true;
   if ( quadHeight < o.quadHeight ) return true;
   if ( quadSpacingX < o.quadSpacingX ) return true;
   if ( quadSpacingY < o.quadSpacingY ) return true;
   if ( glyphSpacingX < o.glyphSpacingX ) return true;
   if ( glyphSpacingY < o.glyphSpacingY ) return true;
   return false;
}

QSize
QFont5x8::getTextSize( QString const & msg ) const
{
   return getTextSize( msg, *this );
}


// static
QSize
QFont5x8::getTextSize( QString const & msg, QFont5x8 const & font )
{
   if ( msg.isEmpty() ) return QSize(0,0);

   int32_t lineCount = 1;
   int32_t lineWidth = 0;
   int32_t lineWidthMax = 0;

   for ( int i = 0; i < msg.size(); ++i )
   {
      uint32_t ch = msg.at( i ).unicode();
      if ( ch == '\r' )  // Mac or Windows line breaks.
      {
         lineCount++;
         lineWidthMax = std::max( lineWidthMax, lineWidth );
         lineWidth = 0;
         continue;
      }
      if ( ch == '\n' )	// Mac or Windows line breaks.
      {
         lineCount++;
         lineWidthMax = std::max( lineWidthMax, lineWidth );
         lineWidth = 0;
         continue;
      }

      lineWidth++;
   }
   lineWidthMax = std::max( lineWidthMax, lineWidth );

   int tx = 5*(font.quadWidth + font.quadSpacingX) * lineWidthMax
               + (lineWidthMax-1) * font.glyphSpacingX;
   int ty = 8*(font.quadHeight + font.quadSpacingY)* lineCount
               + (lineCount-1) * font.glyphSpacingY;

   return QSize( tx, ty );
}

void
QFont5x8::drawText( QPainter & dc, int x, int y, QString const & msg,
            QColor const & color, de::Align::EAlign align ) const
{
   int cx = quadWidth + quadSpacingX;
   int cy = quadHeight + quadSpacingY;
   int gw = 5 * cx + glyphSpacingX;
   int gh = 8 * cy + glyphSpacingY;

   // Align: default is TopLeft
   glm::ivec2 aligned_pos(x,y);

   if ( align != de::Align::TopLeft )
   {
      QSize ts = getTextSize( msg ); // Optimize here for single lines, default: multiline but slower.
      float tw = ts.width();
      float th = ts.height();

           if ( align & de::Align::Center ){ aligned_pos.x -= tw/2; }
      else if ( align & de::Align::Right ) { aligned_pos.x -= tw;   }
      else {}
           if ( align & de::Align::Middle ){ aligned_pos.y -= th/2; }
      else if ( align & de::Align::Bottom ){ aligned_pos.y -= th;   }
      else {}
   }

   dc.setBrush( Qt::NoBrush );
   dc.setPen( QPen( color ) );

   auto g = aligned_pos;

   for ( int u = 0; u < msg.size(); ++u )
   {
      uint32_t ch = msg.at( u ).unicode();
      if ( ch == '\r' )  // Mac or Windows line breaks.
      {
         g.x = aligned_pos.x;
         g.y += gh;
         continue;
      }
      if ( ch == '\n' )	// Mac or Windows line breaks.
      {
         g.x = aligned_pos.x;
         g.y += gh;
         continue;
      }

      //char ch = char( std::clamp( unicode, uint32_t(30), uint32_t(127) ) );

      if ( ch < m_GlyphCache.size() )
      {
         QFont5x8Matrix const & m = m_GlyphCache[ ch ];

         // Add dot matrix 5mal8 as max 49 quads
         for ( int j = 0; j < 8; ++j )
         {
            for ( int i = 0; i < 5; ++i )
            {
               if ( !m.get( i,j ) ) continue; // is white or black.

               int x1 = g.x + i * cx;
               int y1 = g.y + j * cy;
   //            dc.drawPoint( x1,y1 );

               for ( int ky = 0; ky < quadHeight; ++ky )
               {
                  for ( int kx = 0; kx < quadWidth; ++kx )
                  {
                     int x2 = x1 + kx;
                     int y2 = y1 + ky;
                     dc.drawPoint( x2,y2 );
                  }
               }
            }
         }

      }

      g.x += gw;
   }
}



void
QFont5x8::drawText( QImage & o, int x, int y, QString const & msg, QColor const & color, de::Align::EAlign align ) const
{
   int cx = quadWidth + quadSpacingX;
   int cy = quadHeight + quadSpacingY;
   int gw = 5 * cx + glyphSpacingX;
   int gh = 8 * cy + glyphSpacingY;

   // Align: default is TopLeft
   glm::ivec2 aligned_pos(x,y);

   if ( align != de::Align::TopLeft )
   {
      QSize ts = getTextSize( msg ); // Optimize here for single lines, default: multiline but slower.
      int tw = ts.width();
      int th = ts.height();

           if ( align & de::Align::Center ){ aligned_pos.x -= tw/2; }
      else if ( align & de::Align::Right ) { aligned_pos.x -= tw;   }
      else {}
           if ( align & de::Align::Middle ){ aligned_pos.y -= th/2; }
      else if ( align & de::Align::Bottom ){ aligned_pos.y -= th;   }
      else {}
   }

   auto g = aligned_pos;

   int w = o.width();
   int h = o.height();

   for ( int u = 0; u < msg.size(); ++u )
   {
      uint32_t ch = msg.at( u ).unicode();

      if ( ch == '\r' )  // Mac or Windows line breaks.
      {
         g.x = aligned_pos.x;
         g.y += gh;
         continue;
      }
      if ( ch == '\n' )	// Mac or Windows line breaks.
      {
         g.x = aligned_pos.x;
         g.y += gh;
         continue;
      }

      if ( ch < m_GlyphCache.size() )
      {
         QFont5x8Matrix const & m = m_GlyphCache[ ch ];

         // Add dot matrix 5mal8 as max 49 quads
         for ( int j = 0; j < 8; ++j )
         {
            for ( int i = 0; i < 5; ++i )
            {
               int x1 = g.x + i * cx;
               int y1 = g.y + j * cy;

               if ( m.get( i,j ) )
               {
                  for ( int ky = 0; ky < quadHeight; ++ky )
                  {
                     for ( int kx = 0; kx < quadWidth; ++kx )
                     {
                        int x2 = x1 + kx;
                        if ( x2 < 0 || x2 >= w )
                        {
                           //DE_DEBUG("Invalid x(",x2,") of w(",w,")")
                           continue;
                        }

                        int y2 = y1 + ky;
                        if ( y2 < 0 || y2 >= h )
                        {
                           //DE_DEBUG("Invalid y(",y2,") of h(",h,")")
                           continue;
                        }

                        o.setPixelColor( x2,y2, color );
                     }
                  }
               }
            }
         }
      }

      g.x += gw;
   }
}

// static
QFont5x8::TGlyphCache const &
QFont5x8::getGlyphCache()
{
   static TGlyphCache s_Instance{ createGlyphCache() };
   return s_Instance;
}

// static
QFont5x8::TGlyphCache
QFont5x8::createGlyphCache()
{
   TGlyphCache cache;

   auto addGlyph = [&] ( uint32_t unicode,
      std::string const & row0 = "", std::string const & row1 = "",
      std::string const & row2 = "", std::string const & row3 = "",
      std::string const & row4 = "", std::string const & row5 = "",
      std::string const & row6 = "", std::string const & row7 = "" )
   {
      //if ( cache.find( unicode ) != cache.end() ) { return; }
      QFont5x8Matrix glyph;
      glyph.setRow(0,row0); glyph.setRow(1,row1); glyph.setRow(2,row2); glyph.setRow(3,row3);
      glyph.setRow(4,row4); glyph.setRow(5,row5); glyph.setRow(6,row6); glyph.setRow(7,row7);
      cache[ unicode ] = glyph;
   };

   addGlyph(' ', "","","","","","","" );
   addGlyph(',', "","","","","","  ++","   +","  +" );
   addGlyph(';', "","","  ++","  ++","","  ++","   +","  +" );
   addGlyph('.', "","","","","","  ++","  ++" );
   addGlyph(':', "","  ++","  ++","","  ++","  ++","" );
   addGlyph('*', "","  +  ","+ + +"," +++ ","+ + +","  +  ","" );
   addGlyph('~', ""," +   ","+ + +","   + ","","","" );
   addGlyph(L'^',"  +"," + +","+   +","","","","" );
   //addGlyph(L"°","  ++"," +  +","  ++","","","","" );
   addGlyph('"'," + +"," + +","","","","","" );
   addGlyph('\'',"  +","  +","","","","","" );
   addGlyph('\\',"","+"," +","  +","   +","","" );
   addGlyph('/',"","   +","  +"," +","+","","" );
   addGlyph('#'," + +"," + +","+++++"," + +","+++++"," + +"," + +" );
   addGlyph('!',"  +","  +","  +","  +","","  +","  +" );
   addGlyph('?'," +++","+   +","    +","   +","  +","","  +" );
   addGlyph('_',"","","","","","","+++++" );
   addGlyph('$',"  +  "," ++++","+ +  "," +++ ","  + +","++++ ","  +  " );
   addGlyph('%', "++   ","++  +","   + ","  +  "," +   ","+  ++","   ++" );
   addGlyph('&', " +   ","+ +  ","+ +  "," +   ","+ + +","+  +"," ++ +" );
   addGlyph('@', " +++ ","+   +","+ + +","+ +++","+ +  ","+    "," ++++" );
   addGlyph('<', "   +","  +"," +","+"," +","  +","   +" );
   addGlyph('>', " +","  +","   +","    +","   +","  +"," +" );
   addGlyph('|', "  +","  +","  +","  +","  +","  +","  +" );
   addGlyph('=', "","","+++++","","+++++","","" );
   addGlyph('+', "","  +","  +","+++++","  +","  +","" );
   addGlyph('-', "","","","+++++","","","" );
   addGlyph('[', "  ++","  +","  +","  +","  +","  +","  ++" );
   addGlyph(']', " ++","  +","  +","  +","  +","  +"," ++" );
   addGlyph('(', "   +","  +"," +"," +"," +","  +","   +" );
   addGlyph(')', " +","  +","   +","   +","   +","  +"," +" );
   addGlyph('{', "   ++","  +","  +"," +","  +","  +","   ++" );
   addGlyph('}', "++","  +","  +","   +","  +","  +","++" );
   addGlyph('0', " +++ ","+   +","+   +","+ + +","+   +","+   +"," +++ " );
   addGlyph('1', "  +  "," ++  ","  +  ","  +  ","  +  ","  +  "," +++ " );
   addGlyph('2', "  ++ "," +  +","    +","   + ","  +  "," +   ","+++++" );
   addGlyph('3', "+++++","   + ","  +  ","   + ","    +","+   +"," +++ " );
   addGlyph('4', "   + ","  ++ "," + + ","+  + ","+++++","   + ","   + " );
   addGlyph('5', "+++++","+    ","++++ ","    +","    +","+   +"," +++ " );
   addGlyph('6', "  ++ "," +   ","+    ","++++ ","+   +","+   +"," +++ " );
   addGlyph('7', "+++++","    +","   + ","  +  "," +   "," +   "," +   " );
   addGlyph('8', " +++ ","+   +","+   +"," +++ ","+   +","+   +"," +++ " );
   addGlyph('9', " +++ ","+   +","+   +"," ++++","    +","   + "," ++  " );
   addGlyph('A', " +++ ","+   +","+   +","+++++","+   +","+   +","+   +" );
   addGlyph('B', "++++ ","+   +","+   +","++++ ","+   +","+   +","++++ " );
   addGlyph('C', " +++ ","+   +","+    ","+    ","+    ","+   +"," +++ " );
   addGlyph('D', "++++ ","+   +","+   +","+   +","+   +","+   +","++++ " );
   addGlyph('E', "+++++","+    ","+    ","++++ ","+    ","+    ","+++++" );
   addGlyph('F', "+++++","+    ","+    ","++++ ","+    ","+    ","+    " );
   addGlyph('G', " +++ ","+   +","+    ","+ +++","+   +","+   +"," +++ " );
   addGlyph('H', "+   +","+   +","+   +","+++++","+   +","+   +","+   +" );
   addGlyph('I', " +++ ","  +  ","  +  ","  +  ","  +  ","  +  "," +++ " );
   addGlyph('J', "  +++","   +" ,"   +" ,"   +" ,"   +" ,"+  +" ," ++ " );
   addGlyph('K', "+   +","+  + ","+ +  ","++   ","+ +  ","+  + ","+   +" );
   addGlyph('L', "+    ","+    ","+    ","+    ","+    ","+    ","+++++" );
   addGlyph('M', "+   +","++ ++","+ + +","+   +","+   +","+   +","+   +" );
   addGlyph('N', "+   +","++  +","+ + +","+  ++","+   +","+   +","+   +" );
   addGlyph('O', " +++ ","+   +","+   +","+   +","+   +","+   +"," +++ " );
   addGlyph('P', "++++","+   +","+   +","++++","+","+","+" );
   addGlyph('Q', " +++ ","+   +","+   +","+   +","+ + +","+  + "," ++ +" );
   addGlyph('R', "++++","+   +","+   +","++++","+ +","+  +","+   +" );
   addGlyph('S', " ++++","+    ","+    "," +++ ","    +","    +","++++" );
   addGlyph('T', "+++++","  +","  +","  +","  +","  +","  +" );
   addGlyph('U', "+   +","+   +","+   +","+   +","+   +","+   +"," +++ " );
   addGlyph('V', "+   +","+   +","+   +","+   +","+   +"," + +","  + " );
   addGlyph('W', "+   +","+   +","+   +","+ + +","+ + +","+ + +"," + + " );
   addGlyph('X', "+   +","+   +"," + + ","  +  "," + + ","+   +","+   +" );
   addGlyph('Y', "+   +","+   +","+   +"," + +","  +","  +","  +" );
   addGlyph('Z', "+++++","    +","   +","  +"," +","+","+++++" );
   addGlyph('a', "",""," +++ ","    +"," ++++","+   +"," ++++" );
   addGlyph('b', "+    ","+    ","+ ++ ","++  +","+   +","+   +","++++ " );
   addGlyph('c', "",""," +++ ","+    ","+    ","+   +"," +++ " );
   addGlyph('d', "    +","    +"," ++ +","+  ++","+   +","+   +"," ++++" );
   addGlyph('e', "",""," +++ ","+   +","+++++","+    "," +++ " );
   addGlyph('f', "  ++ "," +  +"," +   ","+++  "," +   "," +   "," +   " );
   addGlyph('g', "",""," +++ ","+   +","+   +"," ++++","    +"," +++ " );
   addGlyph('h', "+    ","+    ","+ ++ ","++  +","+   +","+   +","+   +" );
   addGlyph('i', "  +  ","     "," ++  ","  +  ","  +  ","  +  "," +++ " );
   addGlyph('j', "    +","     ","   ++","    +","    +","    +","+  +"," ++" );
   addGlyph('k', " +   "," +   "," +  +"," + + "," ++  "," + + "," +  +" );
   addGlyph('l', " ++","  +","  +","  +","  +","  +"," +++" );
   addGlyph('m', "","","++ + ","+ + +","+ + +","+   +","+   +" );
   addGlyph('n', "","","+ ++ ","++  +","+   +","+   +","+   +" );
   addGlyph('o', "",""," +++ ","+   +","+   +","+   +"," +++" );
   addGlyph('p', "","","++++ ","+   +","+   +","++++","+","+" );
   addGlyph('q', "",""," ++ +","+  ++","+   +"," ++++","    +","    +" );
   addGlyph('r', "","","+ ++ ","++  +","+    ","+    ","+    " );
   addGlyph('s', "     ","     "," +++ ","+    "," +++ ","    +","++++ " );
   addGlyph('t', " +   "," +   ","+++  "," +   "," +   "," +  +","  ++ " );
   addGlyph('u', "","","+   +","+   +","+   +","+  ++"," ++ +" );
   addGlyph('v', "","","+   +","+   +","+   +"," + +","  +" );
   addGlyph('w', "","","+   +","+   +","+ + +","+ + +"," + +" );
   addGlyph('x', "","","+   +"," + + ","  +  "," + + ","+   +" );
   addGlyph('y', "","","+   +","+   +","+   +"," ++++","    +"," +++" );
   addGlyph('z', "","","+++++","   +","  +"," +","+++++" );

   return cache;
}
#endif
