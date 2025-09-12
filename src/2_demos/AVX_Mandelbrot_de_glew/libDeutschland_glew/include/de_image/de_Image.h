#pragma once
#include <de_image/de_Recti.h>
#include <de_image/de_Rectf.h>
#include <de_image/de_PixelFormatUtil.h>
#include <de_image/de_ColorRGBA.h>
#include <vector>
#include <de_core/de_Logger.h>
#include <memory>

namespace de {

// ===========================================================================
struct Image
// ===========================================================================
{
   typedef std::unique_ptr< Image > Ptr;
   typedef std::shared_ptr< Image > SharedPtr;
   static Image* create( int32_t w, int32_t h, PixelFormat format = PixelFormat::R8G8B8A8 );
   static Ptr createPtr( int32_t w, int32_t h, PixelFormat format = PixelFormat::R8G8B8A8 );
   static SharedPtr createSharedPtr( int32_t w, int32_t h, PixelFormat format = PixelFormat::R8G8B8A8 );

   Image();
   Image( int32_t w, int32_t h, PixelFormat format = PixelFormat::R8G8B8A8 );
   Image( int32_t w, int32_t h, uint32_t fillColor, PixelFormat format );
   // ~Image() = default;
   // Image& operator= ( Image const & other );

   void clear( bool forceShrink = false );
   void resize( int32_t w, int32_t h );
   void shrink_to_fit() { return m_Data.shrink_to_fit(); }
   void fill( uint32_t color );
   void fillZero();
   void flipVertical();
   void flipHorizontal();
   Image copy( Recti const & pos ) const;
   void floodFill( int32_t x, int32_t y, uint32_t newColor, uint32_t oldColor );
   void switchRB();

   uint8_t* getPixels( int32_t x, int32_t y );
   uint8_t const* getPixels( int32_t x, int32_t y ) const { return getPixels(x,y); }

   void setPixel( int32_t x, int32_t y, uint32_t color, bool blend = false );
   uint32_t getPixel( int32_t x, int32_t y, uint32_t colorKey = 0x00000000 ) const;

   std::string toString( bool withUri = true, bool withBytes = false ) const;
   bool     empty() const;
   bool     equals( Image const & other ) const;

   void copyColFrom( std::vector< uint8_t > & out, int32_t x );
   void copyColTo( std::vector< uint8_t > const & col, int32_t x );
   void copyRowFrom( std::vector< uint8_t > & out, int32_t y );
   void copyRowTo( std::vector< uint8_t > const & row, int32_t y );

   uint8_t* getColumn( int32_t x );
   uint8_t* getRow( int32_t y );

   uint8_t const* getColumn( int32_t x ) const { return getColumn( x ); }
   uint8_t const* getRow( int32_t y ) const { return getRow( y ); }

   //   float getPixelR32F( int32_t x, int32_t y ) const
   //   {
   //      return float( getPixel(x,y) );
   //      //return *reinterpret_cast< float const * const >( &color );
   //   }

   void setUri( std::string uri ) { m_FileName = uri; }
   std::string const & getUri() const { return m_FileName; }

   void setFormat( PixelFormat colorFormat );
   PixelFormat const & getFormat() const { return m_pixelFormat; }

   void setPTS( double pts ) { m_pts = pts; }
   double getPTS() const { return m_pts; }

   int32_t getWidth() const { return m_Width; }
   void setWidth( int w ) { m_Width = w; }

   int32_t getHeight() const { return m_Height; }
   void setHeight( int h ) { m_Height = h; }

   uint32_t getChannelCount() const { return PixelFormatUtil::getChannelCount( m_pixelFormat ); }
   std::string getFormatStr() const { return PixelFormatUtil::getString( m_pixelFormat ); }

   uint32_t getBitsPerPixel() const { return m_BytesPerPixel * 8; }
   uint32_t getBytesPerPixel() const { return m_BytesPerPixel; }
   uint64_t getByteCount() const { return getPixelCount() * getBytesPerPixel(); }
   uint64_t getPixelCount() const { return uint64_t( m_Width ) * uint64_t( m_Height ); }
   uint32_t getStride() const { return uint32_t( getWidth() ) * getBytesPerPixel(); }

   uint32_t getRedBits() const { return PixelFormatUtil::getRedBits( m_pixelFormat ); }
   uint32_t getGreenBits() const { return PixelFormatUtil::getGreenBits( m_pixelFormat ); }
   uint32_t getBlueBits() const { return PixelFormatUtil::getBlueBits( m_pixelFormat ); }
   uint32_t getAlphaBits() const { return PixelFormatUtil::getAlphaBits( m_pixelFormat ); }
   uint32_t getDepthBits() const { return PixelFormatUtil::getDepthBits( m_pixelFormat ); }
   uint32_t getStencilBits() const { return PixelFormatUtil::getStencilBits( m_pixelFormat ); }
   
   inline  int32_t w() const { return getWidth(); }
   inline  int32_t h() const { return getHeight(); }
   inline uint32_t r() const { return getRedBits(); }
   inline uint32_t g() const { return getGreenBits(); }
   inline uint32_t b() const { return getBlueBits(); }
   inline uint32_t a() const { return getAlphaBits(); }
   inline uint32_t d() const { return getDepthBits(); }
   inline uint32_t s() const { return getStencilBits(); }   

   inline Recti getRect() const { return Recti(0,0,getWidth(),getHeight()); }
   inline glm::ivec2 getSize() const { return { getWidth(), getHeight() }; }

   template < typename T > T*       writePtr() { return reinterpret_cast< T* >( m_Data.data() ); }
   template < typename T > T const* readPtr() const { return reinterpret_cast< T const * >( m_Data.data() ); }
   uint8_t*                         data() { return m_Data.data(); }
   uint8_t const*                   data() const { return m_Data.data(); }
   uint64_t                         size() const { return m_Data.size(); }
   uint64_t                         capacity() const { return m_Data.capacity(); }
   std::vector< uint8_t > &         dataVector() { return m_Data; }
   std::vector< uint8_t > const &   dataVector() const { return m_Data; }

private:
   DE_CREATE_LOGGER("de.Image")
   int32_t m_Width;           // same as (min) col count that has meaningful data
   int32_t m_Height;          // same as (min) row count
   PixelFormat m_pixelFormat; // ColorBits A + R + G + B, if any
   uint32_t m_BytesPerPixel;
   double m_pts;
   std::vector< uint8_t > m_Data;
   std::string m_FileName;
};

} // end namespace de.

