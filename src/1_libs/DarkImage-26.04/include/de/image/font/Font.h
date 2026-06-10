#pragma once

#ifndef HAVE_FREETYPE2
#define HAVE_FREETYPE2
#endif

#ifdef HAVE_FONTAWESOME
#include <de_fontawesome.h>
#endif

// #ifdef HAVE_FREETYPE2
#include <de/image/font/ImageAtlas.h>
#include <de/image/ImagePainter.h>
#include <memory>

namespace de {

// =======================================================================
struct Font
// =======================================================================
{
   Font();
   Font( std::string const & family, uint32_t pixelSize,
         bool bold = false, bool italic = false,
         bool aa = true, bool transparent = true, bool lcdAdjusted = false );

   std::string
   toString() const;

   std::string
   toFontString() const;

   std::string
   toGlyphString( uint32_t glyph_unicode ) const;

   bool
   operator==( Font const & other ) const;

   bool
   operator!=( Font const & other ) const;

   std::string const &
   family() const;

   void
   setFamily( std::string family );

   uint32_t id;
   uint32_t pixelSize;
   int32_t baseLine;
   bool bold;
   bool italic;
   bool antiAlias;
   bool transparent;
   bool lcdFit;
   void* fontPtr;
protected:
   std::string m_family;

};

// =======================================================================
struct TextSize
// =======================================================================
{
    int32_t m_width; //  x = lineWidth, y = lineHeight, z = baselineOffsetY.
    int32_t m_height;
    int32_t m_baseline;
    int32_t m_lineHeight;
    int32_t m_lineCount;

    TextSize()
        : m_width( 0 )
        , m_height( 0 )
        , m_baseline( 0 )
        , m_lineHeight(0)
        , m_lineCount(0)
    {}

    TextSize( int32_t w, int32_t h, int32_t baseline = 0, int32_t lineHeight = 0, int32_t lineCount = 0 )
        : m_width( w )
        , m_height( h )
        , m_baseline( baseline )
        , m_lineHeight( lineHeight )
        , m_lineCount( lineCount )
    {}

    std::string str() const
    {
        std::ostringstream o;
        o   << m_width << ","
            << m_height << ","
            << m_baseline << ","
            << m_lineHeight << ","
            << m_lineCount;
        return o.str();
    }
};

// =======================================================================
struct Text
// =======================================================================
{
    int x;
    int y;
    Align align;
    std::wstring msg;
    Font font;
    Pen pen;
    Brush brush;

    Text( int x = 0, int y = 0, std::wstring msg = L"", Align align = Align::Default,
         uint32_t fillColor = 0xFFFFFFFF, uint32_t penColor = 0xFF000000 )
        : x( x )
        , y( y )
        , align( align )
        , msg( msg )
        , pen( penColor)
        , brush( fillColor )
    {}
};

// =======================================================================
struct Glyph
// =======================================================================
{
    uint32_t unicode;
    int32_t advance;
    ImageRef ref;
    Recti bmp; // bitmap infos from freetype2/3, so (x,y) are for baseline compute.
    uint32_t glyph_index;
    Image debugImg;

    Glyph();
    ~Glyph();
    Image copyImage() const;

    std::string getGlyphString() const;

    Image const * getAtlasImage() const;
    Image * getAtlasImage();
    Recti const & getAtlasRect() const;

    int32_t x() const;
    int32_t y() const;
    int32_t w() const;
    int32_t h() const;
    int32_t x1() const;
    int32_t y1() const;
    int32_t x2() const;
    int32_t y2() const;
    glm::ivec2 size() const;
    Rectf texCoords() const;

    //   float getU1() const;
    //   float getV1() const;
    //   float getU2() const;
    //   float getV2() const;

    // offset < 0: above baseline
    // offset = 0: on baseline ( standing from above baseline, touching it )
    // offset > 0: below baseline ( a 'g' has positive baseline offset )
    int32_t getBaselineOffset() const;
    uint32_t getPixel( int32_t i, int32_t j ) const;
    std::string str() const;
};


// ===========================================================================
// FontAtlas
// ===========================================================================
struct IFontAtlas
{
   virtual ~IFontAtlas() = default;

   virtual Font& getFont() = 0;

   virtual const Font& getFont() const = 0;

   // =======================
   // Text interface
   // =======================
   // x = lineWidth
   // y = lineHeight
   // z = baselineOffsetY
   virtual TextSize getTextSize( const std::wstring & txt ) = 0;

   virtual Glyph createGlyph( const uint32_t unicode ) = 0;

   // [Cache] multiple glyphs at once.
   virtual bool cacheString( const std::wstring& msg ) = 0;

   // [Kerning] between 2 unicode characters.
   virtual glm::ivec2 getKerning( const uint32_t prev, const uint32_t curr ) const = 0;

