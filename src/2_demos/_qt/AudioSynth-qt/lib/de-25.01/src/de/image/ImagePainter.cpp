#include <de/image/ImagePainter.h>
#include <de/Math.h>

namespace de {


// ==============================================================================

// ==============================================================================

// ===      ImagePainter        =================================================

// ==============================================================================

// ==============================================================================

#ifdef DE_USE_TRUETYPE_FONTS

TextSize
ImagePainter::getTextSize( std::wstring const & msg, Font font )
{
   auto face = getFontFace( font );
   if ( !face )
   {
      DE_RUNTIME_ERROR(font.toString(),"No font face")
      return {};
   }
   return face->getTextSize( msg );
}

void
ImagePainter::drawText( Image & img, int32_t x, int32_t y, std::wstring const & msg,
                        uint32_t color, Font font, Align align, bool blend )
{
   auto face = getFontFace( font );
   if ( !face )
   {
      DE_RUNTIME_ERROR(font.toString(),"No font face")
      return;
   }

   face->drawText( img, x,y, msg, color, align );
}

#endif

void
ImagePainter::brighten( Image & img, float factor )
{
    for ( int32_t y = 0; y < img.h(); ++y )
    {
        for ( int32_t x = 0; x < img.w(); ++x )
        {
            uint32_t color = img.getPixel( x,y );
            uint8_t r = dbClampBytef( factor * float( dbRGBA_R( color ) ) );
            uint8_t g = dbClampBytef( factor * float( dbRGBA_G( color ) ) );
            uint8_t b = dbClampBytef( factor * float( dbRGBA_B( color ) ) );
            uint8_t a = dbRGBA_A( color );
            img.setPixel( x,y, dbRGBA( r,g,b,a ) );
        }
    }
}


void
ImagePainter::computeSaturation( Image const & img, uint32_t & color_min, uint32_t & color_max )
{
   uint8_t min_r = 255;
   uint8_t min_g = 255;
   uint8_t min_b = 255;
   //uint8_t min_a = 255;
   uint8_t max_r = 0;
   uint8_t max_g = 0;
   uint8_t max_b = 0;
   //uint8_t max_a = 0;

   for ( int32_t y = 0; y < img.h(); ++y )
   {
      for ( int32_t x = 0; x < img.w(); ++x )
      {
         uint32_t color = img.getPixel( x,y );
         uint8_t r = dbRGBA_R( color );
         uint8_t g = dbRGBA_G( color );
         uint8_t b = dbRGBA_B( color );
         //uint8_t a = RGBA_A( color );
         min_r = std::min( min_r, r );
         min_g = std::min( min_g, g );
         min_b = std::min( min_b, b );
         //min_a = std::min( min_a, a );
         max_r = std::max( max_r, r );
         max_g = std::max( max_g, g );
         max_b = std::max( max_b, b );
         //max_a = std::max( max_a, a );
      }
   }

   color_min = dbRGBA( min_r, min_g, min_b, 255 );
   color_max = dbRGBA( max_r, max_g, max_b, 255 );
}


void
ImagePainter::autoSaturate( Image & img )
{
   uint32_t color_min, color_max;

   computeSaturation( img, color_min, color_max );

   // uint32_t color_delta = color_max - color_min;

   int r1 = dbRGBA_R( color_min );
   int g1 = dbRGBA_G( color_min );
   int b1 = dbRGBA_B( color_min );
   //int sA = RGBA_A( color_min );

   int r2 = dbRGBA_R( color_max );
   int g2 = dbRGBA_G( color_max );
   int b2 = dbRGBA_B( color_max );
   //int dA = RGBA_A( color_delta );

   for ( int32_t y = 0; y < img.h(); ++y )
   {
      for ( int32_t x = 0; x < img.w(); ++x )
      {
         uint32_t color = img.getPixel( x,y );
         int r = std::clamp( (255 * (int32_t(dbRGBA_R( color )) - r1)) / (r2-r1), 0, 255 );
         int g = std::clamp( (255 * (int32_t(dbRGBA_G( color )) - g1)) / (g2-g1), 0, 255 );
         int b = std::clamp( (255 * (int32_t(dbRGBA_B( color )) - b1)) / (b2-b1), 0, 255 );
         uint8_t a = dbRGBA_A( color );
         img.setPixel( x,y, dbRGBA( r,g,b,a ) );
      }
   }
}

void
ImagePainter::replaceColor( Image & img, uint32_t search_color, uint32_t replace_color )
{
   for ( int32_t y = 0; y < img.h(); ++y )
   {
      for ( int32_t x = 0; x < img.w(); ++x )
      {
         if ( img.getPixel( x,y ) == search_color )
         {
            img.setPixel( x,y, replace_color );
         }
      }
   }
}

void
ImagePainter::fill( Image & img, uint32_t color )
{
   img.fill( color );
}

uint32_t
ImagePainter::getPixel( Image & img, int32_t x, int32_t y, uint32_t colorKey )
{
   return img.getPixel( x, y, colorKey );
}

void
ImagePainter::setPixel( Image & img, int32_t x, int32_t y, uint32_t color, bool blend )
{
   img.setPixel( x, y, color, blend );
}

void
ImagePainter::drawImage( Image & img, Image const & src, int32_t x, int32_t y, bool blend )
{
   for ( int j = 0; j < src.h(); ++j )
   {
        for ( int i = 0; i < src.w(); ++i )
      {
         setPixel( img,
                     x + i,
                     y + j,
                     src.getPixel( i, j ),
                     blend );
      }
   }
}

void
ImagePainter::drawLineH( Image & img, int32_t x1, int32_t x2, int32_t y, uint32_t color, bool blend )
{
   auto setPixel = [&] ( int32_t x, int32_t y ) { img.setPixel( x, y, color, blend ); };
   Bresenham::drawLineH( x1, x2, y, setPixel );
}

void
ImagePainter::drawLine( Image & img, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color, bool blend )
{
   auto setPixel = [&] ( int32_t x, int32_t y ) { img.setPixel( x, y, color, blend ); };
   Bresenham::drawLine( x1, y1, x2, y2, setPixel );
}

void
ImagePainter::drawLineGrid( Image & img, Recti const & pos, uint32_t color, bool blend )
{
   drawLineGridX( img, pos, color, blend );
   drawLineGridY( img, pos, color, blend );
}

void
ImagePainter::drawLineGridX( Image & img, Recti const & pos, uint32_t color, bool blend )
{
   int32_t y = 0;
   for ( int32_t x = 0; x < img.w(); ++x )
   {
      if ( x % 2 == 1 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   y = 1;
   for ( int32_t x = 0; x < img.w(); ++x )
   {
      if ( x % 5 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   y = 2;
   for ( int32_t x = 0; x < img.w(); ++x )
   {
      if ( x > 9 && x % 10 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   y = pos.h - 1;
   for ( int32_t x = 0; x < img.w(); ++x )
   {
      if ( x % 2 == 1 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   y = pos.h - 2;
   for ( int32_t x = 0; x < img.w(); ++x )
   {
      if ( x % 5 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   y = pos.h - 3;
   for ( int32_t x = 0; x < img.w(); ++x )
   {
      if ( x > 9 && x % 10 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }
}


void
ImagePainter::drawLineGridY( Image & img, Recti const & pos, uint32_t color, bool blend )
{
   int32_t x = 0;
   for ( int32_t y = 0; y < img.h(); ++y )
   {
      if ( y % 2 == 1 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   x = 1;
   for ( int32_t y = 0; y < img.h(); ++y )
   {
      if ( y % 5 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   x = 2;
   for ( int32_t y = 0; y < img.h(); ++y )
   {
      if ( y > 9 && y % 10 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   x = img.w() - 1;
   for ( int32_t y = 0; y < img.h(); ++y )
   {
      if ( y % 2 == 1 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   x = img.w() - 2;
   for ( int32_t y = 0; y < img.h(); ++y )
   {
      if ( y % 5 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   x = img.w() - 3;
   for ( int32_t y = 0; y < img.h(); ++y )
   {
      if ( y > 9 && y % 10 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }
}

void
ImagePainter::drawDot( Image & img, int32_t x, int32_t y, int32_t w, uint32_t color, bool blend )
{
   if ( w < 2 )
   {
      img.setPixel( x-1, y-1, color, blend );
   }
   else if ( w == 2 )
   {
      img.setPixel( x-1, y-1, color, blend );
      img.setPixel( x-1, y, color, blend );
      img.setPixel( x, y-1, color, blend );
      img.setPixel( x, y, color, blend );
      return;
   }
   //   ##
   //  ####
   //  ##P#
   //   ##
   else if ( w == 2 )
   {
//      setPixel( x-1, y-2, color, blend );
//      setPixel( x, y-2, color, blend );
//      setPixel( x-2, y-1 ); setPixel( x-1, y-1 ); setPixel( x, y-1 ); setPixel( x+1, y-1 );
//      setPixel( x-2, y );   setPixel( x-1, y );   setPixel( x, y );   setPixel( x+1, y );
//      setPixel( x-1, y+1 ); setPixel( x, y+1 );
//      return;
   }
   else
   {

   }
}

void
ImagePainter::drawLineMark( Image & img, int32_t x, int32_t y, uint32_t color, bool blend )
{
   img.setPixel( x,  y-1, color, blend );
   img.setPixel( x-1,y, color, blend );
   img.setPixel( x,  y, color, blend );
   img.setPixel( x+1,y, color, blend );
   img.setPixel( x,  y+1, color, blend );
}



// ===========================================================================
// ===   ImagePainter
// ===========================================================================

// Quad ABCD:  --> u
//             |
// B---C       v
// |\  |
// | \ |
// |  \|    (ccw) ACB + ADC
// A---D    (cw) ABC + ACD
//
// static
void
ImagePainter::drawRect( Image & img,
                       Recti const & pos,
                       uint32_t colorA, uint32_t colorB,
                       uint32_t colorC, uint32_t colorD, bool blend )
{
   int32_t x1 = pos.x1();
   int32_t y1 = pos.y1();
   int32_t x2 = pos.x2();
   int32_t y2 = pos.y2();
   int32_t w = x2 - x1 + 1;
   int32_t h = y2 - y1 + 1;
   if ( w < 2 || h < 2 )
   {
      return;
   }

   glm::vec4 A = dbRGBAf( colorA );
   glm::vec4 B = dbRGBAf( colorB );
   glm::vec4 C = dbRGBAf( colorC );
   glm::vec4 D = dbRGBAf( colorD );

   for ( int32_t j = 0; j < h; ++j )
   {
      for ( int32_t i = 0; i < w; ++i )
      {
         float u = float( i ) / float( w-1 );
         float v = float( j ) / float( h-1 );

         uint32_t color = 0x00000000;

         // Oberes Dreieck ABC
         if ( u >= v )
         {
            float r = C.r + (1.0f-u) * ( B.r - C.r ) + v * ( D.r - C.r );
            float g = C.g + (1.0f-u) * ( B.g - C.g ) + v * ( D.g - C.g );
            float b = C.b + (1.0f-u) * ( B.b - C.b ) + v * ( D.b - C.b );
            float a = C.a + (1.0f-u) * ( B.a - C.a ) + v * ( D.a - C.a );
            color = dbRGBA( glm::vec4( r,g,b,a ) );
         }
         // Unteres Dreieck ADC
         else
         {
            float r = A.r + u * ( D.r - A.r ) + (1.0f-v) * ( B.r - A.r );
            float g = A.g + u * ( D.g - A.g ) + (1.0f-v) * ( B.g - A.g );
            float b = A.b + u * ( D.b - A.b ) + (1.0f-v) * ( B.b - A.b );
            float a = A.a + u * ( D.a - A.a ) + (1.0f-v) * ( B.a - A.a );
            color = dbRGBA( glm::vec4( r,g,b,a ) );
         }

         img.setPixel( i+x1,j+y1, color, blend );
      }
   }
}

void
ImagePainter::drawRect( Image & img, Recti const & pos, uint32_t color, bool blend )
{
   auto setPixel = [&] ( int32_t x, int32_t y ) { img.setPixel( x, y, color, blend ); };
   Bresenham::drawRect( pos, setPixel );
}

void
ImagePainter::drawRectBorder( Image & img, Recti const & pos, uint32_t color, bool blend )
{
   auto setPixel = [&] ( int32_t x, int32_t y ) { img.setPixel( x, y, color, blend ); };
   Bresenham::drawRectBorder( pos, setPixel );
}


void
ImagePainter::drawTriangleBorder( Image & img, int32_t x1, int32_t y1,
                             int32_t x2, int32_t y2,
                             int32_t x3, int32_t y3, uint32_t color, bool blend )
{
   auto setPixel = [&] ( int32_t x, int32_t y ) { img.setPixel( x, y, color, blend ); };
   Bresenham::drawTriangleBorder( x1, y1, x2, y2, x3, y3, setPixel );
}

void
ImagePainter::drawTriangle( Image & img, glm::ivec2 const & A, glm::ivec2 const & B, glm::ivec2 const & C, uint32_t color, bool blend )
{
   auto setPixelFill = [&] ( int32_t x, int32_t y ) { img.setPixel( x, y, color, blend ); };
   Bresenham::drawTriangle( A, B, C, setPixelFill );
}

void
ImagePainter::drawCircle( Image & img, Recti const & pos, uint32_t color, bool blend )
{
   //   drawCircleA( img, pos, color, blend );
   //   drawCircleB( img, pos, color, blend );
   //   drawCircleC( img, pos, color, blend );
   //   drawCircleD( img, pos, color, blend );

   int const w = pos.w;
   int const h = pos.h;
   int const a = w/2;
   int const b = h/2;
   int cx = pos.x + a;
   int cy = pos.y + b;
   //if ( w % 2 == 0 ) { --cx; }
   //if ( h % 2 == 0 ) { --cy; }

   if ( h % 2 == 0 )
   {
      if ( w % 2 == 0 )
      {
         Bresenham::traverseEllipse( a, b,
            [&] ( int32_t i, int32_t j )
            {
               ImagePainter::drawLineH( img, cx - i, cx + i - 1, cy - j, color, blend );
               ImagePainter::drawLineH( img, cx - i, cx + i - 1, cy + j - 1, color, blend );
            } );
      }
      else
      {
         Bresenham::traverseEllipse( a, b,
            [&] ( int32_t i, int32_t j )
            {
               ImagePainter::drawLineH( img, cx - i, cx + i, cy - j, color, blend );
               ImagePainter::drawLineH( img, cx - i, cx + i, cy + j - 1, color, blend );
            } );
      }
   }
   else
   {
      Bresenham::traverseEllipse( a, b,
         [&] ( int32_t i, int32_t j )
         {
            ImagePainter::drawLineH( img, cx - i, cx + i, cy - j, color, blend );
            ImagePainter::drawLineH( img, cx - i, cx + i, cy + j, color, blend );
         } );

   }

   // Draw center
   // img.setPixel( cx, cy, 0xFF0000FF, blend );
}
void
ImagePainter::drawCircleBorder( Image & img, Recti const & pos, uint32_t color, bool blend )
{
   //   drawCircleBorderA( img, pos, pen, blend );
   //   drawCircleBorderB( img, pos, pen, blend );
   //   drawCircleBorderC( img, pos, pen, blend );
   //   drawCircleBorderD( img, pos, pen, blend );

   int const w = pos.w;
   int const h = pos.h;
   int const a = w/2;
   int const b = h/2;
   int cx = pos.x + a;
   int cy = pos.y + b;
   //if ( w % 2 == 0 ) { --cx; }
   //if ( h % 2 == 0 ) { --cy; }

   if ( h % 2 == 0 )
   {
      if ( w % 2 == 0 )
      {
         Bresenham::traverseEllipse( a, b,
            [&] ( int32_t i, int32_t j )
            {
               img.setPixel( cx - i, cy - j, color, blend );
               img.setPixel( cx + i - 1, cy - j, color, blend );
               img.setPixel( cx - i, cy + j - 1, color, blend );
               img.setPixel( cx + i - 1, cy + j - 1, color, blend );
               //ImagePainter::drawLineH( img, cx - i, cx + i - 1, cy - j, color, blend );
               //ImagePainter::drawLineH( img, cx - i, cx + i - 1, cy + j - 1, color, blend );
            } );

         img.setPixel( cx, cy - b, color, blend );
         img.setPixel( cx, cy + b - 1, color, blend );
         img.setPixel( cx - a, cy, color, blend );
         img.setPixel( cx + a - 1, cy, color, blend );

      }
      else
      {
         Bresenham::traverseEllipse( a, b,
            [&] ( int32_t i, int32_t j )
            {
               img.setPixel( cx - i, cy - j, color, blend );
               img.setPixel( cx + i, cy - j, color, blend );
               img.setPixel( cx - i, cy + j - 1, color, blend );
               img.setPixel( cx + i, cy + j - 1, color, blend );
               //ImagePainter::drawLineH( img, cx - i, cx + i, cy - j, color, blend );
               //ImagePainter::drawLineH( img, cx - i, cx + i, cy + j - 1, color, blend );
            } );

         img.setPixel( cx, cy - b, color, blend );
         img.setPixel( cx, cy + b - 1, color, blend );
         img.setPixel( cx - a, cy, color, blend );
         img.setPixel( cx + a, cy, color, blend );

      }
   }
   else
   {
      Bresenham::traverseEllipse( a, b,
         [&] ( int32_t i, int32_t j )
         {
            img.setPixel( cx - i, cy - j, color, blend );
            img.setPixel( cx + i, cy - j, color, blend );
            img.setPixel( cx - i, cy + j, color, blend );
            img.setPixel( cx + i, cy + j, color, blend );
            //ImagePainter::drawLineH( img, cx - i, cx + i, cy - j, color, blend );
            //ImagePainter::drawLineH( img, cx - i, cx + i, cy + j, color, blend );
         } );

      img.setPixel( cx, cy - b, color, blend );
      img.setPixel( cx, cy + b, color, blend );
      img.setPixel( cx - a, cy, color, blend );
      img.setPixel( cx + a, cy, color, blend );

   }

   // Draw center
   // img.setPixel( cx, cy, 0xFF0000FF, blend );
}


void
ImagePainter::drawCircleA( Image & img, Recti const & pos, uint32_t color, bool blend )
{
   int const w = pos.w;
   int const h = pos.h;
   int const a = w/2;
   int const b = h/2;
   int cx = pos.x + a - 1;
   int cy = pos.y + b;
   //if ( w % 2 == 0 ) { --cx; }
   //if ( h % 2 == 0 ) { --cy; }
   auto my_drawHLine = [&img,cx,cy,color,blend] ( int32_t i, int32_t j )
   {
      ImagePainter::drawLineH( img, cx, cx + i, cy + j, color, blend ); // x start is const
   };
   Bresenham::traverseEllipse( a, b, my_drawHLine );
}

void
ImagePainter::drawCircleB( Image & img, Recti const & pos, uint32_t color, bool blend )
{
   int const w = pos.w;
   int const h = pos.h;
   int const a = w/2;
   int const b = h/2;
   int cx = pos.x + a;
   int cy = pos.y + b;
   //if ( h % 2 == 0 ) { --cy; }
   auto my_drawHLine = [&img,cx,cy,color,blend] ( int32_t i, int32_t j )
   {
      ImagePainter::drawLineH( img, cx - i, cx, cy + j, color, blend ); // x end is const
   };
   Bresenham::traverseEllipse( a, b, my_drawHLine );
}

void
ImagePainter::drawCircleC( Image & img, Recti const & pos, uint32_t color, bool blend )
{
   int const w = pos.w;
   int const h = pos.h;
   int const a = w/2;
   int const b = h/2;
   int cx = pos.x + a;
   int cy = pos.y + b;
   auto my_drawHLine = [&img,cx,cy,color,blend] ( int32_t i, int32_t j )
   {
      ImagePainter::drawLineH( img, cx - i, cx, cy - j, color, blend ); // x is const ( start is const )
   };
   Bresenham::traverseEllipse( a, b, my_drawHLine );
}

void
ImagePainter::drawCircleD( Image & img, Recti const & pos, uint32_t color, bool blend )
{
   int const w = pos.w;
   int const h = pos.h;
   int const a = w/2;
   int const b = h/2;
   int cx = pos.x + a - 1;
   int cy = pos.y + b;
   //if ( w % 2 == 0 ) { --cx; }
   auto my_drawHLine = [&img,cx,cy,color,blend] ( int32_t i, int32_t j )
   {
      ImagePainter::drawLineH( img, cx, cx + i, cy - j, color, blend ); // x is const ( start is const )
   };

   Bresenham::traverseEllipse( a, b, my_drawHLine );
}


void
ImagePainter::drawCircleBorderA( Image & img, Recti const & pos, Pen const & pen, bool blend )
{
   int const w = pos.w;
   int const h = pos.h;
   int const a = w/2;
   int const b = h/2;
   int cx = pos.x + a - 1;
   int cy = pos.y + b;
   //if ( w % 2 == 0 ) { --cx; }
   //if ( h % 2 == 0 ) { --cy; }
   auto my_setPixel = [&img,cx,cy,pen,blend] ( int32_t i, int32_t j )
   {
      img.setPixel( cx + i, cy + j, pen.color, blend ); // x start is const
   };
   Bresenham::traverseEllipse( a, b, my_setPixel );
}

void
ImagePainter::drawCircleBorderB( Image & img, Recti const & pos, Pen const & pen, bool blend )
{
   int const w = pos.w;
   int const h = pos.h;
   int const a = w/2;
   int const b = h/2;
   int cx = pos.x + a;
   int cy = pos.y + b;
   //if ( h % 2 == 0 ) { --cy; }
   auto my_setPixel = [&img,cx,cy,pen,blend] ( int32_t i, int32_t j )
   {
      img.setPixel( cx - i, cy + j, pen.color, blend ); // x end is const
   };
   Bresenham::traverseEllipse( a, b, my_setPixel );
}

void
ImagePainter::drawCircleBorderC( Image & img, Recti const & pos, Pen const & pen, bool blend )
{
   int const w = pos.w;
   int const h = pos.h;
   int const a = w/2;
   int const b = h/2;
   int cx = pos.x + a;
   int cy = pos.y + b;
   auto my_setPixel = [&img,cx,cy,pen,blend] ( int32_t i, int32_t j )
   {
      img.setPixel( cx - i, cy - j, pen.color, blend ); // x is const ( start is const )
   };
   Bresenham::traverseEllipse( a, b, my_setPixel );
}

void
ImagePainter::drawCircleBorderD( Image & img, Recti const & pos, Pen const & pen, bool blend )
{
   int const w = pos.w;
   int const h = pos.h;
   int const a = w/2;
   int const b = h/2;
   int cx = pos.x + a - 1;
   int cy = pos.y + b;
   //if ( w % 2 == 0 ) { --cx; }
   auto my_setPixel = [&img,cx,cy,pen,blend] ( int32_t i, int32_t j )
   {
      img.setPixel( cx + i, cy - j, pen.color, blend ); // x is const ( start is const )
   };

   Bresenham::traverseEllipse( a, b, my_setPixel );
}

void
ImagePainter::drawRoundRect( Image & img, Recti const & pos, int rx, int ry, uint32_t color, bool blend )
{
   int x1 = pos.x1();
   int x2 = pos.x1() + rx;
   int x3 = pos.x2() - rx;
   // int x4 = pos.x2();

   int y1 = pos.y1();
   int y2 = pos.y1() + ry;
   int y3 = pos.y2() - ry;
   // int y4 = pos.y2();

   int w = pos.w;
   int h = pos.h;

   int dx = w - 2*rx;
   int dy = h - 2*ry;

   Recti r_top(x2,y1,dx,ry);
   Recti r_center(x1,y2,w,dy);
   Recti r_bottom(x2,y3+1,dx, h - ry - dy );

   drawCircleC( img, Recti(x2-rx,y2-ry,2*rx,2*ry), color, blend ); // Blue
   drawRect( img, r_top, color, blend ); // light gray
   drawCircleD( img, Recti(x3-rx,y2-ry,2*rx,2*ry), color, blend ); // Orange
   drawRect( img, r_center, color, blend ); // light gray
   drawCircleB( img, Recti(x2-rx,y3-ry,2*rx,2*ry), color, blend ); // Green
   drawRect( img, r_bottom, color, blend ); // light gray
   drawCircleA( img, Recti(x3-rx,y3-ry,2*rx,2*ry), color, blend ); // Red
}

void
ImagePainter::drawRoundRectBorder( Image & img, Recti const & pos, int rx, int ry, Pen const & pen, bool blend )
{
   int x1 = pos.x1();
   int x2 = pos.x1() + rx;
   int x3 = pos.x2() - rx;
   int x4 = pos.x2();

   int y1 = pos.y1();
   int y2 = pos.y1() + ry;
   int y3 = pos.y2() - ry;
   int y4 = pos.y2();

   int w = pos.w;
   int h = pos.h;

   int dx = w - 2*rx;
   int dy = h - 2*ry;

   drawCircleBorderC( img, Recti( x2-rx, y2-ry, 2*rx, 2*ry ), pen, blend ); // Blue
   drawCircleBorderD( img, Recti( x3-rx, y2-ry, 2*rx, 2*ry ), pen, blend ); // Orange
   drawCircleBorderB( img, Recti( x2-rx, y3-ry, 2*rx, 2*ry ), pen, blend ); // Green
   drawCircleBorderA( img, Recti( x3-rx, y3-ry, 2*rx, 2*ry ), pen, blend ); // Red

   drawLine( img, Recti( x2, y1, dx, 1 ), pen.color, blend ); // top line
   drawLine( img, Recti( x2, y4, dx, 1 ), pen.color, blend ); // bottom line
   drawLine( img, Recti( x1, y2, 1, dy ), pen.color, blend ); // left line
   drawLine( img, Recti( x4, y2, 1, dy ), pen.color, blend ); // right line
}

// ===========================================================================
// ===   ImageRepeat
// ===========================================================================

Image
ImageRepeat::repeatImage( Image const & src, int w, int h, bool preserveAspect )
{
   DE_ERROR("No font impl")
   return Image();
}

// ===========================================================================
// ===   ImageRotate
// ===========================================================================

bool
ImageRotate::flipH( Image & img )
{
   if ( img.empty() ) return false;

   for ( int32_t y = 0; y < img.h(); ++y )
   {
      for ( int32_t x = 0; x < img.w() / 2; ++x )
      {
         int32_t x_flip = img.w()-1-x;
         uint32_t color1 = img.getPixel( x,      y );
         uint32_t color2 = img.getPixel( x_flip, y );
         img.setPixel( x, y, color2 );
         img.setPixel( x_flip, y, color1 );
      }
   }
   return true;
}

bool
ImageRotate::flipV( Image & img )
{
   if ( img.empty() ) return false;

   for ( int32_t y = 0; y < img.h() / 2; ++y )
   {
      int32_t y_flip = img.h()-1-y;
      for ( int32_t x = 0; x < img.w(); ++x )
      {
         uint32_t color1 = img.getPixel( x, y );
         uint32_t color2 = img.getPixel( x, y_flip );
         img.setPixel( x, y, color2 );
         img.setPixel( x, y_flip, color1 );
      }
   }
   return true;
}

bool
ImageRotate::rotate180( Image & img )
{
   if ( img.empty() ) return false;

   for ( int32_t y = 0; y < img.h() / 2; ++y )
   {
      for ( int32_t x = 0; x < img.w(); ++x )
      {
         int32_t x2 = img.w()-1-x;
         int32_t y2 = img.h()-1-y;
         uint32_t color1 = img.getPixel( x, y );
         uint32_t color2 = img.getPixel( x2, y2 );
         img.setPixel( x, y, color2 );
         img.setPixel( x2, y2, color1 );
      }
   }
   return true;
}

Image
ImageRotate::rotateLeft( Image const & img )
{
   if ( img.empty() ) return img;

   Image dst( img.h(), img.w(), img.pixelFormat() );

   for ( int32_t y = 0; y < dst.h(); ++y )
   {
      for ( int32_t x = 0; x < dst.w(); ++x )
      {
         int32_t x2 = dst.w() - 1 - x;
         uint32_t color = img.getPixel( y, x );
         dst.setPixel( x2, y, color);
      }
   }

   return dst;
}

Image
ImageRotate::rotateRight( Image const & img )
{
   if ( img.empty() ) return img;

   Image dst( img.h(), img.w(), img.pixelFormat() );

   for ( int32_t y = 0; y < dst.h(); ++y )
   {
      for ( int32_t x = 0; x < dst.w(); ++x )
      {
         int32_t y2 = img.h() - 1 - y;
         uint32_t color = img.getPixel( x, y );
         dst.setPixel(y2, x, color);
      }
   }

   return dst;
}

// static
Image
ImageRotate::rotate( Image const & img, float angleDegreesCCW, uint32_t colorKey )
{
   DE_ERROR("No font impl")
   if ( img.empty() ) return img;

   // Winkel
   while ( angleDegreesCCW < 0.0f )	{ angleDegreesCCW += 360.0f; }
   while ( angleDegreesCCW >= 360.0f )	{ angleDegreesCCW -= 360.0f; }

   //
   if ( dbEquals( angleDegreesCCW, 0.0f ) )
   {
      return img;
   }
   else if ( dbEquals( angleDegreesCCW, 90.0f))
   {
      return rotateLeft( img );
   }
   else if ( dbEquals( angleDegreesCCW, 270.0f))
   {
      return rotateRight( img );
   }
   else if ( dbEquals( angleDegreesCCW, 180.0f))
   {
      Image tmp = img;
      rotate180( tmp );
      return tmp;
   }
   else
   {
      // alpha
      const float a = ( 360.0f - angleDegreesCCW ) * Math::DEG2RAD;
      const float sa = sinf( a );
      const float ca = cosf( a );

      // gamma
      const float g = angleDegreesCCW * Math::DEG2RAD;
      const float sg = sinf( g );
      const float cg = cosf( g );

      // BERECHNNUNG des Mittelpunktes von MEM
      const float mXh = 0.5f*(float)img.w();
      const float mYh = 0.5f*(float)img.h();

      // BERECHNNUNG der Eckpunkte
      const float Ax = mXh, Ay=-mYh;
      const float Bx =-mXh, By=-mYh;
      const float Cx =-mXh, Cy= mYh;
      const float Dx = mXh, Dy= mYh;

      // TRANSFORMATION der Eckpunkte
      float Anx = Ax*ca - Ay*sa;
      float Any = Ax*sa + Ay*ca;
      float Bnx = Bx*ca - By*sa;
      float Bny = Bx*sa + By*ca;
      float Cnx = Cx*ca - Cy*sa;
      float Cny = Cx*sa + Cy*ca;
      float Dnx = Dx*ca - Dy*sa;
      float Dny = Dx*sa + Dy*ca;

      // BERECHNUNG der FINAL WIDTH and HEIGHT
      float minX = std::min( std::min( Anx, Bnx ), std::min( Cnx, Dnx ) );
      float minY = std::min( std::min( Any, Bny ), std::min( Cny, Dny ) );
      float maxX = std::max( std::max( Anx, Bnx ), std::max( Cnx, Dnx ) );
      float maxY = std::max( std::max( Any, Bny ), std::max( Cny, Dny ) );

//      if (Anx<minX) minX=Anx; if (Any<minY) minY=Any;
//      if (Bnx<minX) minX=Bnx; if (Bny<minY) minY=Bny;
//      if (Cnx<minX) minX=Cnx; if (Cny<minY) minY=Cny;
//      if (Dnx<minX) minX=Dnx; if (Dny<minY) minY=Dny;

//      if (Anx>maxX) maxX=Anx; if (Any>maxY) maxY=Any;
//      if (Bnx>maxX) maxX=Bnx; if (Bny>maxY) maxY=Bny;
//      if (Cnx>maxX) maxX=Cnx; if (Cny>maxY) maxY=Cny;
//      if (Dnx>maxX) maxX=Dnx; if (Dny>maxY) maxY=Dny;

      float fdx = maxX - minX;
      float fdy = maxY - minY;

      int32_t dx = dbRound32( fdx );
      int32_t dy = dbRound32( fdy );

//      std::string txt = "New Image with size (";
//      txt += dx;
//      txt += ", ";
//      txt += dy;
//      txt += ")";
      //os::Printer::log(txt.c_str(), ELL_INFORMATION);

      // FINAL MEMBLOCK
      Image dst( dx, dy, img.pixelFormat() );

      // BERECHNUNG des Mittelpunktes von FINAL
      float fdxh = 0.5f * fdx;
      float fdyh = 0.5f * fdy;

      // LET'S ROTATE
      uint32_t color;

      // os::Printer::log("OK", ELL_INFORMATION);
      // Fuer alle Punkte im Zielbild den entsprechenden Punkt im Ausgangsbild
      // suchen. Dadurch entstehen keine Luecken im Bild.
      for ( int32_t j = 0; j < dy; ++j )
      {
         for ( int32_t i = 0; i < dx; ++i )
         {
            if ( ( i < dst.w() ) && ( j < dst.h() ) )
            {
               float x0 = (float)i - fdxh; // relativ zu FinalMem Mittelpunkt
               float y0 = (float)j - fdyh; // relativ zu FinalMem Mittelpunkt
               float x1 = (cg*x0-sg*y0)+mXh; // relativ zu SourceMem Mittelpunkt
               float y1 = (cg*y0+sg*x0)+mYh; // relativ zu SourceMem Mittelpunkt
               int32_t x = dbRound32( x1 );
               int32_t y = dbRound32( y1 );

               // PIXEL von SourceMem
               if ( ( x >= 0 ) &&
                    ( y >= 0 ) &&
                    ( x < img.w() ) &&
                    ( y < img.h() ) )
               {
                  color = img.getPixel( x, y );
               }
               else
               {
                  color = colorKey;
               }

               dst.setPixel( i, j, color );
            }
         }
      }
      return dst;
   }


}

/// Shifts an image by 'n' pixels vertical-lines left (can be negative)
// static
void
ImageRotate::shiftLeft( Image & img, int32_t n_shifts )
{

}

/// Shifts an image by 'n' pixels vertical-lines right (can be negative)
// static
void
ImageRotate::shiftRight( Image & img, int32_t n_shifts )
{

}

/// Shifts an image by 'n' pixels horizontal-lines up (can be negative)
// static
void
ImageRotate::shiftUp( Image & img, int32_t n_shifts )
{

}

/// Shifts an image by 'n' pixels horizontal-lines down (can be negative)
// static
void
ImageRotate::shiftDown( Image & img, int32_t n_shifts )
{

}

// static
Image
ImageRotate::skewH( Image const & src, float angle )
{
   return src;
}

// static
Image
ImageRotate::skewV( Image const & src, float angle )
{
   return src;
}



// ===========================================================================
// ===   ImageScaler
// ===========================================================================
/*
Image
ImageScaler::scaleImage( Image const & src, int w, int h, bool preserveAspect )
{
   DE_ERROR("No font impl")


   return Image();
}
*/

Image
ImageScaler::resize( Image const & src, int w, int h, int scaleOp, uint32_t colorKey )
{
   if (src.empty()) return Image();

   switch( scaleOp )
   {
      case 0: return resizeNearest( src, w, h, colorKey );
      case 1: return resizeBilinear( src, w, h, colorKey );
      case 3: return resizeBicubic( src, w, h, colorKey );
      case 4: return resizeBicubic( src, w, h, colorKey );
      default: return resizeBilinear( src, w, h, colorKey );
   }
}

Image
ImageScaler::resizeNearest( Image const & src, int w, int h, uint32_t colorKey )
//                          int color_planes, bool bPreserveAspect, bool bSharpen)
{
   if (src.empty()) return Image();

   // abort
   if ( w < 1 && h < 1 ) return Image();

   // abort
   int src_w = src.w();
   int src_h = src.h();

   if ( src_w == w && src_h == h )
   {
      return src; // copy
   }

   // calculate width from aspect-ratio
   if ( w < 1 ) w = int( float( h )* float( src_w ) / float( src_h ) );

   // calculate height from aspect-ratio
   if ( h < 1 ) h = int( float( w ) * float( src_h ) / float( src_w ) );

    // create new image 32bit ARGB, fill with zero
   Image dst( w, h, src.pixelFormat() );
   if ( dst.empty() ) return dst;

   const float zoomXinv = float( src_w ) / float( w );
   const float zoomYinv = float( src_h ) / float( h );

   // RGB color planes ( alphachannel always 255 == not transparent)
//   if (planes==ECP_RGB)
//   {
   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         int32_t u = int32_t( float( x ) * zoomXinv ); // interpolate pos of current pixel in img to nearest of src
         int32_t v = int32_t( float( y ) * zoomYinv ); // interpolate pos of current pixel in img to nearest of src
         uint32_t color = src.getPixel( u, v, colorKey );
         dbRGBA_setA( color, 255 );
         dst.setPixel( x, y, color, false );
      }
   }
//   }
//    else
//   {
//      for (uint32_t y=0; y<h; y++)
//        {
//            for (uint32_t x=0; x<w; x++)
//            {
//                uint32_t u=(uint32_t)((float)x/zoomX); // interpolate pos of current pixel in img to nearest of src
//                uint32_t v=(uint32_t)((float)y/zoomY); // interpolate pos of current pixel in img to nearest of src
//                uint32_t color = src.getPixel(u,v);
//                dst->setPixel(x,y,color,false);
//            }
//        }
//   }

   return dst;
}


Image
ImageScaler::resizeBilinear( Image const & src, int w, int h, uint32_t colorKey )
//                           E_COLOR_PLANE planes, bool bPreserveAspect, bool bSharpen) const
{
   if (src.empty()) return Image();

   // abort
   if ( w < 1 && h < 1 ) return Image();

   // abort
   int src_w = src.w();
   int src_h = src.h();

   if ( src_w == w && src_h == h )
   {
      return src; // copy
   }

   // calculate width from aspect-ratio
   if ( w < 1 ) w = int( float( h )* float( src_w ) / float( src_h ) );

   // calculate height from aspect-ratio
   if ( h < 1 ) h = int( float( w ) * float( src_h ) / float( src_w ) );

    // create new image 32bit ARGB, fill with zero
   Image dst( w, h, src.pixelFormat() );
   if ( dst.empty() ) return dst;

   const float zoomXinv = float( src_w ) / float( w );
   const float zoomYinv = float( src_h ) / float( h );

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         float u = float( x ) * zoomXinv; // interpolate pos of current pixel in img to nearest of src
         float v = float( y ) * zoomYinv; // interpolate pos of current pixel in img to nearest of src
         uint32_t color = getBilinearPixel(src, u, v, colorKey );
         //RGBA_setA( color, 255 );
         dst.setPixel( x, y, color, false );
      }
   }

   return dst;
}

bool
ImageScaler::resizeBicubic( Image & dst, Image const & src, uint32_t colorKey )
{
    if (src.empty()) return false;

    // abort
    int w = dst.w();
    int h = dst.h();
    if ( w < 1 && h < 1 ) return false;

    // abort
    int src_w = src.w();
    int src_h = src.h();
    if ( src_w == w && src_h == h ) return false;

    // calculate width from aspect-ratio
    if ( w < 1 ) w = int( float( h )* float( src_w ) / float( src_h ) );

    // calculate height from aspect-ratio
    if ( h < 1 ) h = int( float( w ) * float( src_h ) / float( src_w ) );

    // create new image 32bit ARGB, fill with zero
    //Image dst( w, h, src.getFormat() );
    //if ( dst.empty() ) return dst;

    const float zoomXinv = float( src_w ) / float( w );
    const float zoomYinv = float( src_h ) / float( h );

    for ( int32_t y = 0; y < h; ++y )
    {
        for ( int32_t x = 0; x < w; ++x )
        {
            float u = float( x ) * zoomXinv; // interpolate pos of current pixel in img to nearest of src
            float v = float( y ) * zoomYinv; // interpolate pos of current pixel in img to nearest of src
            uint32_t color = getBicubicPixel( src, u, v, &colorKey );
            dst.setPixel( x, y, color, false );
        }
    }

    return true;
}

Image
ImageScaler::resizeBicubic( Image const & src, int w, int h, uint32_t colorKey )
//                           E_COLOR_PLANE planes, bool bPreserveAspect, bool bSharpen) const
{
   if (src.empty()) return Image();

   // abort
   if ( w < 1 && h < 1 ) return Image();

   // abort
   int src_w = src.w();
   int src_h = src.h();

   if ( src_w == w && src_h == h )
   {
      return src; // copy
   }

   // calculate width from aspect-ratio
   if ( w < 1 ) w = int( float( h )* float( src_w ) / float( src_h ) );

   // calculate height from aspect-ratio
   if ( h < 1 ) h = int( float( w ) * float( src_h ) / float( src_w ) );

    // create new image 32bit ARGB, fill with zero
   Image dst( w, h, src.pixelFormat() );
   if ( dst.empty() ) return dst;

   const float zoomXinv = float( src_w ) / float( w );
   const float zoomYinv = float( src_h ) / float( h );

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         float u = float( x ) * zoomXinv; // interpolate pos of current pixel in img to nearest of src
         float v = float( y ) * zoomYinv; // interpolate pos of current pixel in img to nearest of src
         uint32_t color = getBicubicPixel( src, u, v, &colorKey );
         //RGBA_setA( color, 255 );
         dst.setPixel( x, y, color, false );
      }
   }

   return dst;
}



// static
uint32_t
ImageScaler::getNearestPixel( Image const & src, float fx, float fy, uint32_t colorKey )
{
   int32_t const i = dbRound32( fx );
   int32_t const j = dbRound32( fy );
   return src.getPixel(i,j);
}


/*
// static
uint32_t
ImageScaler::getBilinearPixel( IImage* src, float fx, float fy, E_COLOR_PLANE planes, uint32_t* colorKey)
{
   int32_t const i= (int32_t)fx;     // dirty round (fast)
   int32_t const j= (int32_t)fy;     // dirty round (fast)
   const float x=fx-(float)i;   // dirty difference from pixel (fast)
   const float y=fy-(float)j;   // dirty difference from pixel (fast)
   const float e=(1.0f-x);    // A+AB*t = B+AB*(1-t) = B-AB*t = B+BA*t
   const float f=(1.0f-y);    // A+AB*t = B+AB*(1-t) = B-AB*t = B+BA*t
   uint32_t const & A=src.getPixel( i,   j); // read pixel, even if out of bounds (gets most nearest pixel)
   uint32_t const & B=src.getPixel( i+1, j); // read pixel, even if out of bounds (gets most nearest pixel)
   uint32_t const & C=src.getPixel( i+1, j+1); // read pixel, even if out of bounds (gets most nearest pixel)
   uint32_t const & D=src.getPixel( i,   j+1); // read pixel, even if out of bounds (gets most nearest pixel)

    if (colorKey && ((A==*colorKey) || (B==*colorKey) || (C==*colorKey) || (D==*colorKey)))
        return *colorKey;

    uint32_t a(0),r(0),g(0),b(0);

    if (planes & ECP_ALPHA)
    {   float fa = A.getAlpha()*e*f+B.getAlpha()*x*f+C.getAlpha()*x*y+D.getAlpha()*e*y;
        a = (uint32_t)core::s32_clamp( (s32)dbRound32(fa), 0, 255);
    }
    if (planes & ECP_RED)
    {   float fr = A.getRed()*e*f+B.getRed()*x*f+C.getRed()*x*y+D.getRed()*e*y;
        r = (uint32_t)core::s32_clamp( (s32)dbRound32(fr), 0, 255);
    }
    if (planes & ECP_GREEN)
    {   float fg = A.getGreen()*e*f+B.getGreen()*x*f+C.getGreen()*x*y+D.getGreen()*e*y;
        g = (uint32_t)core::s32_clamp( (s32)dbRound32(fg), 0, 255);
    }
    if (planes & ECP_BLUE)
    {   float fb = A.getBlue()*e*f+B.getBlue()*x*f+C.getBlue()*x*y+D.getBlue()*e*y;
        b = (uint32_t)core::s32_clamp( (s32)dbRound32(fb), 0, 255);
    }
    return uint32_t(a,r,g,b);
}
*/
// static
uint32_t
ImageScaler::getBilinearPixel( Image const & src, float fx, float fy, uint32_t colorKey )
{
   int32_t const i = int32_t( fx ); // floor()
   int32_t const j = int32_t( fy ); // floor()
   const float x = fx - float( i ); // fpart()
   const float y = fy - float( j ); // fpart()
   int32_t const ii = ( i+1 >= src.w() ) ? i : i+1;
   int32_t const jj = ( j+1 >= src.h() ) ? j : j+1;
   uint32_t const cA = src.getPixel( i,  j,  colorKey );
   uint32_t const cB = src.getPixel( ii, j,  colorKey );
   uint32_t const cC = src.getPixel( ii, jj, colorKey );
   uint32_t const cD = src.getPixel( i,  jj, colorKey );
   const float rA = (float)dbRGBA_R( cA );
   const float rB = (float)dbRGBA_R( cB );
   const float rC = (float)dbRGBA_R( cC );
   const float rD = (float)dbRGBA_R( cD );
   const float gA = (float)dbRGBA_G( cA );
   const float gB = (float)dbRGBA_G( cB );
   const float gC = (float)dbRGBA_G( cC );
   const float gD = (float)dbRGBA_G( cD );
   const float bA = (float)dbRGBA_B( cA );
   const float bB = (float)dbRGBA_B( cB );
   const float bC = (float)dbRGBA_B( cC );
   const float bD = (float)dbRGBA_B( cD );
   const float e = (1.0f-x);
   const float f = (1.0f-y);
   const float fr = (rA*e*f) + (rB*x*f) + (rC*x*y) + (rD*e*y);
   const float fg = (gA*e*f) + (gB*x*f) + (gC*x*y) + (gD*e*y);
   const float fb = (bA*e*f) + (bB*x*f) + (bC*x*y) + (bD*e*y);
   return dbRGBA( int( fr ), int( fg ), int( fb ), 255);
}


// static
float
ImageScaler::getBicubicFactor( const float arg )
{
   const float f1 = arg>0.0f?arg:0.0f;
   const float f2 = arg-1.0f;
   const float f3 = arg+1.0f;
   const float f4 = arg+2.0f;
   const float n1 = f2>0.0f?f2:0.0f;
   const float n2 = f3>0.0f?f3:0.0f;
   const float n3 = f4>0.0f?f4:0.0f;
   return (n3*n3*n3 - 4.0f*n2*n2*n2 + 6.0f*f1*f1*f1 - 4.0f*n1*n1*n1) / 6.0f;
}

uint32_t
ImageScaler::getBicubicPixel( Image const & src, float fx, float fy, uint32_t* colorKey )
{
   // abort, if nothing todo
   // if (planes == ECP_NONE) return ((colorKey) ? (*colorKey) : uint32_t(0,0,0,0));

   // calculate 4x4 indices
   int32_t const x1= (int32_t)fx-(fx>=0?0:1);
   int32_t const x2=x1-1;
   int32_t const x3=x1+1;
   int32_t const x4=x3+1;
   int32_t const y1= (int32_t)fy-(fy>=0?0:1);
   int32_t const y2=y1-1;
   int32_t const y3=y1+1;
   int32_t const y4=y3+1;
   const float dx=fx-x1;
   const float dy=fy-y1;

   // 4x4 Pixels for bicubic Interpolation
   uint32_t const C0 = src.getPixel( x2,y2);	uint32_t const C1 = src.getPixel( x1,y2 );
   uint32_t const C2 = src.getPixel( x3,y2);	uint32_t const C3 = src.getPixel( x4,y2 );
   uint32_t const C4 = src.getPixel( x2,y1);	uint32_t const C5 = src.getPixel( x1,y1 );
   uint32_t const C6 = src.getPixel( x3,y1);	uint32_t const C7 = src.getPixel( x4,y1 );
   uint32_t const C8 = src.getPixel( x2,y3);	uint32_t const C9 = src.getPixel( x1,y3 );
   uint32_t const CA = src.getPixel( x3,y3);	uint32_t const CB = src.getPixel( x4,y3 );
   uint32_t const CC = src.getPixel( x2,y4);	uint32_t const CD = src.getPixel( x1,y4 );
   uint32_t const CE = src.getPixel( x3,y4);	uint32_t const CF = src.getPixel( x4,y4 );

   // abort, if nothing todo
   // --> return ColorKey as dst, if min 1 out of 16 pixel equals ColorKey --> sharp edges, but some pixel are lost
//   if ( colorKey &&
//         ((C0==*colorKey) || (C1==*colorKey) || (C2==*colorKey) || (C3==*colorKey) ||
//          (C4==*colorKey) || (C5==*colorKey) || (C6==*colorKey) || (C7==*colorKey) ||
//          (C8==*colorKey) || (C9==*colorKey) || (CA==*colorKey) || (CB==*colorKey) ||
//          (CC==*colorKey) || (CD==*colorKey) || (CE==*colorKey) || (CF==*colorKey)) )
//   {
//      return *colorKey;
//   }

   // calculate factors
   const float Fx1 = getBicubicFactor(-1.0f-dx );	const float Fx2 = getBicubicFactor( 0.0f+dx );
   const float Fx3 = getBicubicFactor( 1.0f-dx );	const float Fx4 = getBicubicFactor( 2.0f-dx );
   const float Fy1 = getBicubicFactor( dy+1.0f );	const float Fy2 = getBicubicFactor( dy+0.0f );
   const float Fy3 = getBicubicFactor( dy-1.0f );	const float Fy4 = getBicubicFactor( dy-2.0f );

   // calculate final color, default uint32_t(0,0,0,0)
   int32_t iR(0), iG(0), iB(0), iA(0);

    auto toByte = [] ( float v ) { return std::clamp( int32_t( std::lround( v ) ), 0, 255 ); };
	
//   if (planes & ECP_ALPHA)
//   {
      float fa =
      Fx1*Fy1*dbRGBA_A( C0 ) + Fx2*Fy1*dbRGBA_A( C1 ) + Fx3*Fy1*dbRGBA_A( C2 ) + Fx4*Fy1*dbRGBA_A( C3 ) +
      Fx1*Fy2*dbRGBA_A( C4 ) + Fx2*Fy2*dbRGBA_A( C5 ) + Fx3*Fy2*dbRGBA_A( C6 ) + Fx4*Fy2*dbRGBA_A( C7 ) +
      Fx1*Fy3*dbRGBA_A( C8 ) + Fx2*Fy3*dbRGBA_A( C9 ) + Fx3*Fy3*dbRGBA_A( CA ) + Fx4*Fy3*dbRGBA_A( CB ) +
      Fx1*Fy4*dbRGBA_A( CC ) + Fx2*Fy4*dbRGBA_A( CD ) + Fx3*Fy4*dbRGBA_A( CE ) + Fx4*Fy4*dbRGBA_A( CF );
      iA = toByte( fa );
//   }

		

//   if (planes & ECP_RED)
//   {
      float fr =
      Fx1*Fy1*dbRGBA_R( C0 ) + Fx2*Fy1*dbRGBA_R( C1 ) + Fx3*Fy1*dbRGBA_R( C2 ) + Fx4*Fy1*dbRGBA_R( C3 ) +
      Fx1*Fy2*dbRGBA_R( C4 ) + Fx2*Fy2*dbRGBA_R( C5 ) + Fx3*Fy2*dbRGBA_R( C6 ) + Fx4*Fy2*dbRGBA_R( C7 ) +
      Fx1*Fy3*dbRGBA_R( C8 ) + Fx2*Fy3*dbRGBA_R( C9 ) + Fx3*Fy3*dbRGBA_R( CA ) + Fx4*Fy3*dbRGBA_R( CB ) +
      Fx1*Fy4*dbRGBA_R( CC ) + Fx2*Fy4*dbRGBA_R( CD ) + Fx3*Fy4*dbRGBA_R( CE ) + Fx4*Fy4*dbRGBA_R( CF );
      iR = toByte( fr );
//   }

//   if (planes & ECP_GREEN)
//   {
      float fg =
      Fx1*Fy1*dbRGBA_G( C0 ) + Fx2*Fy1*dbRGBA_G( C1 ) + Fx3*Fy1*dbRGBA_G( C2 ) + Fx4*Fy1*dbRGBA_G( C3 ) +
      Fx1*Fy2*dbRGBA_G( C4 ) + Fx2*Fy2*dbRGBA_G( C5 ) + Fx3*Fy2*dbRGBA_G( C6 ) + Fx4*Fy2*dbRGBA_G( C7 ) +
      Fx1*Fy3*dbRGBA_G( C8 ) + Fx2*Fy3*dbRGBA_G( C9 ) + Fx3*Fy3*dbRGBA_G( CA ) + Fx4*Fy3*dbRGBA_G( CB ) +
      Fx1*Fy4*dbRGBA_G( CC ) + Fx2*Fy4*dbRGBA_G( CD ) + Fx3*Fy4*dbRGBA_G( CE ) + Fx4*Fy4*dbRGBA_G( CF );
      iG = toByte( fg );
//   }

//   if (planes & ECP_BLUE)
//   {
      float fb =
      Fx1*Fy1*dbRGBA_B( C0 ) + Fx2*Fy1*dbRGBA_B( C1 ) + Fx3*Fy1*dbRGBA_B( C2 ) + Fx4*Fy1*dbRGBA_B( C3 ) +
      Fx1*Fy2*dbRGBA_B( C4 ) + Fx2*Fy2*dbRGBA_B( C5 ) + Fx3*Fy2*dbRGBA_B( C6 ) + Fx4*Fy2*dbRGBA_B( C7 ) +
      Fx1*Fy3*dbRGBA_B( C8 ) + Fx2*Fy3*dbRGBA_B( C9 ) + Fx3*Fy3*dbRGBA_B( CA ) + Fx4*Fy3*dbRGBA_B( CB ) +
      Fx1*Fy4*dbRGBA_B( CC ) + Fx2*Fy4*dbRGBA_B( CD ) + Fx3*Fy4*dbRGBA_B( CE ) + Fx4*Fy4*dbRGBA_B( CF );
      iB = toByte( fb );
//   }

   // final color
   return dbRGBA( iR,iG,iB,iA );
}



// ===========================================================================

// static
ImageResizef::V4
ImageResizef::getNearestPixelf( const Image & src, float fx, float fy, const V4& colorKey )
{
    const auto i = std::lround( fx );
    const auto j = std::lround( fy );
    return src.getPixel4f(i,j);
}

// static
ImageResizef::V4
ImageResizef::getBilinearPixelf( const Image & src, float fx, float fy, const V4& colorKey )
{
    const int32_t i = int32_t( fx ); // floor()
    const int32_t j = int32_t( fy ); // floor()
    const float x = fx - float( i ); // fpart()
    const float y = fy - float( j ); // fpart()
    const int32_t ii = ( i+1 >= src.w() ) ? i : i+1;
    const int32_t jj = ( j+1 >= src.h() ) ? j : j+1;
    const V4 A = src.getPixel4f( i,  j,  colorKey );
    const V4 B = src.getPixel4f( ii, j,  colorKey );
    const V4 C = src.getPixel4f( ii, jj, colorKey );
    const V4 D = src.getPixel4f( i,  jj, colorKey );
    const float e = (1.0f-x);
    const float f = (1.0f-y);
    // const float r = (A.r*e*f) + (B.r*x*f) + (C.r*x*y) + (D.r*e*y);
    // const float g = (A.g*e*f) + (B.g*x*f) + (C.g*x*y) + (D.g*e*y);
    // const float b = (A.b*e*f) + (B.b*x*f) + (C.b*x*y) + (D.b*e*y);
    // const float a = (A.a*e*f) + (B.a*x*f) + (C.a*x*y) + (D.a*e*y);
    // return V4(r,g,b,a);
    return (A*(e*f)) + (B*(x*f)) + (C*(x*y)) + (D*(e*y));
}

// int scaleOp = 0 - nearest
// int scaleOp = 1 - bilinear
// int scaleOp = 2 - bicubic
// static
Image
ImageResizef::resizef( const Image & src, int w, int h, int scaleOp, const V4& colorKey )
{
    switch( scaleOp )
    {
    case 0: return resizeNearestf( src, w, h, colorKey );
    case 1: return resizeBilinearf( src, w, h, colorKey );
    //case 3: return resizeBicubicf( src, w, h, colorKey );
    //case 4: return resizeLanczosf( src, w, h, colorKey );
    default: return resizeBilinearf( src, w, h, colorKey );
    }
}

// static
Image
ImageResizef::resizeNearestf( const Image & src, int w, int h, const V4& colorKey )
{
    if (src.empty()) return Image();

    if ( w < 1 && h < 1 ) return Image();

    int src_w = src.w();
    int src_h = src.h();

    if ( src_w == w && src_h == h )
    {
        return src; // copy
    }

    // calculate width from aspect-ratio
    if ( w < 1 ) w = int( float( h )* float( src_w ) / float( src_h ) );

    // calculate height from aspect-ratio
    if ( h < 1 ) h = int( float( w ) * float( src_h ) / float( src_w ) );

    // Create output image:
    Image dst( w, h, src.pixelFormat() );

    if ( dst.empty() ) return dst;

    const float zoomXinv = float( src_w ) / float( w );
    const float zoomYinv = float( src_h ) / float( h );

    for ( int32_t y = 0; y < h; ++y )
    {
        for ( int32_t x = 0; x < w; ++x )
        {
            const int32_t u = std::lround( float( x ) * zoomXinv ); // interpolate pos of current pixel in img to nearest of src
            const int32_t v = std::lround( float( y ) * zoomYinv ); // interpolate pos of current pixel in img to nearest of src
            const V4 color = src.getPixel4f( u, v, colorKey );
            dst.setPixel4f( x, y, color, false );
        }
    }
    return dst;
}

// static
Image
ImageResizef::resizeBilinearf( const Image & src, int w, int h, const V4& colorKey )
{
    if (src.empty()) return Image();

    if ( w < 1 && h < 1 ) return Image();

    int src_w = src.w();
    int src_h = src.h();

    if ( src_w == w && src_h == h )
    {
        return src; // copy
    }

    // calculate width from aspect-ratio
    if ( w < 1 ) w = int( float( h )* float( src_w ) / float( src_h ) );

    // calculate height from aspect-ratio
    if ( h < 1 ) h = int( float( w ) * float( src_h ) / float( src_w ) );

    // Create output image:
    Image dst( w, h, src.pixelFormat() );

    if ( dst.empty() ) return dst;

    const float zoomXinv = float( src_w ) / float( w );
    const float zoomYinv = float( src_h ) / float( h );

    for ( int32_t y = 0; y < h; ++y )
    {
        for ( int32_t x = 0; x < w; ++x )
        {
            const float u = float( x ) * zoomXinv; // interpolate pos of current pixel in img to nearest of src
            const float v = float( y ) * zoomYinv; // interpolate pos of current pixel in img to nearest of src
            const V4 color = getBilinearPixelf(src, u, v, colorKey );
            dst.setPixel4f( x, y, color, false );
        }
    }

    return dst;
}


// ===========================================================================
// ===   ImageCutter
// ===========================================================================
Image
ImageCutter::copyRegionRect( Image const & src, Recti region )
{
   Image dst( region.w, region.h, src.pixelFormat() );
   //dst.fill( clearColor );

   for ( int y = 0; y < dst.h(); ++y )
   {
      for ( int x = 0; x <= dst.w(); ++x )
      {
         uint32_t src_color = src.getPixel( x + region.x, y + region.y );
         dst.setPixel( x, y, src_color );
      }
   }

   return dst;
}

// static
Image
ImageCutter::copyRegionHexagon( Image const & src, Recti region, uint32_t clearColor )
{
   Image dst = copyRegionRect( src, region );

   // =========================================================================
   // @brief The Hexagon ( ver_2018 ):
   // =========================================================================
   //
   //               M| x=0 | y=0 | z=0 | u=.5| v=.5|
   //       D       -|-----|-----|-----|-----|-----|
   //      / \      A|   0 |  0  | -.5 | .5  | 0   |
   //   C /   \ E   B| -.5 |  0  |-.25 |  0  | 0.25|
   //    |-----|    C| -.5 |  0  | .25 |  0  | 0.75|
   //    |  M  |    D| 0.0 |  0  |  .5 |  .5 | 1   |
   //    |-----|    E| 0.5 |  0  | .25 |  1  | 0.75|
   //   B \   / F   F| 0.5 |  0  |-.25 |  1  | 0.25|
   //      \ /
   //       A       triangles: ABF, BCE, BEF, CDE
   //
//   int x0 = -1;
//   int x1 = dst.w / 2;
//   int x2 = dst.w;

//   int y0 = -1;
//   int y1 = dst.h / 4 - 1;
//   int y2 = (3 * dst.h) / 4 + 1;
//   int y3 = dst.h;

//   glm::ivec2 A( x1, y3 );
//   glm::ivec2 B( x0, y2 );
//   glm::ivec2 C( x0, y1 );
//   glm::ivec2 D( x1, y0 );
//   glm::ivec2 E( x2, y1 );
//   glm::ivec2 F( x2, y2 );
//   glm::ivec2 TL( x0,y0 );
//   glm::ivec2 TR( x2,y0 );
//   glm::ivec2 BL( x0,y3 );
//   glm::ivec2 BR( x2,y3 );

   ImagePainter::drawTriangle( dst,
      glm::ivec2( -1, -1 ),
      glm::ivec2( dst.w() / 2, -1 ),
      glm::ivec2( -1, dst.h() / 4 - 1 ), clearColor ); // TL,D,C

   ImagePainter::drawTriangle( dst,
      glm::ivec2( dst.w() / 2, -1 ),
      glm::ivec2( dst.w(), -1 ),
      glm::ivec2( dst.w(), dst.h() / 4 - 1 ), clearColor ); // D,TR,E

   ImagePainter::drawTriangle( dst,
      glm::ivec2( dst.w(), (3*dst.h()) / 4 + 1 ),
      glm::ivec2( dst.w(), dst.h()),
      glm::ivec2( dst.w() / 2, dst.h() ), clearColor ); // F, BR, A

   ImagePainter::drawTriangle( dst,
      glm::ivec2( -1, (3 * dst.h()) / 4 + 1 ),
      glm::ivec2( dst.w() / 2, dst.h() ),
      glm::ivec2( -1, dst.h() ), clearColor ); // B, A, BL

   return dst;
}

// static
Image
ImageCutter::copyRegionEllipse( Image const & src, Recti region, uint32_t clearColor )
{
   Image dst( region.w, region.h, src.pixelFormat() );
   dst.fill( clearColor );

   for ( int y = 0; y < dst.h(); ++y )
   {
      for ( int x = 0; x <= dst.w(); ++x )
      {
         uint32_t src_color = src.getPixel( x + region.x, y + region.y );
         dst.setPixel( x, y, src_color );
      }
   }

   return dst;

}


} // end namespace de.

