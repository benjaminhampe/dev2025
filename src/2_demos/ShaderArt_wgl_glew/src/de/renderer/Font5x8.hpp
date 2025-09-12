#pragma once
#include <cstdint>
#include <sstream>
#include <vector>
#include <array>
#include <memory> // SharedPtr
#include <map>    // GlyphCache

/*
#include <unordered_map>
#include <algorithm>
*/

#include "Align.hpp"

namespace de {

// Can describe font

// ===========================================================================
struct Font5x8 // Save/Load, cheap to copy. What settings the user can control.
// ===========================================================================
{
   Font5x8();
   Font5x8( int quadWidth_,
            int quadHeight_,
            int quadSpacingX_,
            int quadSpacingY_,
            int glyphSpacingX_,
            int glyphSpacingY_);

   uint64_t operator< ( Font5x8 const & o ) const;
   uint64_t hash() const;

   //glm::ivec2 getTextSize( std::string const & msg ) const;
   //static glm::ivec2 getTextSize( std::string const & msg, Font5x8 const & font );

   std::string
   toString() const
   {
      std::stringstream s;
      s  << quadWidth << ","
         << quadHeight << ","
         << quadSpacingX << ","
         << quadSpacingY << ","
         << glyphSpacingX << ","
         << glyphSpacingY;
      return s.str();
   }

   struct TextSize
   {
      int width;
      int height;
      TextSize() : width(0), height(0) {}
      TextSize(int w, int h) : width(w), height(h) {}

      std::string
      toString() const
      {
         std::ostringstream s;
         s << width << "," << height;
         return s.str();
      }
   };

   TextSize
   getTextSize( std::string const & msg ) const;

   static TextSize
   getTextSize( std::string const & msg, Font5x8 const & font );


   int quadWidth;
   int quadHeight;
   int quadSpacingX;
   int quadSpacingY;
   int glyphSpacingX;
   int glyphSpacingY;


};

// Also implements font

// ===========================================================================
struct Font5x8Glyph // A font5x8 glyph pixel image (blackORwhite = bool)
// ===========================================================================
{
   Font5x8Glyph();
   int w() const { return 5; }
   int h() const { return 8; }
   bool isPixel( int x, int y ) const;
   void clear();
   void set( int x, int y, bool state );
   void off( int x, int y );
   void on( int x, int y );
   void clearRow( int y );
   void setRow( int y, std::string cc );
   bool get( int x, int y ) const;
   char m_symbol;
   std::array< bool, 40 > dots; // 5x8
};

// ===========================================================================
struct Font5x8Face // A font5x8 glyph atlas for one font5x8 description
// ===========================================================================
{
   typedef std::shared_ptr< Font5x8Face > SharedPtr;
   typedef std::map< uint32_t, Font5x8Glyph > GlyphCache;
   
   Font5x8Face();
   ~Font5x8Face();
   Font5x8::TextSize getTextSize( std::string const & msg ) const;
   bool hasGlyph( uint32_t unicode ) const;
   Font5x8Glyph & getGlyph( uint32_t unicode );
   void cacheString( std::string const & msg );
   
   //void drawText( Image & o, int x, int y, std::string const & msg, uint32_t color, Align align = Align::Default );
   
   static void createGlyphCache( GlyphCache & cache );
   // void add2DText( SMeshBuffer & o, int x, int y, std::string const & msg, uint32_t color, Align align );
   Font5x8 font;
   GlyphCache m_glyphCache;
};

// User API to access a Font5x8 implementation

Font5x8Face::SharedPtr
getFontFace5x8( Font5x8 const & font = de::Font5x8() );

} // end namespace de.
