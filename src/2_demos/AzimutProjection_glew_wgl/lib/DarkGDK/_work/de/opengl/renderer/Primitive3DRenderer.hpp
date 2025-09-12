#pragma once
#include <de/gpu/smesh/SMesh.hpp>
#include <de/gpu/renderer/Font5x8Renderer.hpp>

namespace de {
namespace gpu {

struct VideoDriver;

// Has some intelligence and reduces DrawCalls immensily
// Adds shapes (colored or textured) in any order the user wants.
// Internally adds a new buffer to the mesh everytime Tex or PrimitiveType change.
// So it can construct a rich content screen easily
// Supported shapes are S2DDot, S2DLine, S2DRect, S2DRoundRect and S2DCirle.
// ===========================================================================
struct Primitive3DRenderer
// ===========================================================================
{
   typedef float T;
   typedef glm::vec3 V3;
   DE_CREATE_LOGGER("de.gpu.Primitive3DRenderer")

   constexpr static const T CONST_Z_INIT = T(0.90); // At 1.0 it disappears, not inside frustum.
   constexpr static const T CONST_Z_STEP = T(-0.00001); // enough for 200.000 elements, TODO: test more.

   SMeshBuffer m_lines;
   SMeshBuffer m_points;
   SMeshBuffer m_triangles;

   Primitive3DRenderer( VideoDriver* driver = nullptr );
   ~Primitive3DRenderer();
   void setDriver( VideoDriver* driver );
   void clear();
   void render();

   // New helper:
   void addLine( T x1, T y1, T z1, T x2, T y2, T z2, uint32_t colorA, uint32_t colorB )
   {
      addLine( V3(x1,y1,z1), V3(x2,y2,z2), colorA, colorB );
   }
   void addLine( T x1, T y1, T z1, T x2, T y2, T z2, uint32_t color = 0xFFFFFFFF )
   {
      addLine( V3(x1,y1,z1), V3(x2,y2,z2), color );
   }

   void addLine( glm::vec3 a, glm::vec3 b, uint32_t colorA, uint32_t colorB );
   void addLine( glm::vec3 a, glm::vec3 b, uint32_t color = 0xFFFFFFFF );
   void addLineTriangle( glm::vec3 a, glm::vec3 b, glm::vec3 c, uint32_t colorA, uint32_t colorB, uint32_t colorC );
   void addLineTriangle( glm::vec3 a, glm::vec3 b, glm::vec3 c, uint32_t color = 0xFFFFFFFF );
   void addLineRect( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD );
   void addLineRect( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, uint32_t color = 0xFFFFFFFF );

protected:
   VideoDriver* m_driver;
   SMeshBuffer* m_buffer;
};

} // end namespace gpu.
} // end namespace de.
