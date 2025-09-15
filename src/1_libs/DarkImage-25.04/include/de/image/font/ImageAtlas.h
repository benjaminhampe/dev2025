#pragma once
#include <de/image/Image.h>

namespace de {

/*
// =====================================================================
struct ImageAtlasRef
// =====================================================================
{
   int32_t id = -1;  // User enum value >= 0, should be faster than a string compare with key.
   int32_t page = -1;  // User enum value >= 0, should be faster than a string compare with key.
   Image* img = nullptr;
   Recti rect;  // We already have a convience struct used for FontAtlas. Reuse.
   int wrapMode = 0; // 0=Repeat, 1=RepeatMirrored, 2=ClampToEdge, 3=ClampToBorder
   bool mipmaps = true;
   std::string name;

   std::string    toString() const;
};
*/

// =======================================================================
// ImageRef = Image + Recti and converter to Rectf tex coords
// ImageRef = SubImage
// ImageRef = ImagePart
// ImageRef = ImageAtlasRef
// =======================================================================
struct ImageRef
// =======================================================================
{
    Image* m_img; // source image atlas page
    Recti m_rect; // subregion in image atlas page the glyph occupies.

    ImageRef();
    ImageRef( Image const * src );
    ImageRef( Image const * src, Recti const & region );
    Image copyImage() const;
    uint32_t pixel( int32_t i, int32_t j ) const;
    Rectf texCoords( bool useOffset = true ) const;
    std::string toString() const;

    Image const * image() const { return m_img; }
    Image * image() { return m_img; }
    Recti const & rect() const { return m_rect; }
    int32_t x() const { return m_rect.x; }
    int32_t y() const { return m_rect.y; }
    int32_t w() const { return m_rect.w; }
    int32_t h() const { return m_rect.h; }
    int32_t x1() const { return m_rect.x1(); }
    int32_t y1() const { return m_rect.y1(); }
    int32_t x2() const { return m_rect.x2(); }
    int32_t y2() const { return m_rect.y2(); }
    Sizei size() const { return m_rect.size(); }

    // Image const * getImage() const { return m_img; }
    // Image * getImage() { return m_img; }
    // Recti const & getRect() const { return m_rect; }
    // int32_t getX() const { return x(); }
    // int32_t getY() const { return y(); }
    // int32_t getWidth() const { return w(); }
    // int32_t getHeight() const { return h(); }
    // int32_t getX1() const { return x1(); }
    // int32_t getY1() const { return y1(); }
    // int32_t getX2() const { return x2(); }
    // int32_t getY2() const { return y2(); }
    // glm::ivec2 getSize() const { return size(); }
    // uint32_t getPixel( int32_t i, int32_t j ) const { return pixel( i, j ); }
    // Rectf getTexCoords( bool useOffset = true ) const { return texCoords( useOffset ); }
};

// =====================================================================
struct ImageAtlasRef
// =====================================================================
{
   int32_t id = -1;  // User enum value >= 0, should be faster than a string compare with key.
   int32_t page = -1;  // User enum value >= 0, should be faster than a string compare with key.
   Image* img = nullptr;
   Recti rect;  // We already have a convience struct used for FontAtlas. Reuse.
   int wrapMode = 0; // 0=Repeat, 1=RepeatMirrored, 2=ClampToEdge, 3=ClampToBorder
   bool mipmaps = true;
   std::string name;

   std::string    toString() const;
};


// =====================================================================
struct ImageAtlasPage
// =====================================================================
{
   ImageAtlasPage();
   ~ImageAtlasPage();
   void clear();
   void setup( std::string pageName, int w = 2048, int h = 2048 );
   int w() const { return img.w(); }
   int h() const { return img.h(); }

   void setMediaDir( std::string dir ) { mediaDir = dir; }

   std::string const & uri() const { return img.uri(); }
   void setUri( std::string const & uri ) { img.setUri( uri ); }


   uint32_t
   getRefCount() const { return refs.size(); }

   ImageAtlasRef const &
   getRef( int id ) const;

   //Image
   //getRefImage( int id ) const;

   int
   findRef( int id ) const;

   bool
   addRef( int id, std::string name, de::Image const & img, int x, int y, bool mipmaps = true );

   bool
   addRef( int id, std::string name, std::string uri, int x, int y, bool mipmaps = true );

   bool
   addFullPageRef( int id, std::string name, bool mipmaps = true, int wrapMode = 0 );
//   Image const &
//   getImage() const { return m_Page0; }

   std::string
   toString() const;

//   std::string const &
//   getXmlUri() const { return m_XmlUri; }

//   void
//   setXmlUri( std::string const & uri ) { m_XmlUri = uri; }

   bool
   loadXml( std::string const & uri );

   bool
   saveXml( std::string const & uri );


   int pageId;
   int wrapMode;
   Image img; // big atlas image
   std::vector< ImageAtlasRef > refs;

   std::string name;
   std::string mediaDir;


};


// =====================================================================
struct ImageAtlas
// =====================================================================
{
   DE_CREATE_LOGGER("de.ImageAtlas")
   std::string name;
   std::vector< ImageAtlasPage > pages;
   //std::vector< ImageAtlasRef > refs;

   // void setMediaDir( std::string dir ) { pages.back().mediaDir = dir; }
   int
   freePageId() const;

   void
   delPage( int pageId );

   int
   findPage( int pageId ) const;

   ImageAtlasPage &
   getPage( int pageId );

   ImageAtlasPage const &
   getPage( int pageId ) const;

   void
   addPageToPage( int srcPageId, int dstPageId, int dstX, int dstY );


   ImageAtlasRef const &
   getRef( int id ) const;

   Image
   copyImage( int id ) const;

   int
   addPage( std::string pageName, int w, int h, std::string mediaDir );

   int
   addPage( std::string pageName, Image const & img );

   void
   addRef( int id, std::string name, de::Image const & img, int x, int y, bool mipmaps = true );

   void
   addRef( int id, std::string name, std::string const & uri, int x, int y, bool mipmaps = true );

   void
   addFullPageRef( int id, std::string name, bool mipmaps = true, int wrapMode = 0 );

   bool
   loadXml( std::string const & uri );

   bool
   saveXml( std::string const & uri ) const;

};


} // end namespace de


inline std::ostream&
operator<< ( std::ostream & o, de::ImageRef const & img )
{
   o << img.toString(); return o;
}