   // =======================
   // Glyph interface
   // =======================
   virtual void drawText( Image & img, int x, int y, const std::wstring& txt,
    uint32_t color = 0xFF000000, Align align = Align::Default ) = 0;

   virtual Image createTextImage( const std::wstring& txt,
      uint32_t txtColor = 0xFF000000, uint32_t fillColor = 0xFFFFFFFF ) = 0;

   // =======================
   // Glyph interface
   // =======================
   virtual bool hasGlyph( const uint32_t unicode ) const = 0;

   virtual Glyph & getGlyph( const uint32_t unicode ) = 0; // , bool* cachedHit = nullptr

   virtual TextSize getGlyphSize( const uint32_t unicode ) = 0;

   // =======================
   // Atlas interface
   // =======================

   virtual bool isDirty() const = 0;
   virtual void setDirty( bool dirty ) = 0;

   virtual Image const * getAtlasImage() const = 0;
   virtual Image * getAtlasImage() = 0;

   virtual void saveAtlas() = 0;
};


// =======================
// FontAtlasManager interface
// =======================
std::shared_ptr< IFontAtlas >
getFontFace( const Font& font );

//de::addFontFamily( mediaDir + "fonts/garton.ttf", de::Font("garton", 32 ) );
//de::addFontFamily( mediaDir + "fonts/carib.otf", de::Font("carib", 32 ) );
//de::addFontFamily( mediaDir + "fonts/arial.ttf", de::Font("Arial", 32 ) );
//de::addFontFamily( mediaDir + "fonts/awesome.ttf", de::Font("awesome", 32 ) );

// ===========================================================================
struct FontFamily
// ===========================================================================
{
    std::string family;

    std::string uri;

    const uint8_t* dataPtr = nullptr;

    uint64_t dataSize = 0;

    std::string
    toString() const
    {
        std::ostringstream s;
        s << family << "|" << uri;
        return s.str();
    }

};

void
saveFonts();

/*
// ===========================================================================
struct IFontManager
// ===========================================================================
{
   virtual ~IFontManager() = default;

   virtual bool
   addFont( const std::string& uri, const Font& font ) = 0;

   virtual std::shared_ptr< IFontAtlas >
   getFont( const Font& font ) = 0;

   virtual void
   saveFonts() = 0;
};
*/

// =======================
// PreparedGlyphText
// =======================
struct PreparedGlyphText
{
   bool isValid;
   bool needUploadAtlas;
   TextSize textSize;
   std::vector< Glyph > glyphs;

   PreparedGlyphText();
};

} // end namespace de.

// #endif



/*
// ===========================================================================
struct FontDatabase
// ===========================================================================
{
    struct Family
    {
        std::string family;
        std::string uri;
        std::vector<uint8_t> bytes;
        std::vector<std::shared_ptr<IFontAtlas>> faces;
        std::string toStr() const
        {
            std::ostringstream o;
            o << family << "|" << faces.size() << "|" << uri;
            return o.str();
        }
    };

    std::vector<std::shared_ptr<Family>> m_families;

    FontDatabase() {}
    ~FontDatabase() {}

    bool
    addFamily( std::string uri )
    {

    }

    uint32_t
    getFamilyCount() const { return m_families.size(); }

    const std::string &
    getFamilyName( uint32_t i ) const { return m_families.at(i)->family; }

    std::shared_ptr< Family >
    getFont( Font font )
    {
        std::string desiredFamily = font.family();
        if (desiredFamily.empty())
        {
            DE_ERROR("Got empty font family")
            desiredFamily = "Arial";
        }
        auto it_family = std::find_if(m_families.begin(),m_families.end(),
                                      [=](const Family& cached)
                                      {
                                          return StringUtil::makeLower(cached.family)
                                          == StringUtil::makeLower(desiredFamily);
                                      }
                                      );

        if (it_family == m_families.end())
        {

        }
    }

    std::shared_ptr< IFontAtlas >
    getFont( Font font )
    {
        std::string desiredFamily = font.family();
        if (desiredFamily.empty())
        {
            DE_ERROR("Got empty font family")
            desiredFamily = "Arial";
        }
        auto it_family = std::find_if(m_families.begin(),m_families.end(),
            [=](const Family& cached)
            {
                return StringUtil::makeLower(cached.family)
                   == StringUtil::makeLower(desiredFamily);
            }
        );

        if (it_family == m_families.end())
        {

        }
    }

    bool
    load( std::string uri ) { return true; }

    bool
    save( std::string uri ) { return true; }

    std::string toStr() const
    {
        std::ostringstream o;
        o << "FontDatabase.Family.Count = " <<getFamilyCount()<< "\n";
        for (size_t i = 0; i < getFamilyCount(); ++i)
        {
            o << "FontDatabase.Family[" <<i<< "] = " <<getFamilyName(i)<< "\n";
        }

        return o.str();
    }

};
*/
