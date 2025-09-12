#include <de/AsciiImage.hpp>

#if 0 
#include <de/Bresenham.hpp>

namespace de {

// ===========================================================================
// ===   AsciiArt
// ===========================================================================

// static
void
AsciiArt::test()
{
   AsciiImage img( 64, 32 );
   img.fill('+');
//   img.drawLine( 1, 1, 62, 30, '1' );
//   img.drawLine( 1, 30, 62, 1, '2' );
//   img.drawLineRect( 2, 2, 17, 25, '5' );
//   img.drawLineRect( 3, 3, 16, 24, '4' );
//   img.drawLineTriangle( 3, 3, 16, 16, 8, 24, '3' );
//   img.drawTriangle( 32,14, 5,3, 50,3, 'A', 'B', 'C' );
//   img.drawTriangle( 32,28, 40,28, 36, 18, '6', '7', '8' );
//   img.drawTriangle( 63,10, 30,30, 60,30 , '6' );
//   img.drawTriangle( 63,10, 30,30, 50,30 , '7' );
   // Triangle ABC
   img.drawFilledTriangle( 63,10, 30,30, 40,30 , '#' );
//   img.drawCircle( 3, 29, 3, 'C' );
//   img.drawLineCircle( 3, 29, 3, 'I' );
//   DE_ERROR(img.toString())
//   img.fill('.');
//   img.drawLine( 1, 1, 62, 30, '1' );
//   img.drawLine( 1, 30, 62, 1, '2' );
//   img.drawLineCircle( 30, 24, 8, 'G' );
//   img.drawCircle( 30, 24, 7, 'F' );
//   img.drawCircle( 30, 24, 6, 'E' );
//   img.drawCircle( 30, 24, 5, 'D' );
//   img.drawCircle( 30, 24, 4, 'C' );
//   img.drawCircle( 30, 24, 2, 'B' );
//   img.drawCircle( 30, 24, 1, 'A' );
//   img.drawCircle( 50, 20, 10, 'C' );
//   img.drawLineCircle( 50, 20, 10, 'I' );
//   img.drawCircle( 60, 3, 3, 'C' );
//   img.drawLineCircle( 60, 3, 3, 'I' );
//   img.drawCircle( 3, 29, 3, 'C' );
//   img.drawLineCircle( 3, 29, 3, 'I' );
   DE_INFO(img.toString())
}


// static
AsciiImage*
AsciiImage::create( int32_t w, int32_t h, char fillChar )
{
   AsciiImage* img = new AsciiImage( w, h, fillChar );
   return img;
}

AsciiImage::AsciiImage()
   : m_Width( 0 ), m_Height( 0 )
{
   // DE_DEBUG(toString())
}

AsciiImage::AsciiImage( int32_t w, int32_t h, char fillChar )
   : m_Width( 0 )
   , m_Height( 0 )
{
   resize( w, h, fillChar );
   // DE_DEBUG(toString())
}

AsciiImage::~AsciiImage()
{
   // DE_DEBUG(toString())
}


void
AsciiImage::floodFill(int32_t x,
                     int32_t y,
                     char newColor,
                     char oldColor )
{
   double const sec0 = dbSeconds();

   int32_t const w = m_Width;
   int32_t const h = m_Height;

   //DWORD oldColor=MemblockReadDword(mem,memX,memY,x,y);
   if ( getPixel( x,y ) == newColor ) return; // nothing todo

   int32_t lg, rg;
   int32_t px = x;

   while ( getPixel( x,y ) == oldColor )
   {
      setPixel( x,y, newColor );
      x--;
   }
   lg = x+1;
   x = px+1;
   while ( getPixel( x,y ) == oldColor )
   {
      setPixel( x,y, newColor );
      x++;
   }
   rg = x-1;
   for ( x = rg; x >= lg; --x )
   {
      if ( ( getPixel( x,y-1 ) == oldColor ) && ( y > 1 ) )
      {
         floodFill( x, y-1, newColor, oldColor );
      }
      if ( ( getPixel( x,y+1 ) == oldColor ) && ( y < h - 1 ) )
      {
         floodFill( x, y+1, newColor, oldColor );
      }
   }

   double const sec1 = dbSeconds();
   double const ms = 1000.0*(sec1 - sec0);
   DE_INFO( "w = ", w, ", h = ", h, ", ms = ", ms )
}

void
AsciiImage::resize( int32_t w, int32_t h, char fillChar )
{
   size_t const pixelCount = size_t( w ) * size_t( h );
   if ( pixelCount < 1 ) return;
   m_Data.resize( pixelCount, fillChar );
   m_Width = w;
   m_Height = h;
}

void
AsciiImage::drawLineH( int32_t x1, int32_t x2, int32_t y, char color )
{
   auto setPixel = [=] ( int32_t x, int32_t y ) { this->setPixel( x, y, color ); };
   Bresenham::drawLineH( x1, x2, y, setPixel );
}

void
AsciiImage::drawLine( int32_t x1, int32_t y1, int32_t x2, int32_t y2, char color )
{
   auto setPixel = [=] ( int32_t x, int32_t y ) { this->setPixel( x, y, color ); };
   Bresenham::drawLine( x1, y1, x2, y2, setPixel );
}



void
AsciiImage::drawOutlineRect( int32_t x1, int32_t y1,
                         int32_t x2, int32_t y2, char color )
{
   auto setPixel = [=] ( int32_t x, int32_t y ) { this->setPixel( x, y, color ); };
   Bresenham::drawRectBorder( x1, y1, x2, y2, setPixel );
}

void
AsciiImage::drawOutlineCircle( int32_t x, int32_t y, int32_t radius, char color )
{
   auto setPixel = [=] ( int32_t x, int32_t y ) { this->setPixel( x, y, color ); };
   Bresenham::drawCircleBorder( x, y, radius, setPixel );
}

void
AsciiImage::drawFilledCircle( int32_t cx, int32_t cy, int32_t radius, char color )
{
//   auto setPixel = [=] ( int32_t x, int32_t y ) { this->setPixel( x, y, color ); };
//   ImagePainter::drawCircle( x, y, radius, setPixel );
   if ( radius < 1 ) return;
   auto draw2HLines = [&] ( int32_t i, int32_t j )
   {
      drawLineH( cx - i , cx + i, cy - j, color );
      drawLineH( cx - i , cx + i, cy + j, color );
   };
   Bresenham::traverseEllipse( radius, radius, draw2HLines );
}

void
AsciiImage::drawOutlineTriangle( int32_t x1, int32_t y1,
                             int32_t x2, int32_t y2,
                             int32_t x3, int32_t y3, char color )
{
   auto setPixel = [=] ( int32_t x, int32_t y ) { this->setPixel( x, y, color ); };
   Bresenham::drawTriangleBorder( x1, y1, x2, y2, x3, y3, setPixel );
}


void
AsciiImage::drawFilledTriangle( glm::ivec2 const & A, glm::ivec2 const & B, glm::ivec2 const & C, char color )
{
   auto setPixelFill = [=] ( int32_t x, int32_t y ) { this->setPixel( x, y, color ); };
   Bresenham::drawTriangle( A, B, C, setPixelFill );
   this->setPixel( A, 'A' );
   this->setPixel( B, 'B' );
   this->setPixel( C, 'C' );
}

/*
void
AsciiImage::drawImage( Image const & src, int32_t x, int32_t y, bool blend = false )
{
   for ( int j = 0; j < src.getHeight(); ++j )
   {
      for ( int i = 0; i < src.getWidth(); ++i )
      {
         int px = x + i;
         int py = y + j;
         setPixel( px, py, src.getPixel( i, j ), blend );
      }
   }
}
*/

std::string
AsciiImage::toString() const
{
   std::stringstream s;
   s << "AsciiImage(" << m_Width << "," << m_Height << "," << m_FileName << "):\n";
   for ( int32_t y = 0; y < m_Height; ++y )
   {
      s << "y_";
      if ( y < 100 ) s << "_";
      if ( y < 10 ) s << "_";
      s << y << " ";
      for ( int32_t x = 0; x < m_Width; ++x )
      {
         s << getPixel( x, y );
      }
      s << '\n';
   }
   return s.str();
}

char
AsciiImage::getPixel( int32_t x, int32_t y, char colorKey ) const
{
   if ( size_t( x ) > size_t( m_Width ) ) return colorKey;
   if ( size_t( y ) > size_t( m_Height ) ) return colorKey;
   size_t const linear_index = size_t( y ) * size_t( m_Width ) + size_t( x );
   if ( linear_index >= m_Data.size() ) return colorKey;
   return m_Data[ linear_index ];
}

void
AsciiImage::setPixel( int32_t x, int32_t y, char color )
{
   if ( size_t( x ) > size_t( m_Width ) ) return;
   if ( size_t( y ) > size_t( m_Height ) ) return;
   size_t const linear_index = size_t( y ) * size_t( m_Width ) + size_t( x );
   if ( linear_index >= m_Data.size() ) return;
   m_Data[ linear_index ] = color;
}

void
AsciiImage::fill( char fillChar )
{
   for ( size_t i = 0; i < m_Data.size(); ++i )
   {
      m_Data[ i ] = fillChar;
   }
}

char*
AsciiImage::getRow( int32_t y )
{
   if ( y < 0 ) return nullptr;
   if ( y >= m_Height ) return nullptr;
   size_t const linear_index = size_t( y ) * size_t( m_Width );
   if ( linear_index >= m_Data.size() )
   {
      DE_ERROR("Invalid y(",y,") of h(",m_Height,")")
      return nullptr;
   }
   return &m_Data[ linear_index ];
}

char const*
AsciiImage::getRow( int32_t y ) const
{
   return getRow( y );
}

} // end namespace de.


#endif