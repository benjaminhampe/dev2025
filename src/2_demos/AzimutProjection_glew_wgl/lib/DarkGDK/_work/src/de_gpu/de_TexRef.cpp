#include <de_gpu/de_TexRef.h>

namespace de {

/*
// ===========================================================================
TexRef::TexRef()
// ===========================================================================


TexRef::TexRef( Tex* ptr, bool useOffset )

TexRef::TexRef( Tex* ptr, Recti const & pos, bool useOffset )


//bool
//TexRef::operator==( TexRef const & other ) const
//{
//   if ( other.m_tex != m_tex ) return false;
//   if ( other.m_rect != m_rect ) return false;
//   if ( other.m_repeat != m_repeat )
//   {
//      DE_WARN("Differs in m_repeat tex scaling")
//      return false;
//   }
//   return true;
//}

Image*         TexRef::img()
Image const*   TexRef::img() const { return m_img; }
Rectf const &  TexRef::texCoords() const
float          TexRef::u1() const
float          TexRef::v1() const { return m_coords.v1(); }
float          TexRef::u2() const { return m_coords.u2(); }
float          TexRef::v2() const { return m_coords.v2(); }
float          TexRef::du() const { return m_coords.du(); }
float          TexRef::dv() const
Rectf const &  TexRef::getTexCoords() const { return texCoords(); }
float          TexRef::getU1() const { return u1(); }
float          TexRef::getV1() const { return v1(); }
float          TexRef::getU2() const { return u2(); }
float          TexRef::getV2() const { return v2(); }
float          TexRef::getDU() const { return du(); }
float          TexRef::getDV() const { return dv(); }
Recti const &  TexRef::rect() const
int            TexRef::x() const
int            TexRef::y() const { return m_rect.y(); }
int            TexRef::w() const { return m_rect.w(); }
int            TexRef::h() const { return m_rect.h(); }
Recti const &  TexRef::getRect() const { return rect(); }
int            TexRef::getX() const { return x(); }
int            TexRef::getY() const { return y(); }
int            TexRef::getWidth() const { return w(); }
int            TexRef::getHeight() const { return h(); }
int            TexRef::texWidth() const { return m_w; }
int            TexRef::texHeight() const { return m_h; }
glm::ivec2     TexRef::texSize() const { return glm::ivec2(m_w,m_h); }
glm::vec2      TexRef::texSizef() const { return glm::vec2(m_w,m_h); }
int            TexRef::getTexWidth() const { return texWidth(); }
int            TexRef::getTexHeight() const { return texHeight(); }
glm::ivec2     TexRef::getTexSize() const { return texSize(); }
glm::vec2      TexRef::getTexSizef() const { return texSizef(); }
std::string    TexRef::toString() const

bool           TexRef::empty() const


void           TexRef::setMatrix( float tx, float ty, float sx, float sy )


glm::vec4      TexRef::getTexTransform() const



Image
TexRef::copyImage() const

*/

} // end namespace de.
