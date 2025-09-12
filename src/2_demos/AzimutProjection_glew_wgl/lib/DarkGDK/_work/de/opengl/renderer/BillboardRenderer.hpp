#pragma once
#include <de/gpu/smesh/SMesh.hpp>

namespace de {
namespace gpu {

struct VideoDriver;

// ===========================================================================
struct Billboard
// ===========================================================================
{
   enum EType
   {
      Rect = 0,
      RoundRect,
      Circle,
      ETypeCount
   };

   int typ = Rect;
   int tess = 33;
   glm::vec2 size;
   glm::vec2 radius; // relevant for RoundRect, not Circle
   glm::vec3 pos;
   float zoffset; // for multiple billboards at same pos, order of overlap zoffset.
   uint32_t color;
   TexRef tex;
   SMeshBuffer mesh;
};

// ===========================================================================
struct BillboardRenderer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.BillboardRenderer")
   VideoDriver* m_driver;
   SMeshBuffer meshRect;
   SMeshBuffer meshRoundRect;
   SMeshBuffer meshCircle;

   std::vector< Billboard > m_billboards;
   BillboardRenderer( VideoDriver* driver = nullptr );
   ~BillboardRenderer();

   void setDriver( VideoDriver* driver ) { m_driver = driver; }

   void clear() { m_billboards.clear(); }

   void render();

   void draw3DBillboard( IMeshBuffer & vao );

   void
   add( int typ,
        glm::vec2 size,
        glm::vec3 pos,
        glm::vec2 radius,
        uint32_t color = 0xFFFFFFFF,
        TexRef ref = TexRef() );

   void
   addRect( glm::vec2 size, glm::vec3 pos,
            uint32_t color = 0xFFFFFFFF, TexRef ref = TexRef() )
   {
      add( Billboard::Rect, size, pos, glm::vec2(0,0), color, ref );
   }

   void
   addRoundRect( glm::vec2 size, glm::vec3 pos, glm::vec2 radius,
            uint32_t color = 0xFFFFFFFF, TexRef ref = TexRef() )
   {
      add( Billboard::RoundRect, size, pos, radius, color, ref );
   }

   void
   addCircle( glm::vec2 size, glm::vec3 pos,
            uint32_t color = 0xFFFFFFFF, TexRef ref = TexRef() )
   {
      add( Billboard::Circle, size, pos, glm::vec2(0,0), color, ref );
   }

};

} // end namespace gpu.
} // end namespace de.

