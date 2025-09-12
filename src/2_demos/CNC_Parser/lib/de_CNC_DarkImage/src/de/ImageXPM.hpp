// Copyright (C) 2002-2012, Benjamin Hampe
#pragma once
#include <de/ImageReader.hpp>
#include <de/ImageWriter.hpp>
#include <unordered_map>

#if defined(DE_IMAGE_READER_XPM_ENABLED) || defined(DE_IMAGE_WRITER_XPM_ENABLED)

namespace de {
namespace image {
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
         uint8_t r = de::RGBA_R(color);
         uint8_t g = de::RGBA_G(color);
         uint8_t b = de::RGBA_B(color);
         uint8_t a = de::RGBA_A(color);
         s += '#';
         s += dbStrByte( r );
         s += dbStrByte( g );
         s += dbStrByte( b );
         s += dbStrByte( a );
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
         s += dbStrHighNibble( r );
         s += dbStrLowNibble( r );
         s += dbStrHighNibble( g );
         s += dbStrLowNibble( g );
         s += dbStrHighNibble( b );
         s += dbStrLowNibble( b );
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
} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_CODEC_XPM_ENABLED




/*
   static std::unordered_map< uint32_t, std::string >
   computeColorTable( Image const & src, std::string const & valid_chars )
   {
      std::vector< uint32_t > colors = computeUniqueColors( src );

      std::unordered_map< uint32_t, std::string > lut;
      for ( size_t i = 0; i < colors.size(); ++i )
      {
         lut[ colors[ i ] ] = computeColorName( i, valid_chars );
      }

      DE_DEBUG("lut.size() = ", lut.size())
      size_t i = 0;
      const_iterator it = colors.begin();
      while ( it != colors.end() )
      {
         DE_DEBUG("lut[",i,"/",lut.size(),"] color:", it->first,", name:",it->second)
         it++;
         i++;
      }

      return lut;
   }

   static std::vector< uint32_t >
   computeUniqueColors( Image const & src )
   {
      std::vector< uint32_t > unique_colors;

      for ( size_t y = 0; y < size_t( src.getHeight() ); ++y )
      {
         for ( size_t x = 0; x < size_t( src.getWidth() ); ++x )
         {
            uint32_t color = src.getPixel( x, y );
            auto it = std::find_if( unique_colors.begin(), unique_colors.end(),
                        [&] ( uint32_t cached ){ return cached == color; } );
            if ( it == unique_colors.end() )
            {
               unique_colors.emplace_back( color );
            }
         }
      }

      std::cout << "unique_colors = " << unique_colors.size() << std::endl;
      return unique_colors;
   }

*/

//   static
//   getColorMap( Image const & src )
//   {
//      std::vector< std::string > keys;

//   getUniqueColors( Image const & src )
//   {
//      std::vector< uint32_t > unique_colors;
//   }
