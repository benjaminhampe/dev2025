#include <de/image/ImagePainter.h>
#include <de/Math.h>

namespace de {


// ==============================================================================

// ==============================================================================

// ===      ImagePainter        =================================================

// ==============================================================================

// ==============================================================================

Font5x8::TextSize ImagePainter::
getTextSize5x8( const std::string& msg, Font5x8 font )
{
    return font.getTextSize( msg );
}

void ImagePainter::
drawText5x8(Image & img, s32 x, s32 y, const std::string& msg,
            u32 color, Font5x8 font, Align align, bool blend )
{
    font.drawText( img, x,y, msg, color, align );
}

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
ImagePainter::drawText( Image & img, s32 x, s32 y, std::wstring const & msg,
                        u32 color, Font font, Align align, bool blend )
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

void ImagePainter::brighten( Image & img, f32 factor )
{
    for ( s32 y = 0; y < img.h(); ++y )
    {
        for ( s32 x = 0; x < img.w(); ++x )
        {
            u32 color = img.getPixel( x,y );
            u8 r = dbClampBytef( factor * f32( dbRGB_R( color ) ) );
            u8 g = dbClampBytef( factor * f32( dbRGB_G( color ) ) );
            u8 b = dbClampBytef( factor * f32( dbRGB_B( color ) ) );
            u8 a = dbRGB_A( color );
            img.setPixel( x,y, dbRGB( r,g,b,a ) );
        }
    }
}

void ImagePainter::
computeSaturation( const Image& img, u32 & color_min, u32 & color_max )
{
   u8 min_r = 255;
   u8 min_g = 255;
   u8 min_b = 255;
   //u8 min_a = 255;
   u8 max_r = 0;
   u8 max_g = 0;
   u8 max_b = 0;
   //u8 max_a = 0;

   for ( s32 y = 0; y < img.h(); ++y )
   {
      for ( s32 x = 0; x < img.w(); ++x )
      {
         u32 color = img.getPixel( x,y );
         u8 r = dbRGB_R( color );
         u8 g = dbRGB_G( color );
         u8 b = dbRGB_B( color );
         //u8 a = RGBA_A( color );
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

   color_min = dbRGB( min_r, min_g, min_b, 255 );
   color_max = dbRGB( max_r, max_g, max_b, 255 );
}

void ImagePainter::autoSaturate( Image & img )
{
    u32 color_min, color_max;

    computeSaturation( img, color_min, color_max );

    // u32 color_delta = color_max - color_min;

    const s32 r1 = dbRGB_R( color_min );
    const s32 g1 = dbRGB_G( color_min );
    const s32 b1 = dbRGB_B( color_min );

    const s32 r2 = dbRGB_R( color_max );
    const s32 g2 = dbRGB_G( color_max );
    const s32 b2 = dbRGB_B( color_max );

    const s32 dR = r2-r1;
    const s32 dG = g2-g1;
    const s32 dB = b2-b1;

    const f32 sR = (dR > 0) ? (255.0f / f32(dR)) : 1.0f;
    const f32 sG = (dG > 0) ? (255.0f / f32(dG)) : 1.0f;
    const f32 sB = (dB > 0) ? (255.0f / f32(dB)) : 1.0f;

    for ( s32 y = 0; y < img.h(); ++y ) {
    for ( s32 x = 0; x < img.w(); ++x ) {
        const u32 color = img.getPixel( x,y );
        const s32 iR = dbRGB_R( color );
        const s32 iG = dbRGB_G( color );
        const s32 iB = dbRGB_B( color );
        const u8 a = dbRGB_A( color );
        const u8 r = std::clamp( r1 + std::lround( sR * (iR - r1) ), 0l, 255l );
        const u8 g = std::clamp( g1 + std::lround( sG * (iG - g1) ), 0l, 255l );
        const u8 b = std::clamp( b1 + std::lround( sB * (iB - b1) ), 0l, 255l );
        img.setPixel( x,y, dbRGB( r,g,b,a ) );
    }
    }
}

void ImagePainter::
replaceColor( Image & img, u32 search_color, u32 replace_color )
{
    for ( s32 y = 0; y < img.h(); ++y ) {
    for ( s32 x = 0; x < img.w(); ++x ) {
        if ( img.getPixel( x,y ) == search_color )
        {
            img.setPixel( x,y, replace_color );
        }
    }
    }
}

void ImagePainter::fill( Image & img, u32 color )
{
   img.fill( color );
}

void ImagePainter::
floodFill( Image & img, s32 x, s32 y, u32 newColor, u32 oldColor )
{
    if ( img.getPixel( x,y ) == newColor ) return;

    s32 w = img.w();
    s32 h = img.h();

    s32 lg, rg, px = x;

    while ( img.getPixel( x,y ) == oldColor )
    {
        img.setPixel( x,y, newColor );
        x--;
    }

    lg = x+1;
    x = px+1;

    while ( img.getPixel( x,y ) == oldColor )
    {
        img.setPixel( x,y, newColor );
        x++;
    }
    rg = x-1;

    for ( x = rg; x >= lg; --x )
    {
        // Move scanline up
        if ( ( img.getPixel( x,y-1 ) == oldColor ) && ( y > 1 ) )
        {
            floodFill( img, x, y-1, newColor, oldColor );
        }
        // Move scanline down
        if ( ( img.getPixel( x,y+1 ) == oldColor ) && ( y < h - 1 ) )
        {
            floodFill( img, x, y+1, newColor, oldColor );
        }
    }
}

u32 ImagePainter::
getPixel( Image & img, s32 x, s32 y, u32 colorKey )
{
   return img.getPixel( x, y, colorKey );
}

void ImagePainter::
setPixel( Image & img, s32 x, s32 y, u32 color, bool blend )
{
   img.setPixel( x, y, color, blend );
}

void ImagePainter::
drawImage( Image & img, const Image& src, s32 x, s32 y, bool blend )
{
    for ( s32 j = 0; j < src.h(); ++j )
    {
        for ( s32 i = 0; i < src.w(); ++i )
        {
            setPixel( img, x + i, y + j, src.getPixel( i, j ), blend );
        }
    }
}

void ImagePainter::
drawLineH( Image & img, s32 x1, s32 x2, s32 y, u32 color, bool blend )
{
    auto setPixel = [&] ( s32 x, s32 y ) { img.setPixel( x, y, color, blend ); };
    Bresenham::traverseLineH( x1, x2, y, setPixel );
}

void ImagePainter::
drawLine( Image & img, s32 x1, s32 y1, s32 x2, s32 y2, u32 color, bool blend )
{
    auto setPixel = [&] ( s32 x, s32 y ) { img.setPixel( x, y, color, blend ); };
    Bresenham::traverseLine( x1, y1, x2, y2, setPixel );
}

void ImagePainter::
drawLine( Image & img, const Recti& r, u32 color, bool blend )
{
    drawLine( img, r.x1(), r.y1(), r.x2(), r.y2(), color, blend );
}

void ImagePainter::
drawLine( de::Image& img, s32 x1, s32 y1, s32 x2, s32 y2, u32 color, bool blend,
          const de::Recti& clipRect )
{
    auto setPixel = [&] (s32 x, s32 y)
    {
        if (clipRect.contains(x,y))
        {
            img.setPixel( x, y, color, blend );
        }
    };
    Bresenham::traverseLine( x1, y1, x2, y2, setPixel );
}

void ImagePainter::
drawSuperCoverLine( de::Image& img, s32 x1, s32 y1, s32 x2, s32 y2, u32 color, bool blend)
{
    auto setPixel = [&] (s32 x, s32 y) { img.setPixel( x, y, color, blend ); };
    Bresenham::traverseSupercoverLine( x1, y1, x2, y2, setPixel );
}

void ImagePainter::
drawSuperCoverLine( de::Image& img, s32 x1, s32 y1, s32 x2, s32 y2, u32 color, bool blend,
                    const de::Recti& clipRect )
{
    auto setPixel = [&] (s32 x, s32 y)
    {
        if (clipRect.contains(x,y))
        {
            img.setPixel( x, y, color, blend );
        }
    };
    Bresenham::traverseSupercoverLine( x1, y1, x2, y2, setPixel );
}


void ImagePainter::
drawLineGrid( Image & img, const Recti& pos, u32 color, bool blend )
{
    drawLineGridX( img, pos, color, blend );
    drawLineGridY( img, pos, color, blend );
}

void ImagePainter::
drawLineGridX( Image & img, const Recti& pos, u32 color, bool blend )
{
   s32 y = 0;
   for ( s32 x = 0; x < img.w(); ++x )
   {
      if ( x % 2 == 1 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   y = 1;
   for ( s32 x = 0; x < img.w(); ++x )
   {
      if ( x % 5 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   y = 2;
   for ( s32 x = 0; x < img.w(); ++x )
   {
      if ( x > 9 && x % 10 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   y = pos.h - 1;
   for ( s32 x = 0; x < img.w(); ++x )
   {
      if ( x % 2 == 1 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   y = pos.h - 2;
   for ( s32 x = 0; x < img.w(); ++x )
   {
      if ( x % 5 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   y = pos.h - 3;
   for ( s32 x = 0; x < img.w(); ++x )
   {
      if ( x > 9 && x % 10 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }
}

void ImagePainter::
drawLineGridY( Image & img, const Recti& pos, u32 color, bool blend )
{
   s32 x = 0;
   for ( s32 y = 0; y < img.h(); ++y )
   {
      if ( y % 2 == 1 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   x = 1;
   for ( s32 y = 0; y < img.h(); ++y )
   {
      if ( y % 5 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   x = 2;
   for ( s32 y = 0; y < img.h(); ++y )
   {
      if ( y > 9 && y % 10 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   x = img.w() - 1;
   for ( s32 y = 0; y < img.h(); ++y )
   {
      if ( y % 2 == 1 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   x = img.w() - 2;
   for ( s32 y = 0; y < img.h(); ++y )
   {
      if ( y % 5 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }

   x = img.w() - 3;
   for ( s32 y = 0; y < img.h(); ++y )
   {
      if ( y > 9 && y % 10 == 0 )
      {
         img.setPixel( x, y, color, blend );
      }
   }
}

void ImagePainter::
drawDot( Image & img, s32 x, s32 y, s32 w, u32 color, bool blend )
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

void ImagePainter::
drawLineMark( Image & img, s32 x, s32 y, u32 color, bool blend )
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
ImagePainter::drawRect(Image & img,
                       const Recti& pos,
                       u32 colorA, u32 colorB,
                       u32 colorC, u32 colorD, bool blend )
{
   s32 x1 = pos.x1();
   s32 y1 = pos.y1();
   s32 x2 = pos.x2();
   s32 y2 = pos.y2();
   s32 w = x2 - x1 + 1;
   s32 h = y2 - y1 + 1;
   if ( w < 2 || h < 2 )
   {
      return;
   }

   glm::vec4 A = dbRGBfv4( colorA );
   glm::vec4 B = dbRGBfv4( colorB );
   glm::vec4 C = dbRGBfv4( colorC );
   glm::vec4 D = dbRGBfv4( colorD );

   for ( s32 j = 0; j < h; ++j )
   {
      for ( s32 i = 0; i < w; ++i )
      {
         f32 u = f32( i ) / f32( w-1 );
         f32 v = f32( j ) / f32( h-1 );

         u32 color = 0x00000000;

         // Oberes Dreieck ABC
         if ( u >= v )
         {
            f32 r = C.r + (1.0f-u) * ( B.r - C.r ) + v * ( D.r - C.r );
            f32 g = C.g + (1.0f-u) * ( B.g - C.g ) + v * ( D.g - C.g );
            f32 b = C.b + (1.0f-u) * ( B.b - C.b ) + v * ( D.b - C.b );
            f32 a = C.a + (1.0f-u) * ( B.a - C.a ) + v * ( D.a - C.a );
            color = dbRGBv4( glm::vec4( r,g,b,a ) );
         }
         // Unteres Dreieck ADC
         else
         {
            f32 r = A.r + u * ( D.r - A.r ) + (1.0f-v) * ( B.r - A.r );
            f32 g = A.g + u * ( D.g - A.g ) + (1.0f-v) * ( B.g - A.g );
            f32 b = A.b + u * ( D.b - A.b ) + (1.0f-v) * ( B.b - A.b );
            f32 a = A.a + u * ( D.a - A.a ) + (1.0f-v) * ( B.a - A.a );
            color = dbRGBv4( glm::vec4( r,g,b,a ) );
         }

         img.setPixel( i+x1,j+y1, color, blend );
      }
   }
}

void ImagePainter::
drawRect( Image & img, const Recti& pos, u32 color, bool blend )
{
   auto setPixel = [&] ( s32 x, s32 y ) { img.setPixel( x, y, color, blend ); };
   Bresenham::traverseRect( pos, setPixel );
}

void ImagePainter::
drawRectBorder( Image & img, const Recti& pos, u32 color, bool blend )
{
   auto setPixel = [&] ( s32 x, s32 y ) { img.setPixel( x, y, color, blend ); };
   Bresenham::traverseRectBorder( pos, setPixel );
}

void ImagePainter::
drawTriangleBorder( Image & img, s32 x1, s32 y1, s32 x2, s32 y2,
                    s32 x3, s32 y3, u32 color, bool blend )
{
    auto setPixel = [&] ( s32 x, s32 y ) { img.setPixel( x, y, color, blend ); };
    Bresenham::traverseTriangleBorder( x1, y1, x2, y2, x3, y3, setPixel );
}

void ImagePainter::
drawTriangle( Image & img, glm::ivec2 const & A, glm::ivec2 const & B, glm::ivec2 const & C, u32 color, bool blend )
{
    auto setPixelFill = [&] ( s32 x, s32 y ) { img.setPixel( x, y, color, blend ); };
    Bresenham::traverseTriangle( A.x, A.y, B.x, B.y, C.x, C.y, setPixelFill );
}

void ImagePainter::
drawCircle( Image & img, const Recti& pos, u32 color, bool blend )
{
   //   drawCircleA( img, pos, color, blend );
   //   drawCircleB( img, pos, color, blend );
   //   drawCircleC( img, pos, color, blend );
   //   drawCircleD( img, pos, color, blend );

   const s32 w = pos.w;
   const s32 h = pos.h;
   const s32 a = w/2;
   const s32 b = h/2;
   s32 cx = pos.x + a;
   s32 cy = pos.y + b;
   //if ( w % 2 == 0 ) { --cx; }
   //if ( h % 2 == 0 ) { --cy; }

   if ( h % 2 == 0 )
   {
      if ( w % 2 == 0 )
      {
         Bresenham::traverseEllipse( a, b,
            [&] ( s32 i, s32 j )
            {
               ImagePainter::drawLineH( img, cx - i, cx + i - 1, cy - j, color, blend );
               ImagePainter::drawLineH( img, cx - i, cx + i - 1, cy + j - 1, color, blend );
            } );
      }
      else
      {
         Bresenham::traverseEllipse( a, b,
            [&] ( s32 i, s32 j )
            {
               ImagePainter::drawLineH( img, cx - i, cx + i, cy - j, color, blend );
               ImagePainter::drawLineH( img, cx - i, cx + i, cy + j - 1, color, blend );
            } );
      }
   }
   else
   {
      Bresenham::traverseEllipse( a, b,
         [&] ( s32 i, s32 j )
         {
            ImagePainter::drawLineH( img, cx - i, cx + i, cy - j, color, blend );
            ImagePainter::drawLineH( img, cx - i, cx + i, cy + j, color, blend );
         } );

   }

   // Draw center
   // img.setPixel( cx, cy, 0xFF0000FF, blend );
}

void ImagePainter::
drawCircleBorder( Image & img, const Recti& pos, u32 color, bool blend )
{
   //   drawCircleBorderA( img, pos, pen, blend );
   //   drawCircleBorderB( img, pos, pen, blend );
   //   drawCircleBorderC( img, pos, pen, blend );
   //   drawCircleBorderD( img, pos, pen, blend );

   const s32 w = pos.w;
   const s32 h = pos.h;
   const s32 a = w/2;
   const s32 b = h/2;
   s32 cx = pos.x + a;
   s32 cy = pos.y + b;
   //if ( w % 2 == 0 ) { --cx; }
   //if ( h % 2 == 0 ) { --cy; }

   if ( h % 2 == 0 )
   {
      if ( w % 2 == 0 )
      {
         Bresenham::traverseEllipse( a, b,
            [&] ( s32 i, s32 j )
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
            [&] ( s32 i, s32 j )
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
         [&] ( s32 i, s32 j )
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

void ImagePainter::
drawCircleA( Image & img, const Recti& pos, u32 color, bool blend )
{
   const s32 w = pos.w;
   const s32 h = pos.h;
   const s32 a = w/2;
   const s32 b = h/2;
   s32 cx = pos.x + a - 1;
   s32 cy = pos.y + b;
   //if ( w % 2 == 0 ) { --cx; }
   //if ( h % 2 == 0 ) { --cy; }
   auto my_drawHLine = [&img,cx,cy,color,blend] ( s32 i, s32 j )
   {
      ImagePainter::drawLineH( img, cx, cx + i, cy + j, color, blend ); // x start is const
   };
   Bresenham::traverseEllipse( a, b, my_drawHLine );
}

void ImagePainter::
drawCircleB( Image & img, const Recti& pos, u32 color, bool blend )
{
   const s32 w = pos.w;
   const s32 h = pos.h;
   const s32 a = w/2;
   const s32 b = h/2;
   s32 cx = pos.x + a;
   s32 cy = pos.y + b;
   //if ( h % 2 == 0 ) { --cy; }
   auto my_drawHLine = [&img,cx,cy,color,blend] ( s32 i, s32 j )
   {
      ImagePainter::drawLineH( img, cx - i, cx, cy + j, color, blend ); // x end is const
   };
   Bresenham::traverseEllipse( a, b, my_drawHLine );
}

void ImagePainter::
drawCircleC( Image & img, const Recti& pos, u32 color, bool blend )
{
   const s32 w = pos.w;
   const s32 h = pos.h;
   const s32 a = w/2;
   const s32 b = h/2;
   s32 cx = pos.x + a;
   s32 cy = pos.y + b;
   auto my_drawHLine = [&img,cx,cy,color,blend] ( s32 i, s32 j )
   {
      ImagePainter::drawLineH( img, cx - i, cx, cy - j, color, blend ); // x is const ( start is const )
   };
   Bresenham::traverseEllipse( a, b, my_drawHLine );
}

void ImagePainter::
drawCircleD( Image & img, const Recti& pos, u32 color, bool blend )
{
   const s32 w = pos.w;
   const s32 h = pos.h;
   const s32 a = w/2;
   const s32 b = h/2;
   s32 cx = pos.x + a - 1;
   s32 cy = pos.y + b;
   //if ( w % 2 == 0 ) { --cx; }
   auto my_drawHLine = [&img,cx,cy,color,blend] ( s32 i, s32 j )
   {
      ImagePainter::drawLineH( img, cx, cx + i, cy - j, color, blend ); // x is const ( start is const )
   };

   Bresenham::traverseEllipse( a, b, my_drawHLine );
}

void ImagePainter::
drawCircleBorderA( Image & img, const Recti& pos, Pen const & pen, bool blend )
{
   const s32 w = pos.w;
   const s32 h = pos.h;
   const s32 a = w/2;
   const s32 b = h/2;
   s32 cx = pos.x + a - 1;
   s32 cy = pos.y + b;
   //if ( w % 2 == 0 ) { --cx; }
   //if ( h % 2 == 0 ) { --cy; }
   auto my_setPixel = [&img,cx,cy,pen,blend] ( s32 i, s32 j )
   {
      img.setPixel( cx + i, cy + j, pen.color, blend ); // x start is const
   };
   Bresenham::traverseEllipse( a, b, my_setPixel );
}

void ImagePainter::
drawCircleBorderB( Image & img, const Recti& pos, Pen const & pen, bool blend )
{
   const s32 w = pos.w;
   const s32 h = pos.h;
   const s32 a = w/2;
   const s32 b = h/2;
   s32 cx = pos.x + a;
   s32 cy = pos.y + b;
   //if ( h % 2 == 0 ) { --cy; }
   auto my_setPixel = [&img,cx,cy,pen,blend] ( s32 i, s32 j )
   {
      img.setPixel( cx - i, cy + j, pen.color, blend ); // x end is const
   };
   Bresenham::traverseEllipse( a, b, my_setPixel );
}

void ImagePainter::
drawCircleBorderC( Image & img, const Recti& pos, Pen const & pen, bool blend )
{
   const s32 w = pos.w;
   const s32 h = pos.h;
   const s32 a = w/2;
   const s32 b = h/2;
   s32 cx = pos.x + a;
   s32 cy = pos.y + b;
   auto my_setPixel = [&img,cx,cy,pen,blend] ( s32 i, s32 j )
   {
      img.setPixel( cx - i, cy - j, pen.color, blend ); // x is const ( start is const )
   };
   Bresenham::traverseEllipse( a, b, my_setPixel );
}

void ImagePainter::
drawCircleBorderD( Image & img, const Recti& pos, Pen const & pen, bool blend )
{
   const s32 w = pos.w;
   const s32 h = pos.h;
   const s32 a = w/2;
   const s32 b = h/2;
   s32 cx = pos.x + a - 1;
   s32 cy = pos.y + b;
   //if ( w % 2 == 0 ) { --cx; }
   auto my_setPixel = [&img,cx,cy,pen,blend] ( s32 i, s32 j )
   {
      img.setPixel( cx + i, cy - j, pen.color, blend ); // x is const ( start is const )
   };

   Bresenham::traverseEllipse( a, b, my_setPixel );
}

void ImagePainter::
drawRoundRect( Image & img, const Recti& pos, s32 rx, s32 ry, u32 color, bool blend )
{
   s32 x1 = pos.x1();
   s32 x2 = pos.x1() + rx;
   s32 x3 = pos.x2() - rx;
   // s32 x4 = pos.x2();

   s32 y1 = pos.y1();
   s32 y2 = pos.y1() + ry;
   s32 y3 = pos.y2() - ry;
   // s32 y4 = pos.y2();

   s32 w = pos.w;
   s32 h = pos.h;

   s32 dx = w - 2*rx;
   s32 dy = h - 2*ry;

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

void ImagePainter::
drawRoundRectBorder( Image & img, const Recti& pos, s32 rx, s32 ry, Pen const & pen, bool blend )
{
   s32 x1 = pos.x1();
   s32 x2 = pos.x1() + rx;
   s32 x3 = pos.x2() - rx;
   s32 x4 = pos.x2();

   s32 y1 = pos.y1();
   s32 y2 = pos.y1() + ry;
   s32 y3 = pos.y2() - ry;
   s32 y4 = pos.y2();

   s32 w = pos.w;
   s32 h = pos.h;

   s32 dx = w - 2*rx;
   s32 dy = h - 2*ry;

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
ImageRepeat::repeatImage( const Image& src, s32 w, s32 h, bool preserveAspect )
{
    DE_ERROR("Not implemented")
    return Image();
}

// ===========================================================================
// ===   ImageRotate
// ===========================================================================

bool ImageRotate::flipH( Image & img )
{
   if ( img.empty() ) return false;

   for ( s32 y = 0; y < img.h(); ++y )
   {
      for ( s32 x = 0; x < img.w() / 2; ++x )
      {
         s32 x_flip = img.w()-1-x;
         u32 color1 = img.getPixel( x,      y );
         u32 color2 = img.getPixel( x_flip, y );
         img.setPixel( x, y, color2 );
         img.setPixel( x_flip, y, color1 );
      }
   }
   return true;
}

bool ImageRotate::flipV( Image & img )
{
   if ( img.empty() ) return false;

   for ( s32 y = 0; y < img.h() / 2; ++y )
   {
      s32 y_flip = img.h()-1-y;
      for ( s32 x = 0; x < img.w(); ++x )
      {
         u32 color1 = img.getPixel( x, y );
         u32 color2 = img.getPixel( x, y_flip );
         img.setPixel( x, y, color2 );
         img.setPixel( x, y_flip, color1 );
      }
   }
   return true;
}

bool ImageRotate::rotate180( Image & img )
{
   if ( img.empty() ) return false;

   for ( s32 y = 0; y < img.h() / 2; ++y )
   {
      for ( s32 x = 0; x < img.w(); ++x )
      {
         s32 x2 = img.w()-1-x;
         s32 y2 = img.h()-1-y;
         u32 color1 = img.getPixel( x, y );
         u32 color2 = img.getPixel( x2, y2 );
         img.setPixel( x, y, color2 );
         img.setPixel( x2, y2, color1 );
      }
   }
   return true;
}

Image ImageRotate::rotateLeft( const Image& img )
{
   if ( img.empty() ) return img;

   Image dst( img.h(), img.w(), img.pixelFormat() );

   for ( s32 y = 0; y < dst.h(); ++y )
   {
      for ( s32 x = 0; x < dst.w(); ++x )
      {
         s32 x2 = dst.w() - 1 - x;
         u32 color = img.getPixel( y, x );
         dst.setPixel( x2, y, color);
      }
   }

   return dst;
}

Image ImageRotate::rotateRight( const Image& img )
{
   if ( img.empty() ) return img;

   Image dst( img.h(), img.w(), img.pixelFormat() );

   for ( s32 y = 0; y < dst.h(); ++y )
   {
      for ( s32 x = 0; x < dst.w(); ++x )
      {
         s32 y2 = img.h() - 1 - y;
         u32 color = img.getPixel( x, y );
         dst.setPixel(y2, x, color);
      }
   }

   return dst;
}

Image ImageRotate::
rotate( const Image& img, f32 angleDegreesCCW, u32 colorKey )
{
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
      const f32 a = ( 360.0f - angleDegreesCCW ) * Math::DEG2RAD;
      const f32 sa = sinf( a );
      const f32 ca = cosf( a );

      // gamma
      const f32 g = angleDegreesCCW * Math::DEG2RAD;
      const f32 sg = sinf( g );
      const f32 cg = cosf( g );

      // BERECHNNUNG des Mittelpunktes von MEM
      const f32 mXh = 0.5f*(f32)img.w();
      const f32 mYh = 0.5f*(f32)img.h();

      // BERECHNNUNG der Eckpunkte
      const f32 Ax = mXh, Ay=-mYh;
      const f32 Bx =-mXh, By=-mYh;
      const f32 Cx =-mXh, Cy= mYh;
      const f32 Dx = mXh, Dy= mYh;

      // TRANSFORMATION der Eckpunkte
      f32 Anx = Ax*ca - Ay*sa;
      f32 Any = Ax*sa + Ay*ca;
      f32 Bnx = Bx*ca - By*sa;
      f32 Bny = Bx*sa + By*ca;
      f32 Cnx = Cx*ca - Cy*sa;
      f32 Cny = Cx*sa + Cy*ca;
      f32 Dnx = Dx*ca - Dy*sa;
      f32 Dny = Dx*sa + Dy*ca;

      // BERECHNUNG der FINAL WIDTH and HEIGHT
      f32 minX = std::min( std::min( Anx, Bnx ), std::min( Cnx, Dnx ) );
      f32 minY = std::min( std::min( Any, Bny ), std::min( Cny, Dny ) );
      f32 maxX = std::max( std::max( Anx, Bnx ), std::max( Cnx, Dnx ) );
      f32 maxY = std::max( std::max( Any, Bny ), std::max( Cny, Dny ) );

//      if (Anx<minX) minX=Anx; if (Any<minY) minY=Any;
//      if (Bnx<minX) minX=Bnx; if (Bny<minY) minY=Bny;
//      if (Cnx<minX) minX=Cnx; if (Cny<minY) minY=Cny;
//      if (Dnx<minX) minX=Dnx; if (Dny<minY) minY=Dny;

//      if (Anx>maxX) maxX=Anx; if (Any>maxY) maxY=Any;
//      if (Bnx>maxX) maxX=Bnx; if (Bny>maxY) maxY=Bny;
//      if (Cnx>maxX) maxX=Cnx; if (Cny>maxY) maxY=Cny;
//      if (Dnx>maxX) maxX=Dnx; if (Dny>maxY) maxY=Dny;

      f32 fdx = maxX - minX;
      f32 fdy = maxY - minY;

      s32 dx = dbRound32( fdx );
      s32 dy = dbRound32( fdy );

//      std::string txt = "New Image with size (";
//      txt += dx;
//      txt += ", ";
//      txt += dy;
//      txt += ")";
      //os::Printer::log(txt.c_str(), ELL_INFORMATION);

      // FINAL MEMBLOCK
      Image dst( dx, dy, img.pixelFormat() );

      // BERECHNUNG des Mittelpunktes von FINAL
      f32 fdxh = 0.5f * fdx;
      f32 fdyh = 0.5f * fdy;

      // LET'S ROTATE
      u32 color;

      // os::Printer::log("OK", ELL_INFORMATION);
      // Fuer alle Punkte im Zielbild den entsprechenden Punkt im Ausgangsbild
      // suchen. Dadurch entstehen keine Luecken im Bild.
      for ( s32 j = 0; j < dy; ++j )
      {
         for ( s32 i = 0; i < dx; ++i )
         {
            if ( ( i < dst.w() ) && ( j < dst.h() ) )
            {
               f32 x0 = (f32)i - fdxh; // relativ zu FinalMem Mittelpunkt
               f32 y0 = (f32)j - fdyh; // relativ zu FinalMem Mittelpunkt
               f32 x1 = (cg*x0-sg*y0)+mXh; // relativ zu SourceMem Mittelpunkt
               f32 y1 = (cg*y0+sg*x0)+mYh; // relativ zu SourceMem Mittelpunkt
               s32 x = dbRound32( x1 );
               s32 y = dbRound32( y1 );

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
void ImageRotate::shiftLeft( Image & img, s32 n_shifts )
{

}

/// Shifts an image by 'n' pixels vertical-lines right (can be negative)
void ImageRotate::shiftRight( Image & img, s32 n_shifts )
{

}

/// Shifts an image by 'n' pixels horizontal-lines up (can be negative)
void ImageRotate::shiftUp( Image & img, s32 n_shifts )
{

}

/// Shifts an image by 'n' pixels horizontal-lines down (can be negative)
void ImageRotate::shiftDown( Image & img, s32 n_shifts )
{

}

Image ImageRotate::skewH( const Image& src, f32 angle )
{
   return src;
}

Image ImageRotate::skewV( const Image& src, f32 angle )
{
   return src;
}

// ===========================================================================
// ===   ImageScaler
// ===========================================================================

Image ImageScaler::
resize( const Image& src, s32 w, s32 h, s32 scaleOp, u32 colorKey )
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

Image ImageScaler::
resizeNearest( const Image& src, s32 w, s32 h, u32 colorKey )
    // s32 color_planes, bool bPreserveAspect, bool bSharpen)
{
   if (src.empty()) return Image();

   // abort
   if ( w < 1 && h < 1 ) return Image();

   s32 src_w = src.w();
   s32 src_h = src.h();
   if ( src_w == w && src_h == h )
   {
      return src; // copy
   }

   // calculate width from aspect-ratio
   if ( w < 1 ) w = s32( f32( h )* f32( src_w ) / f32( src_h ) );

   // calculate height from aspect-ratio
   if ( h < 1 ) h = s32( f32( w ) * f32( src_h ) / f32( src_w ) );

    // create new image 32bit ARGB, fill with zero
   Image dst( w, h, src.pixelFormat() );
   if ( dst.empty() ) return dst;

   const f32 zoomXinv = f32( src_w ) / f32( w );
   const f32 zoomYinv = f32( src_h ) / f32( h );

   // RGB color planes ( alphachannel always 255 == not transparent)
//   if (planes==ECP_RGB)
//   {
   for ( s32 y = 0; y < h; ++y )
   {
      for ( s32 x = 0; x < w; ++x )
      {
         s32 u = s32( f32( x ) * zoomXinv ); // interpolate pos of current pixel in img to nearest of src
         s32 v = s32( f32( y ) * zoomYinv ); // interpolate pos of current pixel in img to nearest of src
         u32 color = src.getPixel( u, v, colorKey );
         dbRGB_setA( color, 255 );
         dst.setPixel( x, y, color, false );
      }
   }
//   }
//    else
//   {
//      for (u32 y=0; y<h; y++)
//        {
//            for (u32 x=0; x<w; x++)
//            {
//                u32 u=(u32)((f32)x/zoomX); // interpolate pos of current pixel in img to nearest of src
//                u32 v=(u32)((f32)y/zoomY); // interpolate pos of current pixel in img to nearest of src
//                u32 color = src.getPixel(u,v);
//                dst->setPixel(x,y,color,false);
//            }
//        }
//   }

   return dst;
}

Image ImageScaler::
resizeBilinear( const Image& src, s32 w, s32 h, u32 colorKey )
    // E_COLOR_PLANE planes, bool bPreserveAspect, bool bSharpen) const
{
   if (src.empty()) return Image();

   // abort
   if ( w < 1 && h < 1 ) return Image();

   // abort
   s32 src_w = src.w();
   s32 src_h = src.h();

   if ( src_w == w && src_h == h )
   {
      return src; // copy
   }

   // calculate width from aspect-ratio
   if ( w < 1 ) w = s32( f32( h )* f32( src_w ) / f32( src_h ) );

   // calculate height from aspect-ratio
   if ( h < 1 ) h = s32( f32( w ) * f32( src_h ) / f32( src_w ) );

    // create new image 32bit ARGB, fill with zero
   Image dst( w, h, src.pixelFormat() );
   if ( dst.empty() ) return dst;

   const f32 zoomXinv = f32( src_w ) / f32( w );
   const f32 zoomYinv = f32( src_h ) / f32( h );

   for ( s32 y = 0; y < h; ++y )
   {
      for ( s32 x = 0; x < w; ++x )
      {
         f32 u = f32( x ) * zoomXinv; // interpolate pos of current pixel in img to nearest of src
         f32 v = f32( y ) * zoomYinv; // interpolate pos of current pixel in img to nearest of src
         u32 color = getBilinearPixel(src, u, v, colorKey );
         //RGBA_setA( color, 255 );
         dst.setPixel( x, y, color, false );
      }
   }

   return dst;
}

bool ImageScaler::
resizeBicubic( Image & dst, const Image& src, u32 colorKey )
{
    if (src.empty()) return false;

    // abort
    s32 w = dst.w();
    s32 h = dst.h();
    if ( w < 1 && h < 1 ) return false;

    // abort
    s32 src_w = src.w();
    s32 src_h = src.h();
    if ( src_w == w && src_h == h ) return false;

    // calculate width from aspect-ratio
    if ( w < 1 ) w = s32( f32( h )* f32( src_w ) / f32( src_h ) );

    // calculate height from aspect-ratio
    if ( h < 1 ) h = s32( f32( w ) * f32( src_h ) / f32( src_w ) );

    // create new image 32bit ARGB, fill with zero
    //Image dst( w, h, src.getFormat() );
    //if ( dst.empty() ) return dst;

    const f32 zoomXinv = f32( src_w ) / f32( w );
    const f32 zoomYinv = f32( src_h ) / f32( h );

    for ( s32 y = 0; y < h; ++y )
    {
        for ( s32 x = 0; x < w; ++x )
        {
            f32 u = f32( x ) * zoomXinv; // interpolate pos of current pixel in img to nearest of src
            f32 v = f32( y ) * zoomYinv; // interpolate pos of current pixel in img to nearest of src
            u32 color = getBicubicPixel( src, u, v, &colorKey );
            dst.setPixel( x, y, color, false );
        }
    }

    return true;
}

Image ImageScaler::
resizeBicubic( const Image& src, s32 w, s32 h, u32 colorKey )
    // E_COLOR_PLANE planes, bool bPreserveAspect, bool bSharpen) const
{
   if (src.empty()) return Image();

   // abort
   if ( w < 1 && h < 1 ) return Image();

   // abort
   s32 src_w = src.w();
   s32 src_h = src.h();

   if ( src_w == w && src_h == h )
   {
      return src; // copy
   }

   // calculate width from aspect-ratio
   if ( w < 1 ) w = s32( f32( h )* f32( src_w ) / f32( src_h ) );

   // calculate height from aspect-ratio
   if ( h < 1 ) h = s32( f32( w ) * f32( src_h ) / f32( src_w ) );

    // create new image 32bit ARGB, fill with zero
   Image dst( w, h, src.pixelFormat() );
   if ( dst.empty() ) return dst;

   const f32 zoomXinv = f32( src_w ) / f32( w );
   const f32 zoomYinv = f32( src_h ) / f32( h );

   for ( s32 y = 0; y < h; ++y )
   {
      for ( s32 x = 0; x < w; ++x )
      {
         f32 u = f32( x ) * zoomXinv; // interpolate pos of current pixel in img to nearest of src
         f32 v = f32( y ) * zoomYinv; // interpolate pos of current pixel in img to nearest of src
         u32 color = getBicubicPixel( src, u, v, &colorKey );
         //RGBA_setA( color, 255 );
         dst.setPixel( x, y, color, false );
      }
   }

   return dst;
}

u32 ImageScaler::
getNearestPixel( const Image& src, f32 fx, f32 fy, u32 colorKey )
{
   const s32 i = dbRound32( fx );
   const s32 j = dbRound32( fy );
   return src.getPixel(i,j);
}

u32 ImageScaler::
getBilinearPixel( const Image& src, f32 fx, f32 fy, u32 colorKey )
{
   const s32 i = s32( fx ); // floor()
   const s32 j = s32( fy ); // floor()
   const f32 x = fx - f32( i ); // fpart()
   const f32 y = fy - f32( j ); // fpart()
   const s32 ii = ( i+1 >= src.w() ) ? i : i+1;
   const s32 jj = ( j+1 >= src.h() ) ? j : j+1;
   const u32 cA = src.getPixel( i,  j,  colorKey );
   const u32 cB = src.getPixel( ii, j,  colorKey );
   const u32 cC = src.getPixel( ii, jj, colorKey );
   const u32 cD = src.getPixel( i,  jj, colorKey );
   const f32 rA = (f32)dbRGB_R( cA );
   const f32 rB = (f32)dbRGB_R( cB );
   const f32 rC = (f32)dbRGB_R( cC );
   const f32 rD = (f32)dbRGB_R( cD );
   const f32 gA = (f32)dbRGB_G( cA );
   const f32 gB = (f32)dbRGB_G( cB );
   const f32 gC = (f32)dbRGB_G( cC );
   const f32 gD = (f32)dbRGB_G( cD );
   const f32 bA = (f32)dbRGB_B( cA );
   const f32 bB = (f32)dbRGB_B( cB );
   const f32 bC = (f32)dbRGB_B( cC );
   const f32 bD = (f32)dbRGB_B( cD );
   const f32 e = (1.0f-x);
   const f32 f = (1.0f-y);
   const f32 fr = (rA*e*f) + (rB*x*f) + (rC*x*y) + (rD*e*y);
   const f32 fg = (gA*e*f) + (gB*x*f) + (gC*x*y) + (gD*e*y);
   const f32 fb = (bA*e*f) + (bB*x*f) + (bC*x*y) + (bD*e*y);
   return dbRGB( s32( fr ), s32( fg ), s32( fb ), 255);
}

f32 ImageScaler::getBicubicFactor( const f32 arg )
{
   const f32 f1 = arg>0.0f?arg:0.0f;
   const f32 f2 = arg-1.0f;
   const f32 f3 = arg+1.0f;
   const f32 f4 = arg+2.0f;
   const f32 n1 = f2>0.0f?f2:0.0f;
   const f32 n2 = f3>0.0f?f3:0.0f;
   const f32 n3 = f4>0.0f?f4:0.0f;
   return (n3*n3*n3 - 4.0f*n2*n2*n2 + 6.0f*f1*f1*f1 - 4.0f*n1*n1*n1) / 6.0f;
}

u32 ImageScaler::
getBicubicPixel( const Image& src, f32 fx, f32 fy, u32* colorKey )
{
   // abort, if nothing todo
   // if (planes == ECP_NONE) return ((colorKey) ? (*colorKey) : u32(0,0,0,0));

   // calculate 4x4 indices
   const s32 x1= (s32)fx-(fx>=0?0:1);
   const s32 x2=x1-1;
   const s32 x3=x1+1;
   const s32 x4=x3+1;
   const s32 y1= (s32)fy-(fy>=0?0:1);
   const s32 y2=y1-1;
   const s32 y3=y1+1;
   const s32 y4=y3+1;
   const f32 dx=fx-x1;
   const f32 dy=fy-y1;

   // 4x4 Pixels for bicubic Interpolation
   const u32 C0 = src.getPixel( x2,y2);	const u32 C1 = src.getPixel( x1,y2 );
   const u32 C2 = src.getPixel( x3,y2);	const u32 C3 = src.getPixel( x4,y2 );
   const u32 C4 = src.getPixel( x2,y1);	const u32 C5 = src.getPixel( x1,y1 );
   const u32 C6 = src.getPixel( x3,y1);	const u32 C7 = src.getPixel( x4,y1 );
   const u32 C8 = src.getPixel( x2,y3);	const u32 C9 = src.getPixel( x1,y3 );
   const u32 CA = src.getPixel( x3,y3);	const u32 CB = src.getPixel( x4,y3 );
   const u32 CC = src.getPixel( x2,y4);	const u32 CD = src.getPixel( x1,y4 );
   const u32 CE = src.getPixel( x3,y4);	const u32 CF = src.getPixel( x4,y4 );

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
   const f32 Fx1 = getBicubicFactor(-1.0f-dx );	const f32 Fx2 = getBicubicFactor( 0.0f+dx );
   const f32 Fx3 = getBicubicFactor( 1.0f-dx );	const f32 Fx4 = getBicubicFactor( 2.0f-dx );
   const f32 Fy1 = getBicubicFactor( dy+1.0f );	const f32 Fy2 = getBicubicFactor( dy+0.0f );
   const f32 Fy3 = getBicubicFactor( dy-1.0f );	const f32 Fy4 = getBicubicFactor( dy-2.0f );

   // calculate final color, default u32(0,0,0,0)
   s32 iR(0), iG(0), iB(0), iA(0);

    auto toByte = [] ( f32 v ) { return std::clamp( s32( std::lround( v ) ), 0, 255 ); };

//   if (planes & ECP_ALPHA)
//   {
      f32 fa =
      Fx1*Fy1*dbRGB_A( C0 ) + Fx2*Fy1*dbRGB_A( C1 ) + Fx3*Fy1*dbRGB_A( C2 ) + Fx4*Fy1*dbRGB_A( C3 ) +
      Fx1*Fy2*dbRGB_A( C4 ) + Fx2*Fy2*dbRGB_A( C5 ) + Fx3*Fy2*dbRGB_A( C6 ) + Fx4*Fy2*dbRGB_A( C7 ) +
      Fx1*Fy3*dbRGB_A( C8 ) + Fx2*Fy3*dbRGB_A( C9 ) + Fx3*Fy3*dbRGB_A( CA ) + Fx4*Fy3*dbRGB_A( CB ) +
      Fx1*Fy4*dbRGB_A( CC ) + Fx2*Fy4*dbRGB_A( CD ) + Fx3*Fy4*dbRGB_A( CE ) + Fx4*Fy4*dbRGB_A( CF );
      iA = toByte( fa );
//   }



//   if (planes & ECP_RED)
//   {
      f32 fr =
      Fx1*Fy1*dbRGB_R( C0 ) + Fx2*Fy1*dbRGB_R( C1 ) + Fx3*Fy1*dbRGB_R( C2 ) + Fx4*Fy1*dbRGB_R( C3 ) +
      Fx1*Fy2*dbRGB_R( C4 ) + Fx2*Fy2*dbRGB_R( C5 ) + Fx3*Fy2*dbRGB_R( C6 ) + Fx4*Fy2*dbRGB_R( C7 ) +
      Fx1*Fy3*dbRGB_R( C8 ) + Fx2*Fy3*dbRGB_R( C9 ) + Fx3*Fy3*dbRGB_R( CA ) + Fx4*Fy3*dbRGB_R( CB ) +
      Fx1*Fy4*dbRGB_R( CC ) + Fx2*Fy4*dbRGB_R( CD ) + Fx3*Fy4*dbRGB_R( CE ) + Fx4*Fy4*dbRGB_R( CF );
      iR = toByte( fr );
//   }

//   if (planes & ECP_GREEN)
//   {
      f32 fg =
      Fx1*Fy1*dbRGB_G( C0 ) + Fx2*Fy1*dbRGB_G( C1 ) + Fx3*Fy1*dbRGB_G( C2 ) + Fx4*Fy1*dbRGB_G( C3 ) +
      Fx1*Fy2*dbRGB_G( C4 ) + Fx2*Fy2*dbRGB_G( C5 ) + Fx3*Fy2*dbRGB_G( C6 ) + Fx4*Fy2*dbRGB_G( C7 ) +
      Fx1*Fy3*dbRGB_G( C8 ) + Fx2*Fy3*dbRGB_G( C9 ) + Fx3*Fy3*dbRGB_G( CA ) + Fx4*Fy3*dbRGB_G( CB ) +
      Fx1*Fy4*dbRGB_G( CC ) + Fx2*Fy4*dbRGB_G( CD ) + Fx3*Fy4*dbRGB_G( CE ) + Fx4*Fy4*dbRGB_G( CF );
      iG = toByte( fg );
//   }

//   if (planes & ECP_BLUE)
//   {
      f32 fb =
      Fx1*Fy1*dbRGB_B( C0 ) + Fx2*Fy1*dbRGB_B( C1 ) + Fx3*Fy1*dbRGB_B( C2 ) + Fx4*Fy1*dbRGB_B( C3 ) +
      Fx1*Fy2*dbRGB_B( C4 ) + Fx2*Fy2*dbRGB_B( C5 ) + Fx3*Fy2*dbRGB_B( C6 ) + Fx4*Fy2*dbRGB_B( C7 ) +
      Fx1*Fy3*dbRGB_B( C8 ) + Fx2*Fy3*dbRGB_B( C9 ) + Fx3*Fy3*dbRGB_B( CA ) + Fx4*Fy3*dbRGB_B( CB ) +
      Fx1*Fy4*dbRGB_B( CC ) + Fx2*Fy4*dbRGB_B( CD ) + Fx3*Fy4*dbRGB_B( CE ) + Fx4*Fy4*dbRGB_B( CF );
      iB = toByte( fb );
//   }

   // final color
   return dbRGB( iR,iG,iB,iA );
}

// ===========================================================================

ImageResizef::V4 ImageResizef::
getNearestPixelf( const Image & src, f32 fx, f32 fy, const V4& colorKey )
{
    const auto i = std::lround( fx );
    const auto j = std::lround( fy );
    return src.getPixel4f(i,j);
}

ImageResizef::V4 ImageResizef::
getBilinearPixelf( const Image & src, f32 fx, f32 fy, const V4& colorKey )
{
    const s32 i = s32( fx ); // floor()
    const s32 j = s32( fy ); // floor()
    const f32 x = fx - f32( i ); // fpart()
    const f32 y = fy - f32( j ); // fpart()
    const s32 ii = ( i+1 >= src.w() ) ? i : i+1;
    const s32 jj = ( j+1 >= src.h() ) ? j : j+1;
    const V4 A = src.getPixel4f( i,  j,  colorKey );
    const V4 B = src.getPixel4f( ii, j,  colorKey );
    const V4 C = src.getPixel4f( ii, jj, colorKey );
    const V4 D = src.getPixel4f( i,  jj, colorKey );
    const f32 e = (1.0f-x);
    const f32 f = (1.0f-y);
    // const f32 r = (A.r*e*f) + (B.r*x*f) + (C.r*x*y) + (D.r*e*y);
    // const f32 g = (A.g*e*f) + (B.g*x*f) + (C.g*x*y) + (D.g*e*y);
    // const f32 b = (A.b*e*f) + (B.b*x*f) + (C.b*x*y) + (D.b*e*y);
    // const f32 a = (A.a*e*f) + (B.a*x*f) + (C.a*x*y) + (D.a*e*y);
    // return V4(r,g,b,a);
    return (A*(e*f)) + (B*(x*f)) + (C*(x*y)) + (D*(e*y));
}

// s32 scaleOp = 0 - nearest
// s32 scaleOp = 1 - bilinear
// s32 scaleOp = 2 - bicubic

Image ImageResizef::
resizef( const Image & src, s32 w, s32 h, s32 scaleOp, const V4& colorKey )
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

Image ImageResizef::
resizeNearestf( const Image & src, s32 w, s32 h, const V4& colorKey )
{
    if (src.empty()) return Image();

    if ( w < 1 && h < 1 ) return Image();

    s32 src_w = src.w();
    s32 src_h = src.h();

    if ( src_w == w && src_h == h )
    {
        return src; // copy
    }

    // calculate width from aspect-ratio
    if ( w < 1 ) w = s32( f32( h )* f32( src_w ) / f32( src_h ) );

    // calculate height from aspect-ratio
    if ( h < 1 ) h = s32( f32( w ) * f32( src_h ) / f32( src_w ) );

    // Create output image:
    Image dst( w, h, src.pixelFormat() );

    if ( dst.empty() ) return dst;

    const f32 zoomXinv = f32( src_w ) / f32( w );
    const f32 zoomYinv = f32( src_h ) / f32( h );

    for ( s32 y = 0; y < h; ++y )
    {
        for ( s32 x = 0; x < w; ++x )
        {
            const s32 u = std::lround( f32( x ) * zoomXinv ); // interpolate pos of current pixel in img to nearest of src
            const s32 v = std::lround( f32( y ) * zoomYinv ); // interpolate pos of current pixel in img to nearest of src
            const V4 color = src.getPixel4f( u, v, colorKey );
            dst.setPixel4f( x, y, color, false );
        }
    }
    return dst;
}

Image ImageResizef::
resizeBilinearf( const Image & src, s32 w, s32 h, const V4& colorKey )
{
    if (src.empty()) return Image();

    if ( w < 1 && h < 1 ) return Image();

    s32 src_w = src.w();
    s32 src_h = src.h();

    if ( src_w == w && src_h == h )
    {
        return src; // copy
    }

    // calculate width from aspect-ratio
    if ( w < 1 ) w = s32( f32( h )* f32( src_w ) / f32( src_h ) );

    // calculate height from aspect-ratio
    if ( h < 1 ) h = s32( f32( w ) * f32( src_h ) / f32( src_w ) );

    // Create output image:
    Image dst( w, h, src.pixelFormat() );

    if ( dst.empty() ) return dst;

    const f32 zoomXinv = f32( src_w ) / f32( w );
    const f32 zoomYinv = f32( src_h ) / f32( h );

    for ( s32 y = 0; y < h; ++y )
    {
        for ( s32 x = 0; x < w; ++x )
        {
            const f32 u = f32( x ) * zoomXinv; // interpolate pos of current pixel in img to nearest of src
            const f32 v = f32( y ) * zoomYinv; // interpolate pos of current pixel in img to nearest of src
            const V4 color = getBilinearPixelf(src, u, v, colorKey );
            dst.setPixel4f( x, y, color, false );
        }
    }

    return dst;
}

// ===========================================================================
// ===   ImageCutter
// ===========================================================================
Image ImageCutter::copyRegionRect( const Image& src, Recti region )
{
   Image dst( region.w, region.h, src.pixelFormat() );
   //dst.fill( clearColor );

   for ( s32 y = 0; y < dst.h(); ++y )
   {
      for ( s32 x = 0; x <= dst.w(); ++x )
      {
         u32 src_color = src.getPixel( x + region.x, y + region.y );
         dst.setPixel( x, y, src_color );
      }
   }

   return dst;
}

Image ImageCutter::
copyRegionHexagon( const Image& src, Recti region, u32 clearColor )
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
//   s32 x0 = -1;
//   s32 x1 = dst.w / 2;
//   s32 x2 = dst.w;

//   s32 y0 = -1;
//   s32 y1 = dst.h / 4 - 1;
//   s32 y2 = (3 * dst.h) / 4 + 1;
//   s32 y3 = dst.h;

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

Image ImageCutter::
copyRegionEllipse( const Image& src, Recti region, u32 clearColor )
{
    Image dst( region.w, region.h, src.pixelFormat() );
    dst.fill( clearColor );

    for ( s32 y = 0; y < dst.h(); ++y )
    {
        for ( s32 x = 0; x <= dst.w(); ++x )
        {
            u32 src_color = src.getPixel( x + region.x, y + region.y );
            dst.setPixel( x, y, src_color );
        }
    }

    return dst;
}

} // end namespace de.
