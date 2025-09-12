#pragma once
#include "DarkImage_details.h"
// #include <de_image_formats/de_ImageReader.h>
// #include <de_image_formats/de_ImageWriter.h>
// #include <de_core/de_StringUtil.h>
#include <unordered_map>


namespace DarkGDK {
namespace image {

#if defined(DE_IMAGE_READER_XPM_ENABLED) || defined(DE_IMAGE_WRITER_XPM_ENABLED)

namespace xpm {

struct Color
{
   std::string m_Key;
   uint32_t m_Color;
   Color( std::string key, uint32_t color )
      : m_Key( key ), m_Color( color )
   {}
};

struct Utils
{
   DE_CREATE_LOGGER("de.image.xpm.Utils")

   static std::string
   toRGBA( uint32_t color )
   {
      std::string s;
      //s.reserve( 7 );

      if ( color == 0 )
      {
         return "none"; // transparent
      }
      else
      {
         uint8_t r = RGBA_R(color);
         uint8_t g = RGBA_G(color);
         uint8_t b = RGBA_B(color);
         uint8_t a = RGBA_A(color);
         s += '#';
         s += StringUtil::hex( r );
         s += StringUtil::hex( g );
         s += StringUtil::hex( b );
         s += StringUtil::hex( a );
      }
      return s;
   }

   static std::string
   toRGB( uint32_t color )
   {
      std::string s;
      //s.reserve( 7 );

      if ( color == 0 )
      {
         return "none"; // transparent
      }
      else
      {
         uint8_t r = color & 0xFF;
         uint8_t g = ( color >> 8 ) & 0xFF;
         uint8_t b = ( color >> 16 ) & 0xFF;
         s += '#';
         s += StringUtil::hexHighNibble( r );
         s += StringUtil::hexLowNibble( r );
         s += StringUtil::hexHighNibble( g );
         s += StringUtil::hexLowNibble( g );
         s += StringUtil::hexHighNibble( b );
         s += StringUtil::hexLowNibble( b );
      }
      return s;
   }

//   static std::string
//   getValidChars()
//   {
//      return " .+@#$%&*=-;>,')!~{]^/(_:<[}|1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ`";
//   }

   static uint32_t
   pow_u( uint32_t val, uint32_t exponent = 0 )
   {
      uint32_t potenz = 1;
      for ( size_t i = 0; i < size_t( exponent ); ++i )
      {
         potenz *= val;
      }
      //DE_DEBUG("pow(", val, ",",exponent,") = ",potenz)
      return potenz;
   }

   static std::string
   computeColorName( uint64_t index, std::string const & valid_chars, int digitCount )
   {
      std::string tmp;
//      int digitCount = 0; // each digit in range [0,chars.size()-1]
//      double f = double( index ); // copy to keep index untouched.
//      while (f >= double( valid_chars.size() ))
//      {
//         f /= double( valid_chars.size() );
//         digitCount++;
//      }

      // std::vector< uint32_t > digits;

      //tmp.resize( digitCount + 1 );

      //std::stringstream s;
      uint32_t v = uint32_t( valid_chars.size() );

      //double f = double( index ); // copy to keep index untouched.
      uint64_t rest = index; // 50000
      for ( int i = digitCount-1; i >= 0; --i ) // n=2 -> [0,1]
      {
         uint32_t n = pow_u( v, uint32_t(i) ); // 39^2
         double f = double( rest ) / double( n ); // 50000/39^2 = 32.8731
         uint32_t digit = uint32_t( f ); // digit = 32;
         //digits.push_back( digit );
         char ch = valid_chars[ digit ];

         //s << ch;
         tmp += ch;

         // 50000 - 32*39^2 = 1328
         //  1328 - 34*39^1 =    2
         //     2 -  2*39^0 =    0
         rest -= digit * n;

         //u -= digitCount /= double( chars.size() );
      }

      //std::string name = s.str();
      //DE_DEBUG("index(", index, "), digitCount(",digitCount,") = ", name)
      //return name;
      //return s.str();
      //tmp[ digitCount ] = '\0';
      return tmp;
   }
};


} // end namespace xpm

#endif // DE_IMAGE_CODEC_XPM_ENABLED




#ifdef DE_IMAGE_READER_XPM_ENABLED

class ImageReaderXPM : public IImageReader
{
   DE_CREATE_LOGGER("de.ImageReaderXPM")
public:
   std::vector< std::string >
   getSupportedReadExtensions() const override
   {
      DE_DEBUG("")
      return std::vector< std::string >{ "xpm" };
   }

   bool
   isSupportedReadExtension( std::string const & ext ) const override
   {
      DE_DEBUG("")
      if ( ext == "xpm" ) return true;
      return false;
   }

   bool
   load( Image & img, std::string const & fileName ) override;

   static bool
   loadImpl( Image & img, std::vector< std::string > const & xpm_data );
};

#endif // DE_IMAGE_READER_XPM_ENABLED






#ifdef DE_IMAGE_WRITER_XPM_ENABLED

class ImageWriterXPM : public IImageWriter
{
   DE_CREATE_LOGGER("de.ImageWriterXPM")
public:
   std::vector< std::string >
   getSupportedWriteExtensions() const override
   {
      return { "xpm", "h", "hpp" };
   }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override
   {
      if ( ext == "xpm" ) return true;
      if ( ext == "h" ) return true;
      if ( ext == "hpp" ) return true;
      //if ( ext == "txt" ) return true;
      return false;
   }

   bool
   save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;

//   struct XPM_Color
//   {
//      uint32_t color;
//      std::string key;

//      XPM_Color( uint32_t color_ = 0, std::string const & charCombi_ = "" )
//         : color( color_ ), key( charCombi_ )
//      {}
//   };

};

#endif // DE_IMAGE_WRITER_XPM_ENABLED


} // end namespace image.
} // end namespace DarkGDK.

