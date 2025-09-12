#pragma once
#include <de_gpu/de_VideoDriver.h>
#include <de_gpu/de_SMesh.h>
#include <map>

#ifndef USE_RENDER_STATS
#define USE_RENDER_STATS
#endif

namespace de {

// ===========================================================================
struct SMaterialRenderer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.SMaterialRenderer")

public:
   SMaterialRenderer();
   ~SMaterialRenderer();

   void setDriver( VideoDriver* driver ) { m_Driver = driver; }

   //bool setMVP( Shader* shader, glm::dmat4 const & mvp );
   bool setMaterial( SMaterial const & material );
   void unsetMaterial( SMaterial const & material );

   // New: Mostly for render stats, counts vertices, indices and primitives
   void beginFrame();
   void endFrame();
   void drawLights();
   void animate( double pts ); // called once each frame, in Driver::beginFrame()

   // === Set/Get Shader Uniform Gamma
   float getGamma() const { return m_ScreenGamma; }
   void setGamma( float gamma ) { m_ScreenGamma = gamma; }
   // === Lights ===, could move to driver

   /* typ = 0 is a point light like the sun, dir is computed by pos only
    * typ = 1 is a directional cone light like a Taschenlampe,
    * typ = 2 is a directional cylinder with round caps like a neon light
    */
   void
   addLight( int typ = 1,
             glm::vec3 siz = glm::vec3(10,10,10),
             glm::vec3 pos = glm::vec3(0,0,0),
             float maxdist = 500.0f,
             glm::vec3 dir = glm::vec3(0,-1,0),
             uint32_t color = 0XFFFFFFFF )
   {
      SMaterialLight light;
      light.typ = typ;
      light.siz = siz;
      light.pos = pos;
      light.maxdist = maxdist;
      light.dir = dir;
      light.color = glm::vec3( RGBAf( color ) );
      //light.mesh.setPrimitiveType( PrimitiveType::Triangles );
      //light.mesh.setLighting( 0 );
      //SMeshSphere::add( light.mesh, light.siz, light.pos, color, 12, 12 );
      m_Lights.emplace_back( std::move( light ) );
   }

   uint32_t getLightCount() const { return uint32_t(m_Lights.size()); }
   SMaterialLight & getLight( uint32_t light ) { return m_Lights[ light ]; }
   SMaterialLight const & getLight( uint32_t light ) const { return m_Lights[ light ]; }

protected:
   Shader* getShader( SMaterial const & material ); // Could be public
   std::string createVS( SMaterial const & material ) const;
   std::string createFS( SMaterial const & material ) const;
   std::string createShaderName( SMaterial const & material ) const;
   uint32_t createShaderId( SMaterial const & material ) const;

public:
   VideoDriver* m_Driver;
   std::map< uint32_t, Shader* > m_ShaderLut; // get Shader by faster id, not a name.
   float m_ScreenGamma;
   std::vector< SMaterialLight > m_Lights;
   AnimTimer m_AnimTime;
   SMaterial m_Material;
//   glm::vec3 m_light0_color;
//   glm::vec3 m_light0_dir;
//   glm::vec3 m_light1_color;
//   glm::vec3 m_light1_dir;
//   glm::vec3 m_light2_color;
//   glm::vec3 m_light2_dir;
//   glm::vec3 m_light3_color;
//   glm::vec3 m_light3_dir;

#ifdef USE_RENDER_STATS
   uint64_t m_FrameBuffers = 0;
   uint64_t m_FrameVertices = 0;
   uint64_t m_FrameIndices = 0;
   uint64_t m_FramePrimitives = 0;
   uint64_t m_TotalFrames = 0;
   uint64_t m_TotalBuffers = 0;
   uint64_t m_TotalVertices = 0;
   uint64_t m_TotalIndices = 0;
   uint64_t m_TotalPrimitives = 0;
#endif
};

} // end namespace de.
