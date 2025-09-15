#pragma once
#include <cstdint>
#include <sstream>
#include <vector>
#include <array>
#include <memory> // SharedPtr
#include <map>    // GlyphCache

#include <de/image/Image.h>

namespace de {

// Can describe font

// ===========================================================================
struct Font5x8 // Save/Load, cheap to copy. What settings the user can control.
// ===========================================================================
{
    Font5x8();
    Font5x8(int quadWidth_, int quadHeight_,
            int quadSpacingX_ = 0, int quadSpacingY_ = 0,
            int glyphSpacingX_ = 0, int glyphSpacingY_ = 0);

    uint64_t operator< ( Font5x8 const & o ) const;
    uint64_t hash() const;
    std::string toString() const;

    struct TextSize
    {
        int width;
        int height;
        TextSize() : width(0), height(0) {}
        TextSize(int w, int h) : width(w), height(h) {}

        std::string
        toString() const
        {
            std::ostringstream o;
            o << width << "," << height;
            return o.str();
        }
    };

    TextSize
    getTextSize( std::string const & msg ) const;

    //static TextSize
    //getTextSize( std::string const & msg, Font5x8 const & font );

    int quadWidth;
    int quadHeight;
    int quadSpacingX;
    int quadSpacingY;
    int glyphSpacingX;
    int glyphSpacingY;

    typedef std::function<void(int, int, uint32_t)> FN_SET_PIXEL;

    void renderText(int x, int y,
                  const std::string & msg, uint32_t color,
                  Align align,
                  const FN_SET_PIXEL & setPixel ) const;

    void drawText(Image & o, int x, int y,
                  std::string const & msg, uint32_t color,
                  Align align = Align::Default ) const;
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
   //Font5x8::TextSize getTextSize( std::string const & msg ) const;
   bool hasGlyph( uint32_t unicode ) const;
   Font5x8Glyph & getGlyph( uint32_t unicode );
   void cacheString( std::string const & msg );
      
   static void createGlyphCache( GlyphCache & cache );
   // void add2DText( SMeshBuffer & o, int x, int y, std::string const & msg, uint32_t color, Align align );
   //Font5x8 font;
   GlyphCache m_glyphCache;
};

// User API to access a Font5x8 implementation

// New: There is only one GlyphCache for all Font5x8's!
Font5x8Face::SharedPtr
getFontFace5x8(); //  Font5x8 const & font = Font5x8()

} // end namespace de.
