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
struct Primitive2DRenderer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.Primitive2DRenderer")

   Primitive2DRenderer( VideoDriver* driver = nullptr );
   ~Primitive2DRenderer();
   void setDriver( VideoDriver* driver );
   void render();
   void clear();
   SMesh const & getMesh() const { return m_mesh; }
   SMesh & getMesh() { return m_mesh; }

   void add2DRect( Recti const & pos, uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD, TexRef const & ref = TexRef() );
   void add2DRectOutline( Recti const & pos, uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD, TexRef const & ref = TexRef(), int borderWidth = 3 );
   void add2DRoundRect( Recti const & pos, glm::ivec2 radius, uint32_t oColor, uint32_t iColor, TexRef const & ref = TexRef(), int tess = 33 );
   void add2DRoundRectOutline( Recti const & pos, glm::ivec2 radius, uint32_t color = 0xFFFFFFFF, TexRef const & ref = TexRef(), int borderWidth = 3, int tess = 33 );
   //void add2DText( int x, int y, std::string msg, uint32_t color = 0xFFFFFFFF, Align::EAlign align = Align::TopLeft, Font5x8 const & font = Font5x8(), TexRef const & ref = TexRef() );

   void
   add2DRect( Recti const & pos, uint32_t color = 0xFFFFFFFF, TexRef const & ref = TexRef() )
   {
      add2DRect( pos, color, color, color, color, ref );
   }

   void
   add2DRectOutline( Recti const & pos, uint32_t color = 0xFFFFFFFF, TexRef const & ref = TexRef(), int borderWidth = 3 )
   {
      add2DRectOutline( pos, color, color, color, color, ref, borderWidth );
   }

   void
   add2DRoundRect( Recti const & pos, glm::ivec2 radius, uint32_t color = 0xFFFFFFFF, TexRef const & ref = TexRef(), int tess = 33 )
   {
      add2DRoundRect( pos, radius, color, color, ref );
   }

/*
   void
   add3DLine( glm::vec3 a, glm::vec3 b, uint32_t colorA, uint32_t colorB );

   void
   add3DLine( glm::vec3 a, glm::vec3 b, uint32_t color = 0xFFFFFFFF )
   {
      add3DLine( a,b, color, color );
   }

   void
   add3DTriangleOutline( glm::vec3 a, glm::vec3 b, glm::vec3 c, uint32_t colorA, uint32_t colorB, uint32_t colorC );

   void
   add3DTriangleOutline( glm::vec3 a, glm::vec3 b, glm::vec3 c, uint32_t color = 0xFFFFFFFF )
   {
      add3DTriangleOutline( a,b,c, color, color, color );
   }

   void
   add3DRectOutline( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD );

   void
   add3DRectOutline( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, uint32_t color = 0xFFFFFFFF )
   {
      add3DRectOutline( a,b,c,d, color,color,color,color );
   }
*/

protected:
   void begin( PrimitiveType primType = PrimitiveType::Triangles, TexRef const & ref = TexRef() );
   VideoDriver* m_driver;
   SMeshBuffer* m_buffer;
   SMesh m_mesh; // std::vector< SMeshBuffer > m_DrawCalls;
   float m_zIndex;
};

} // end namespace gpu.
} // end namespace de.
