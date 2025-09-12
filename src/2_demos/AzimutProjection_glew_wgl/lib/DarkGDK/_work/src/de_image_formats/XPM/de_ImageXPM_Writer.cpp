// Copyright (C) 2002-2012, Benjamin Hampe

#include "de_ImageXPM_Writer.h"

#ifdef DE_IMAGE_WRITER_XPM_ENABLED

#include "de_ImageXPM.h"
#include <fstream>
#include <iomanip>

namespace de {
namespace image {

bool
ImageWriterXPM::save( Image const & img, std::string const & uri, uint32_t param )
{
   if ( uri.empty() ) { DE_ERROR("Empty uri(", uri, ")") return false; }
   if ( img.empty() ) { DE_ERROR("Cant save empty image uri(", uri, ")") return false; }
   int32_t w = img.getWidth();
   int32_t h = img.getHeight();
   if ( w < 1 || h < 1 ) { DE_ERROR("Got empty image(", uri, ")") return false; }

   // calculate needed bytes to store one color
   std::string const valid_chars = "1234567890"
                                   "abcdefghijklmnopqrstuvwxyz"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "{}[|]$<^>:~(;%-)=@_!.+`#'&*,/";

   // Allocate worst case ( each pixel has a different color )
   std::vector< uint32_t > colors;
   colors.reserve( img.getPixelCount() );

   // loop image and find all unique colors

   // std::ostringstream s;
   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t const color = img.getPixel( x, y ); // get color
         //color |= 0xFF000000; // set alpha const to 255

         // add_unique
         auto itCached = std::find_if( colors.begin(), colors.end(),
            [&] ( uint32_t cached )
            { return cached == color; } );
         if ( itCached == colors.end() )
         {
            colors.emplace_back( color );
         }

//         bool found = false;
//         for ( size_t i = 0; i < colors.size(); ++i )
//         {
//            if ( colors[ i ] == color )
//            {
//               found = true;
//               break;
//            }
//         }

//         // add_unique
//         if ( !found )
//         {
//            colors.emplace_back( color );
//         }
      }
   }

   uint32_t digitCount = 1;
   double f = double( colors.size() );
   double e = double( valid_chars.size() );
   while ( f >= e )
   {
      f /= e;
      ++digitCount;
   }

   //DE_DEBUG("digitCount(",digitCount,"), img(", img.toString(),")")

   std::vector< std::string > names;
   names.reserve( colors.size() );

   // Unique color -> Unique name
   for ( size_t i = 0; i < colors.size(); ++i )
   {
      names.push_back( xpm::Utils::computeColorName(i,valid_chars,digitCount) );
   }

   // XPM-Header
   // static char const * const xpm_test1[] = {
   // "/* XPM */",
   // "16 16 6 1",
   // " 	c None",
   // ".	c #FFFFFF",
   // "+	c #000000",
   // "@	c #9A9A9A",
   // "#	c #FEFEFE",
   // "$	c #F7F7F7",
   // "................",

   std::string baseName = FileSystem::fileBase( uri );
   StringUtil::replace( baseName, ".", "_");
   StringUtil::replace( baseName, ",", "_");
   StringUtil::replace( baseName, "-", "_");
   StringUtil::replace( baseName, "+", "_");
   StringUtil::replace( baseName, "*", "_");
   StringUtil::replace( baseName, "/", "_");
   StringUtil::replace( baseName, "\\","_");
   StringUtil::replace( baseName, " ", "_");
   StringUtil::replace( baseName, "#", "_");
   StringUtil::replace( baseName, "<", "_");
   StringUtil::replace( baseName, ">", "_");
   StringUtil::replace( baseName, "$", "_");
   StringUtil::replace( baseName, "!", "_");
   StringUtil::replace( baseName, "?", "_");
   StringUtil::replace( baseName, "(", "_");
   StringUtil::replace( baseName, ")", "_");
   StringUtil::replace( baseName, "{", "_");
   StringUtil::replace( baseName, "}", "_");
   StringUtil::replace( baseName, "[", "_");
   StringUtil::replace( baseName, "]", "_");
   StringUtil::replace( baseName, "%", "_");
   StringUtil::replace( baseName, "'", "_");
   StringUtil::replace( baseName, "~", "_");
   StringUtil::replace( baseName, "|", "_");
   StringUtil::replace( baseName, "'", "_");
   StringUtil::replace( baseName, "^", "_");
   StringUtil::replace( baseName, "°", "_");
   StringUtil::replace( baseName, "&", "_");
   StringUtil::replace( baseName, "€", "_");

   std::ostringstream o;
   o <<
   "/* XPM-Header */\n"
   "/* URI = " << uri << " */\n"
   "/* ValidChars = " << valid_chars.size() << "*/\n"
   "/* Width in [px] = " << w << "*/\n"
   "/* Height in [px] = " << h << "*/\n"
   "/* NumColors = " << colors.size() << "*/\n"
   "/* CharsPerColor = " << digitCount << "*/\n"
   "/* Encoded by ImageWriterXPM.cpp from libDarkImage */\n"
   "/* Author: Benjamin Hampe <benjaminhampe@gmx.de> */\n"
   "/* Whats special about these: Decoding and Encoding are much faster than IrfanView! */\n"
   //"static std::vector< std::string > const " << baseName << "_xpm {\n"
   "static const char * const " << baseName << "_xpm[] = {\n"

   "\""<<w<<" "<<h<<" "<<
#if 0
   colorCount
#else
   colors.size()
#endif
   <<" "<<digitCount<<"\",\n";

   // ====================================
   // Write color-table to xpm
   // ====================================
   for ( size_t i = 0; i < colors.size(); ++i )
   {
      // format color #RRGGBB
      uint32_t color = colors[ i ];
//      std::string name = ;
//      char buf[ 8 ];
//      sprintf( buf, "#%02x%02x%02x",
//                     RGBA_R( color ),
//                     RGBA_G( color ),
//                     RGBA_B( color ) );
//      std::string tmp = buf;
//      dbStrUpperCase( tmp );
      o << "\"" << names[ i ] << " c " << xpm::Utils::toRGBA( color ) << "\",\n";
   }

   // ====================================
   // Write pixels to xpm
   // ====================================
   std::string line;
   for ( int32_t y = 0; y < h; ++y )
   {
      line = "\"";
      for ( int32_t x = 0; x < w; ++x )
      {
         // current pixel
         uint32_t color = img.getPixel( x, y );
         //color.setAlpha(255);

         // search color in color-table
//         if ( color != 0 )
//         {
//            color |= 0xFF000000; // Set alpha to opaque ( speed up compare )
//         }

         auto cit = std::find_if( colors.begin(), colors.end(),
                        [&] ( uint32_t cached ) { return color == cached; }
         );
         if ( cit != colors.end() )
         {
            // write found color-table color
            line += names[ std::distance( colors.begin(), cit ) ];
         }

/*
         size_t found = 0;
         for ( size_t i = 0; i < colors.size(); ++i )
         {
            if ( colors[ i ] == color )
            {
               found = i;
               break;
            }
         }

         // write found color-table color
         line += names[ found ];
*/
      }

      line += "\"";
      if ( y < h-1 )
         line+=",";
      else
         line+="};";

      o << line << "\n";
   }

   // std::cout << s.str() << std::endl;


   std::ofstream fout( uri.c_str() );
   if ( fout.is_open() )
   {
      fout << o.str();
      fout.close();
   }

   return true;
}




} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_XPM_ENABLED

