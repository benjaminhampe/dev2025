#pragma once
#include <Material.hpp>
#include <map>

#ifndef USE_RENDER_STATS
#define USE_RENDER_STATS
#endif

namespace de {
namespace gpu {

struct VideoDriver;

// ===========================================================================
struct MaterialRenderer
// ===========================================================================
{
   MaterialRenderer( VideoDriver* driver );
   ~MaterialRenderer();

   //bool setMVP( Shader* shader, glm::dmat4 const & mvp );
   bool setMaterial( Material const & material );
   void unsetMaterial( Material const & material );

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
      MaterialLight light;
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

   uint32_t getLightCount() const { return m_Lights.size(); }
   MaterialLight & getLight( uint32_t light ) { return m_Lights[ light ]; }
   MaterialLight const & getLight( uint32_t light ) const { return m_Lights[ light ]; }

   // typ = 0 -> MeshBuffers per Frame
   // typ = 1 -> Vertices per Frame
   // typ = 2 -> Indices per Frame
   // typ = 3 -> Primitives per Frame
   // typ = 4 -> Total rendered Frames
   // typ = 5 -> Total rendered MeshBuffers
   // typ = 6 -> Total rendered Vertices
   // typ = 7 -> Total rendered Indices
   // typ = 8 -> Total rendered Primitives
   uint64_t getRenderStat( int typ ) const
   {
      switch ( typ )
      {
         case 0: return m_FrameBuffers;
         case 1: return m_FrameVertices;
         case 2: return m_FrameIndices;
         case 3: return m_FramePrimitives;
         case 4: return m_TotalFrames;
         case 5: return m_TotalBuffers;
         case 6: return m_TotalVertices;
         case 7: return m_TotalIndices;
         case 8: return m_TotalPrimitives;
         default: return 0;
      }
   }

   std::vector< MaterialLight > const & getLights() const { return m_Lights; }
   std::vector< MaterialLight > & getLights() { return m_Lights; }

protected:
   Shader* getShader( Material const & material ); // Could be public
   std::string createVS( Material const & material ) const;
   std::string createFS( Material const & material ) const;
   std::string createShaderName( Material const & material ) const;
   uint32_t createShaderId( Material const & material ) const;

   DE_CREATE_LOGGER("de.gpu.MaterialRenderer")
public:
   VideoDriver* m_driver;
   std::map< uint32_t, Shader* > m_ShaderLut; // get Shader by faster id, not a name.
   float m_ScreenGamma;
   std::vector< MaterialLight > m_Lights;
   AnimTimer m_AnimTime;
   Material m_Material;
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

} // end namespace gpu.
} // end namespace de.
