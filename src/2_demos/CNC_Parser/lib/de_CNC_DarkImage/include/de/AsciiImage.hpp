#pragma once

#if 0
#include <de/FileSystem.hpp>
#include <de/Color.hpp>

namespace de {

// ===========================================================================
/// @brief AsciiImage - 8bit Image (char) class for Consoles and Terminals.
// ===========================================================================
struct AsciiImage
{
   DE_CREATE_LOGGER("de.AsciiImage")
   int32_t m_Width;
   int32_t m_Height;
   std::vector< char > m_Data;
   std::string m_FileName;

public:
   static AsciiImage*
   create( int32_t w, int32_t h, char fillChar = ' ' );

   AsciiImage();
   AsciiImage( int32_t w, int32_t h, char fillChar = ' ' );
   ~AsciiImage();

   void
   floodFill( int32_t x, int32_t y, char newColor, char oldColor );

   std::string
   toString() const;

   void 
   resize( int32_t w, int32_t h, char fillChar = ' ' );

   char
   getPixel( int32_t x, int32_t y, char colorKey = ' ' ) const;
   void
   setPixel( int32_t x, int32_t y, char color );

   inline void
   setPixel( glm::ivec2 const & p, char color ) { setPixel( p.x, p.y, color ); }

   void
   fill( char fillChar );

   char*
   getRow( int32_t y );

   char const*
   getRow( int32_t y ) const;

   void
   drawLineH( int32_t x1, int32_t x2, int32_t y, char color );

   void
   drawLine( int32_t x1, int32_t y1, int32_t x2, int32_t y2, char color );

   void
   drawOutlineRect( int32_t x1, int32_t y1,
                 int32_t x2, int32_t y2, char color );

   void
   drawOutlineCircle( int32_t x, int32_t y, int32_t radius, char color );

   void
   drawFilledCircle( int32_t x, int32_t y, int32_t radius, char color );

   void
   drawOutlineTriangle( int32_t x1, int32_t y1,
                     int32_t x2, int32_t y2,
                     int32_t x3, int32_t y3, char color );

   void
   drawFilledTriangle( glm::ivec2 const & A, glm::ivec2 const & B, glm::ivec2 const & C, char color );

   inline void
   drawFilledTriangle( int32_t x1, int32_t y1,
                 int32_t x2, int32_t y2,
                 int32_t x3, int32_t y3, char color )
   {
      drawFilledTriangle( glm::ivec2( x1, y1 ), glm::ivec2( x2, y2 ), glm::ivec2( x3, y3 ), color );
   }


   // void
   // drawImage( Image const & src, int32_t x, int32_t y, bool blend = false );

   // bool
   // load( std::string const & fileName );

   // bool
   // save( std::string const & fileName );

   // virtual std::vector< std::string >
   // getSupportedReadExtensions() const = 0;

   // virtual bool
   // isSupportedReadExtension( std::string const & suffix ) const = 0;

   // virtual bool
   // isSupportedWriteExtension( std::string const & suffix ) const = 0;

   // Convenience section: Make class behave/feel like std::vector< char >
   size_t
   size() const { return m_Data.size(); }
   size_t
   capacity() const { return m_Data.capacity(); }

   typedef std::vector< char >::const_iterator const_iterator;
   const_iterator
   begin() const { return m_Data.begin(); }
   const_iterator
   end() const { return m_Data.end(); }

   typedef std::vector< char >::iterator iterator;
   iterator
   begin() { return m_Data.begin(); }
   iterator
   end() { return m_Data.end(); }


};

// ===========================================================================
// AsciiImageTest
// ===========================================================================
struct AsciiArt
{
   DE_CREATE_LOGGER("de.AsciiArt")

   static void
   test();
};

} // end namespace de.

#endif