#pragma once
#include <de/gpu/smesh/SMesh.hpp>

namespace de {
namespace gpu {

struct VideoDriver;

// Has some intelligence and reduces DrawCalls immensily
// Adds shapes (colored or textured) in any order the user wants.
// Internally adds a new buffer to the mesh everytime Tex or PrimitiveType change.
// So it can construct a rich content screen easily
// Supported shapes are S2DDot, S2DLine, S2DRect, S2DRoundRect and S2DCirle.
// ===========================================================================
struct Primitive3DList
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.Primitive3DList")

   Primitive3DList( VideoDriver* driver = nullptr );
   ~Primitive3DList();
   void setDriver( VideoDriver* driver );
   void render();
   void clear();
   SMesh const & getMesh() const { return m_Mesh; }
   SMesh & getMesh() { return m_Mesh; }

   void
   add3DLine( glm::vec3 a, glm::vec3 b, uint32_t colorA, uint32_t colorB );

   inline void
   add3DLine( glm::vec3 a, glm::vec3 b, uint32_t color = 0xFFFFFFFF )
   {
      add3DLine( a,b, color, color );
   }

   void
   add3DTriangleOutline( glm::vec3 a, glm::vec3 b, glm::vec3 c, uint32_t colorA, uint32_t colorB, uint32_t colorC );

   inline void
   add3DTriangleOutline( glm::vec3 a, glm::vec3 b, glm::vec3 c, uint32_t color = 0xFFFFFFFF )
   {
      add3DTriangleOutline( a,b,c, color, color, color );
   }

   void
   add3DRectOutline( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD );

   inline void
   add3DRectOutline( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, uint32_t color = 0xFFFFFFFF )
   {
      add3DRectOutline( a,b,c,d, color,color,color,color );
   }


protected:
   void begin( PrimitiveType primType = PrimitiveType::Triangles, TexRef const & ref = TexRef() );
   void end();
   VideoDriver* m_driver;
   SMeshBuffer* m_buffer;
   SMesh m_Mesh; // std::vector< SMeshBuffer > m_DrawCalls;
};

} // end namespace gpu.
} // end namespace de.
