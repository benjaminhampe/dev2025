#pragma once
#include <de_gpu/de_Tex.h>
#include <de_core/de_FileSystem.h>

namespace de {

// ===========================================================================
struct TexRef
// ===========================================================================
{
   //Image* m_img;     // the CPU-Side original image, if avail
   Tex* m_tex;  // the full GPU texture(w,h)
   //uint32_t m_w; // copy for faster lookup
   //uint32_t m_h; // copy for faster lookup
   //int m_id;
   //int m_loadCounter;
   // int m_refCount;
   Recti m_rect;   // irect - integral subrect of (w,h) in pixels
   Rectf m_coords; // frect - fractional subrect of (w,h) in multiples of one.
   glm::vec2 m_repeat; // scale?
   //std::string m_name;
   //std::string m_uri;

   TexRef()
      : m_tex( nullptr )
//      , m_w(0)
//      , m_h(0)
//      , m_id(-1)
//      , m_loadCounter(0)
      , m_rect( 0,0,0,0 )
      , m_coords( 0,0,1,1 )
      , m_repeat( 1,1 )
   {}
   TexRef( Tex* ptr, bool useOffset = false )
      : m_tex( ptr )
//      , m_w(0)
//      , m_h(0)
//      , m_id(-1)
//      , m_loadCounter(0)
      , m_rect( 0,0,0,0 )
      , m_coords( 0,0,1,1 )
      , m_repeat( 1,1 )
   {
      if ( m_tex )
      {
         auto w = int( m_tex->getWidth() );
         auto h = int( m_tex->getHeight() );
         m_rect = Recti( 0, 0, w, h );
         m_coords = Rectf::fromRecti( m_rect, w, h, useOffset );
      }
   }

   TexRef( Tex* ptr, Recti const & pos, bool useOffset = false )
      : m_tex( ptr )
//      , m_w(0)
//      , m_h(0)
//      , m_id(-1)
//      , m_loadCounter(0)
      , m_rect( pos )
      , m_coords( 0,0,1,1 )
      , m_repeat( 1,1 )
   {
      if ( m_tex )
      {
         auto w = int( m_tex->getWidth() );
         auto h = int( m_tex->getHeight() );
         m_coords = Rectf::fromRecti( m_rect, w, h, useOffset );
      }
   }

   Tex const* getTexture() const { return m_tex; }
   Tex* getTexture() { return m_tex; }

   //   Tex const* texture() const { return m_tex; }
   //   Tex* texture() { return m_tex; }

   //Image* img() { return m_img; }
   //Image const* img() const { return m_img; }
   Rectf const & texCoords() const { return m_coords; }
   float u1() const { return m_coords.u1(); }
   float v1() const { return m_coords.v1(); }
   float u2() const { return m_coords.u2(); }
   float v2() const { return m_coords.v2(); }
   float du() const { return m_coords.du(); }
   float dv() const { return m_coords.dv(); }
   Rectf const & getTexCoords() const { return m_coords; }
   float getU1() const { return m_coords.u1(); }
   float getV1() const { return m_coords.v1(); }
   float getU2() const { return m_coords.u2(); }
   float getV2() const { return m_coords.v2(); }
   float getDU() const { return m_coords.du(); }
   float getDV() const { return m_coords.dv(); }

   // Sub irect
   Recti const & rect() const { return m_rect; }
   int x() const { return m_rect.x(); }
   int y() const { return m_rect.y(); }
   int w() const { return m_rect.w(); }
   int h() const { return m_rect.h(); }
   Recti const & getRect() const { return m_rect; }
   int getX() const { return m_rect.x(); }
   int getY() const { return m_rect.y(); }
   int getWidth() const { return m_rect.w(); }
   int getHeight() const { return m_rect.h(); }

   // Full texture stuff
   int texWidth() const;
   int texHeight() const;
   glm::ivec2 texSize() const;
   glm::vec2 texSizef() const;

   int getTexWidth() const { return m_tex ? int(m_tex->getWidth()) : 0; }
   int getTexHeight() const { return m_tex ? int(m_tex->getHeight()) : 0; }

   glm::ivec2 getTexSize() const
   {
      if (!m_tex) return glm::ivec2{0,0};
      return glm::ivec2{ int(m_tex->getWidth()), int(m_tex->getHeight()) };
   }
   glm::vec2 getTexSizef() const { return getTexSize(); }

   std::string toString() const
   {
      std::ostringstream o;
      if (m_tex) { o << FileSystem::fileName(m_tex->getName()); }
      else       { o << "0"; }
      o << ", rect:" << getRect()
      //  << ", semantic:" << int(m_tex->getTexSemantic())
      ;
      return o.str();
   }


   bool empty() const
   {
      if ( !m_tex || w() < 1 || h() < 1 )
         return true;
      else
         return false;
   }

   void setMatrix( float tx, float ty, float sx, float sy )
   {
      m_coords = Rectf( tx,ty,sx,sy );
      if (m_tex)
      {
         auto w = int( m_tex->getWidth() );
         auto h = int( m_tex->getHeight() );
         m_rect = m_coords.toRecti( w, h );
      }
      else
      {
         m_rect = Recti(0,0,0,0);
      }
   }

   glm::vec4 getTexTransform() const
   {
      return glm::vec4( m_coords.x(), m_coords.y(), m_coords.w(), m_coords.h() );
   }

   /*
   // vec4.<x,y> = translation x,y ( atlas tex chunk pos )
   // vec4.<z,w> = scale x,y ( atlas tex chunk size )
   Image TexRef::copyImage() const
   {
      uint32_t w = ref.rect.getWidth();
      uint32_t h = ref.rect.getHeight();
      uint32_t x = ref.rect.getX();
      uint32_t y = ref.rect.getY();
      DE_DEBUG("id:",id, ", w:",w, ", h:",h, ", x:",x, ", y:",y, ", img:", ref.m_img->toString())

      Image img( w,h );
      for ( uint32_t j = 0; j < h; ++j )
      {
      for ( uint32_t i = 0; i < w; ++i )
      {
         img.setPixel( i,j, ref.img->getPixel(x+i,y+j) );
      }
      }

      return img;
   }
   */
};

} // end namespace de.
