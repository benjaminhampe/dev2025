#pragma once
#include <de/Recti.hpp>
#include <de/Rectf.hpp>
#include <de/Image.hpp>

namespace de {

// =======================================================================
// ImageRef = Image + Recti and converter to Rectf tex coords
// ImageRef = SubImage
// ImageRef = ImagePart
// ImageRef = ImageAtlasRef
// =======================================================================
struct ImageRef
// =======================================================================
{
   DE_CREATE_LOGGER("de.ImageRef")

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
   int32_t x() const { return m_rect.x(); }
   int32_t y() const { return m_rect.y(); }
   int32_t w() const { return m_rect.w(); }
   int32_t h() const { return m_rect.h(); }
   int32_t x1() const { return m_rect.x1(); }
   int32_t y1() const { return m_rect.y1(); }
   int32_t x2() const { return m_rect.x2(); }
   int32_t y2() const { return m_rect.y2(); }
   glm::ivec2 size() const { return m_rect.size(); }

   Image const * getImage() const { return m_img; }
   Image * getImage() { return m_img; }
   Recti const & getRect() const { return m_rect; }
   int32_t getX() const { return x(); }
   int32_t getY() const { return y(); }
   int32_t getWidth() const { return w(); }
   int32_t getHeight() const { return h(); }
   int32_t getX1() const { return x1(); }
   int32_t getY1() const { return y1(); }
   int32_t getX2() const { return x2(); }
   int32_t getY2() const { return y2(); }
   glm::ivec2 getSize() const { return size(); }
   uint32_t getPixel( int32_t i, int32_t j ) const { return pixel( i, j ); }
   Rectf getTexCoords( bool useOffset = true ) const { return texCoords( useOffset ); }



};

} // end namespace de.

inline std::ostream&
operator<< ( std::ostream & o, de::ImageRef const & img )
{
   o << img.toString(); return o;
}