inline std::ostream&
// ===========================================================================
operator<< ( std::ostream & o, de::Image const & img )
// ===========================================================================
{
   o << img.toString();
   return o;
}

/*
// ===========================================================================
// implementation of the VIP font atlas getter defined in <de/Font.hpp>
// ===========================================================================
inline std::string
ANSIConsole_setColor( int r, int g, int b )
{
   std::ostringstream s;
   s << "\033[48;2;" << r << ";" << g << ";" << b << "m";
   return s.str();
}

inline std::string
ANSIConsole_drawImage( de::Image const & img )
{
   uint32_t w = img.w();
   uint32_t h = img.h();
   uint32_t bpp = img.getBytesPerPixel();
   uint8_t const* src = img.data(); // rgb(a) tupels

   std::ostringstream s;

   if ( bpp == 3 )
   {
      for ( size_t y = 0; y < h; ++y )
      {
         for ( size_t x = 0; x < w; ++x )
         {
            int r = *src++;
            int g = *src++;
            int b = *src++;
            s << "\033[48;2;" << r << ";" << g << ";" << b << "m ";
         }
         s << "\033[0m\n";
      }
   }
   else if ( bpp == 4 )
   {
      for ( size_t y = 0; y < h; ++y )
      {
         for ( size_t x = 0; x < w; ++x )
         {
            int r = *src++;
            int g = *src++;
            int b = *src++;
            int a = *src++;
            s << "\033[48;2;" << r << ";" << g << ";" << b << "m ";
         }
         s << "\033[0m\n";
      }
   }
   else
   {
      s << ANSIConsole_setColor(255,0,0) << " ERROR " << "\033[0m\n";
   }
   return s.str();
}
*/