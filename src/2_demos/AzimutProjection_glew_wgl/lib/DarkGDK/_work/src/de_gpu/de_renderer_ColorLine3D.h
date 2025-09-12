#pragma once
#include <cstdint>
#include <vector>
#include <de_gpu/de_VideoDriver.h>
#include <de_gpu/de_FVF.h>

namespace de {

#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct ColorLine3DVertex
// ===========================================================================
{
   glm::vec3 m_pos;
   uint32_t m_color;

   ColorLine3DVertex()
      : m_pos(), m_color(0xFFFFFFFF)
   {}
   ColorLine3DVertex( glm::vec3 pos, uint32_t color = 0xFFFFFFFF )
      : m_pos( pos ), m_color( color )
   {}
   ColorLine3DVertex( float x, float y, float z, uint32_t color = 0xFFFFFFFF )
      : m_pos( x,y,z ), m_color( color )
   {}
};

#pragma pack( pop )

// ===========================================================================
struct ColorLine3DMeshBuffer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.ColorLine3DMeshBuffer")
   PrimitiveType m_primType;
   uint32_t m_vao;
   uint32_t m_vbo;
   bool m_shouldUpload;
   std::string m_debugName;
   std::vector< ColorLine3DVertex > m_vertices;
   //std::vector< uint32_t > m_indices;

   ColorLine3DMeshBuffer();
   ~ColorLine3DMeshBuffer();
   void setDebugName( std::string const & name ) { m_debugName = name; }
   size_t computeMemoryConsumption() const;
   bool empty() const { return m_vertices.empty(); }
   void destroy();
   void clear();
   void upload();
   void render();
   void addLine(  glm::vec3 a, glm::vec3 b,
                  uint32_t color_a, uint32_t color_b );
   void addTriangle( glm::vec3 a, glm::vec3 b, glm::vec3 c,
                     uint32_t color_a, uint32_t color_b, uint32_t color_c );

   void
   addLine( glm::vec3 a, glm::vec3 b, uint32_t color )
   {
      addLine(a,b,color,color);
   }

   void
   addTriangle( glm::vec3 a, glm::vec3 b, glm::vec3 c, uint32_t color )
   {
      addTriangle( a, b, c, color, color, color );
   }
};

// ===========================================================================
struct ColorLine3DRenderer
// ===========================================================================
{
   VideoDriver* m_driver;
   Shader* m_shader;

   ColorLine3DRenderer();
   ~ColorLine3DRenderer();

   void init( VideoDriver* driver );
   void destroy();

   void render( ColorLine3DMeshBuffer & meshBuffer, glm::mat4 const & mvp = glm::mat4(1.0f) );
};

} // end namespace GL.
