#include <de/Image.hpp>
#include <de/Bresenham.hpp>

namespace de {

// ===========================================================================
// ===   static
// ===========================================================================

Image*
Image::create( int32_t w, int32_t h, ColorFormat format )
{
   Image* img = new Image( w, h, format );
   return img;
}

Image::Ptr
Image::createPtr( int32_t w, int32_t h, ColorFormat format )
{
   return std::unique_ptr< Image >( new Image( w, h, format ) );
}

Image::SharedPtr
Image::createSharedPtr( int32_t w, int32_t h, ColorFormat format )
{
   return std::make_shared< Image >( w, h, format );
}

// ===========================================================================
// ===   constructor
// ===========================================================================

Image::Image()
   : m_Width( 0 )
   , m_Height( 0 )
   , m_ColorFormat( ColorFormat::RGBA8888 )
   , m_BytesPerPixel( ColorUtil::getBytesPerPixel( m_ColorFormat ) )
{}

Image::Image( int32_t w, int32_t h, ColorFormat format )
   : m_Width( w )
   , m_Height( h )
   , m_ColorFormat( format )
   , m_BytesPerPixel( ColorUtil::getBytesPerPixel( m_ColorFormat ) )
{
   uint64_t const pixelCount = uint64_t( w ) * uint64_t( h );
   uint64_t const byteCount = pixelCount * getBytesPerPixel();
   m_Data.resize( byteCount, 0x00 );
   //DE_DEBUG("w(",w,"), h(",h,"), pixelCount(",pixelCount,")")
}

Image::Image( int32_t w, int32_t h, uint32_t fillColor, ColorFormat format )
   : m_Width( w )
   , m_Height( h )
   , m_ColorFormat( format )
   , m_BytesPerPixel( ColorUtil::getBytesPerPixel( m_ColorFormat ) )
{
   uint64_t const pixelCount = uint64_t( w ) * uint64_t( h );
   uint64_t const byteCount = pixelCount * getBytesPerPixel();
   m_Data.resize( byteCount, 0x00 );
   //DE_DEBUG("w(",w,"), h(",h,"), pixelCount(",pixelCount,")")
   fill( fillColor );
}

// ===========================================================================
// ===   operator==( Image const & other ) const
// ===========================================================================

bool
Image::equals( Image const & other ) const
{
   int w = other.getWidth();
   int h = other.getHeight();
   if ( getWidth() != w ) return false;
   if ( getHeight() != h ) return false;
   if ( getFormat() != other.getFormat() ) return false;

   return 0 == memcmp( data(), other.data(), other.size() );
}

// ===========================================================================
// ===   operator!() const
// ===========================================================================

bool
Image::empty() const
{
   if ( getWidth() < 1 ) { return true; }
   if ( getHeight() < 1 ) { return true; }
   if ( getByteCount() > m_Data.size() ) { return true; }
   return false;
}

uint32_t
Image::getRedBits() const
{
   switch( m_ColorFormat )
   {
      case ColorFormat::RGBA8888: return 8;
      case ColorFormat::RGB888:   return 8;
      case ColorFormat::ARGB1555: return 5;
      case ColorFormat::RGB565:   return 5;
      case ColorFormat::R32F:     return 32;
      case ColorFormat::R8:       return 8;
      case ColorFormat::R16:      return 16;
      case ColorFormat::R24:      return 24;
      case ColorFormat::R32:      return 32;
      default: return 0;
   }
}

uint32_t
Image::getGreenBits() const
{
   switch( m_ColorFormat )
   {
      case ColorFormat::RGBA8888: return 8;
      case ColorFormat::RGB888:   return 8;
      case ColorFormat::ARGB1555: return 5;
      case ColorFormat::RGB565:   return 6;
      default: return 0;
   }
}

uint32_t
Image::getBlueBits() const
{
   switch( m_ColorFormat )
   {
      case ColorFormat::RGBA8888: return 8;
      case ColorFormat::RGB888:   return 8;
      case ColorFormat::ARGB1555: return 5;
      case ColorFormat::RGB565:   return 5;
      default: return 0;
   }
}

uint32_t
Image::getAlphaBits() const
{
   switch( m_ColorFormat )
   {
      case ColorFormat::RGBA8888: return 8;
      case ColorFormat::ARGB1555: return 1;
      default: return 0;
   }
}

void
Image::clear( bool forceShrink )
{
   m_Data.clear();
   if ( forceShrink )
   {
      m_Data.shrink_to_fit(); // here it actually frees memory, could be heavy load.
   }

   m_Width = 0;
   m_Height = 0;
   // Dont touch format
   // Dont touch uri string
}

void
Image::setFormat( ColorFormat colorFormat )
{
   m_ColorFormat = colorFormat;
   m_BytesPerPixel = ColorUtil::getBytesPerPixel( m_ColorFormat );
}

void
Image::setPixel( int32_t x, int32_t y, uint32_t color, bool blend )
{
   if ( m_Width < 1 || m_Height < 1 )
   {
      DE_ERROR("Empty image")
      return;
   }
   uint32_t ux = uint32_t( x );
   uint32_t uy = uint32_t( y );
   uint32_t uw = uint32_t( m_Width );
   uint32_t uh = uint32_t( m_Height );
   if ( ux >= uw )
   {
      //DE_ERROR("Invalid x(",x,") >= w(",m_Width,")")
      return;
   }
   if ( uy >= uh )
   {
      //DE_ERROR("Invalid y(",y,") >= h(",m_Height,")")
      return;
   }
   uint64_t byteOffset = uint64_t( m_Width ) * uy + ux;
   byteOffset *= m_BytesPerPixel;
   if ( byteOffset + m_BytesPerPixel > m_Data.size() )
   {
      DE_ERROR("byteOffset > m_Data.size()")
      return;
   }

   if ( blend )
   {
      color = blendColor( getPixel( x,y), color );
   }

   if ( m_BytesPerPixel >= 4 )
   {
      uint32_t* p = reinterpret_cast< uint32_t* >( m_Data.data() + byteOffset );
      *p = color;
   }
   else if ( m_BytesPerPixel == 3 )
   {
      *(m_Data.data() + byteOffset+0) = RGBA_R( color );
      *(m_Data.data() + byteOffset+1) = RGBA_G( color );
      *(m_Data.data() + byteOffset+2) = RGBA_B( color );
   }
   else if ( m_BytesPerPixel == 2 )
   {
      *(m_Data.data() + byteOffset+0) = RGBA_R( color );
      *(m_Data.data() + byteOffset+1) = RGBA_G( color );
   }
   else if ( m_BytesPerPixel == 1 )
   {
      *(m_Data.data() + byteOffset+0) = RGBA_R( color );
   }
   else // if ( m_BytesPerPixel == 1 )
   {
      DE_ERROR("No bps")
   }
}

uint8_t*
Image::getPixels( int32_t x, int32_t y )
{
   if ( m_Width < 1 || m_Height < 1 ) return nullptr;
   uint32_t ux = uint32_t( x );
   uint32_t uy = uint32_t( y );
   uint32_t uw = uint32_t( m_Width );
   uint32_t uh = uint32_t( m_Height );
   if ( ux >= uw || uy >= uh ) return nullptr;
   uint64_t byteOffset = uint64_t( m_Width ) * uy + ux;
   byteOffset *= getBytesPerPixel();
   if ( byteOffset + m_BytesPerPixel > m_Data.size() ) return nullptr;
   return m_Data.data() + byteOffset;
}

uint32_t
Image::getPixel( int32_t x, int32_t y, uint32_t colorKey ) const
{
   if ( m_Width < 1 || m_Height < 1 )
   {
      DE_ERROR("Empty image (",x,",",y,")")
      return colorKey;
   }

   uint32_t ux = uint32_t( x );
   uint32_t uy = uint32_t( y );
   uint32_t uw = uint32_t( m_Width );
   uint32_t uh = uint32_t( m_Height );
   if ( ux >= uw || uy >= uh )
   {
      DE_ERROR("Coords outside range (",x,",",y,")")
      return colorKey;
   }

   uint64_t byteOffset = uint64_t( m_Width ) * uy + ux;
   byteOffset *= getBytesPerPixel();
   if ( byteOffset + m_BytesPerPixel > m_Data.size() ) return colorKey;

   if ( m_BytesPerPixel >= 4 )
   {
      return *reinterpret_cast< uint32_t const * const >( m_Data.data() + byteOffset );
   }
   else if ( m_BytesPerPixel == 3 )
   {
      uint8_t r = *(m_Data.data() + byteOffset);
      uint8_t g = *(m_Data.data() + byteOffset + 1);
      uint8_t b = *(m_Data.data() + byteOffset + 2);
      return RGBA( r,g,b );
   }
   else if ( m_BytesPerPixel == 2 )
   {
      uint8_t r = *(m_Data.data() + byteOffset);
      uint8_t g = *(m_Data.data() + byteOffset + 1);
      return RGBA( r,g,0 );
   }
   else if ( m_BytesPerPixel == 1 )
   {
      uint8_t r = *(m_Data.data() + byteOffset);
      return RGBA( r,r,r );
   }
   else // if ( m_BytesPerPixel == 1 )
   {
      DE_ERROR("No bps")
      return 0;
   }
}

void
Image::resize( int32_t w, int32_t h )
{
//   if ( w == m_Width && h == m_Height )
//   {
//      return; // Nothing todo
//   }

   if ( w < 1 || h < 1 )
   {
      clear();
      return; // cleared image
   }

   uint64_t byteCount = uint64_t( w ) * uint64_t( h ) * getBytesPerPixel();
   m_Data.resize( byteCount, 0x00 );
   m_Width = w;
   m_Height = h;
}

void
Image::fill( uint32_t color )
{
   uint32_t * pixels = writePtr< uint32_t >();
   for ( size_t i = 0; i < uint64_t( m_Width ) * size_t( m_Height ); ++i )
   {
      *pixels = color;
      pixels++;
   }
}

void
Image::fillZero()
{
   ::memset( m_Data.data(), 0x00, m_Data.size() );

//   for ( size_t i = 0; i < m_Data.size(); ++i )
//   {
//      m_Data[ i ] = 0;
//   }
}

void
Image::floodFill(  int32_t x,
            int32_t y,
            uint32_t newColor,
            uint32_t oldColor )
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
Image::flipVertical()
{
   std::vector< uint8_t > row_buf1;
   std::vector< uint8_t > row_buf2;
   for ( int32_t y = 0; y < m_Height/2; ++y )
   {
      copyRowFrom( row_buf1, y );
      copyRowFrom( row_buf2, m_Height - 1 - y );
      copyRowTo( row_buf1, m_Height - 1 - y );
      copyRowTo( row_buf2, y );
   }
}

void
Image::flipHorizontal()
{
   std::vector< uint8_t > col1;
   std::vector< uint8_t > col2;
   for ( int32_t x = 0; x < m_Width/2; ++x )
   {
      copyColFrom( col1, x );
      copyColFrom( col2, m_Width - 1 - x );
      copyColTo( col1, m_Width - 1 - x );
      copyColTo( col2, x );
   }
}

Image
Image::copy( Recti const & pos ) const
{
   int w = pos.w();
   int h = pos.h();
   Image dst( w,h, getFormat() );
   for ( int j = 0; j < h; ++j )
   {
      for ( int i = 0; i < w; ++i )
      {
         auto color = getPixel( pos.x() + i, pos.y() + j );
         dst.setPixel( i, j, color );
      }
   }
   return dst;
}

void
Image::copyRowFrom( std::vector< uint8_t > & out, int32_t y )
{
   if ( uint32_t( y ) >= uint32_t( m_Height ) )
   {
      DE_DEBUG("Invalid row index(",y,")")
      return;
   }

   uint64_t bpr = uint64_t( getWidth() ) * getBytesPerPixel();
   if ( bpr < 1 )
   {
      DE_DEBUG("Nothing todo")
      return;
   }

   out.resize( bpr, 0x00 );
   ::memcpy( out.data(), data() + (bpr * y), bpr );
}

void
Image::copyRowTo( std::vector< uint8_t > const & row, int32_t y )
{
   if ( uint32_t( y ) >= uint32_t( m_Height ) )
   {
      DE_DEBUG("Invalid row index(",y,")")
      return;
   }

   uint64_t bpr = uint64_t( getWidth() ) * getBytesPerPixel();
   if ( bpr < 1 )
   {
      DE_DEBUG("Nothing todo")
      return;
   }

   if ( bpr != row.size() )
   {
      DE_ERROR("Cant copy incomplete row ", y )
      return;
   }

   ::memcpy( data() + (bpr * y), row.data(), bpr );
}

void
Image::copyColFrom( std::vector< uint8_t > & col, int32_t x )
{
   if ( uint32_t( x ) >= uint32_t( m_Width ) )
   {
      DE_DEBUG("Invalid column x(",x,")")
      return;
   }

   uint32_t bpp = getBytesPerPixel();

   uint32_t bpc = uint32_t(m_Height) * bpp;
   col.resize( bpc );

   uint8_t* dst = col.data();
   for ( int32_t y = 0; y < m_Height; ++y )
   {
      uint8_t* src = getPixels(x,y);
      ::memcpy( dst, src, bpp );
      dst += bpp;
   }
}

void
Image::copyColTo( std::vector< uint8_t > const & col, int32_t x )
{
//   if ( uint32_t( x ) >= uint32_t( m_Width ) )
//   {
//      DE_DEBUG("Invalid col index(",y,")")
//      return;
//   }

//   uint64_t bpr = uint64_t( getWidth() ) * getBytesPerPixel();
//   if ( bpr < 1 )
//   {
//      DE_DEBUG("Nothing todo")
//      return;
//   }

//   if ( bpr != row.size() )
//   {
//      DE_ERROR("Cant copy incomplete row ", y )
//      return;
//   }

//   ::memcpy( data() + (bpr * y), row.data(), bpr );
   if ( uint32_t( x ) >= uint32_t( m_Width ) )
   {
      DE_DEBUG("Invalid column x(",x,")")
      return;
   }

//   uint32_t bpp = getBytesPerPixel();

//   uint32_t bpc = uint32_t(m_Height) * bpp;
//   col.resize( bpc );

//   uint8_t* dst = col.data();
//   for ( int32_t y = 0; y < m_Height; ++y )
//   {
//      uint8_t* src = getPixels(x,y);
//      ::memcpy( dst, src, bpp );
//      dst += bpp;
//   }
}

void
Image::switchRB()
{
   for ( int32_t y = 0; y < getHeight(); ++y )
   {
      for ( int32_t x = 0; x < getWidth(); ++x )
      {
         uint32_t c = getPixel( x, y );
         setPixel( x, y, RGBA( RGBA_B( c ), RGBA_G( c ), RGBA_R( c ), RGBA_A( c ) ) );
      }
   }
}

uint8_t*
Image::getRow( int32_t y )
{
   // TODO: Necessary?
   if ( empty() ) { DE_ERROR("Empty") return nullptr; }

   // CheckPoint 1: Test user param row-y if valid
   if ( y < 0 || y >= m_Height )
   {
      DE_DEBUG("Invalid y(",y,") of h(",m_Height,")")
      return nullptr;
   }

   // Compute linear byte index...
   uint64_t const byteOffset = uint64_t( y ) * getStride();

   // CheckPoint 2: Test linear byte index if valid
   if ( byteOffset >= m_Data.size() )
   {
      DE_DEBUG("Invalid byteOffset(",byteOffset,"), y(",y,") of h(",m_Height,")")
      return nullptr;
   }

   // Output address (base+offset) of first pixel of row y...
   return m_Data.data() + byteOffset;
}

uint8_t*
Image::getColumn( int32_t x )
{
   if ( empty() ) { DE_ERROR("Empty") return nullptr; }

   if ( x < 0 || x >= m_Width )
   {
      DE_DEBUG("Invalid x(",x,") of w(",m_Width,")")
      return nullptr;
   }

   if ( x >= int32_t(m_Data.size()) )
   {
      DE_DEBUG("Invalid x(",x,") > m_Data.size(",m_Data.size(),")")
      return nullptr;
   }
   return m_Data.data() + x;
}


std::string Image::toString( bool withUri, bool withBytes ) const
{
   std::ostringstream s;
   if ( empty() )
   {
      s << "empty";
   }
   else
   {
      s << m_Width << "," << m_Height << "," << getFormatStr();
      if ( withBytes ) s << "," << m_Data.size();
      if ( withUri ) s << "," << m_FileName;
   }

   return s.str();
}


} // end namespace de.
