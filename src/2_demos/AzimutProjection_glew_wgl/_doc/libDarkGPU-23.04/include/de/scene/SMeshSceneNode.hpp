#pragma once
#include <de/scene/ISceneNode.hpp>
#include <de/gpu/smesh/SMesh.hpp>

namespace de {
namespace scene {

using ::de::gpu::SMeshBuffer;
using ::de::gpu::SMesh;
using ::de::gpu::Blend;
using ::de::gpu::Culling;
using ::de::gpu::Depth;
using ::de::gpu::TexRef;

// =======================================================================
struct SMeshSceneNode : public ISceneNode
// =======================================================================
{
   SMeshSceneNode( ISceneManager* smgr, ISceneNode* parent = nullptr, int id = -1 );
   SMeshSceneNode( SMesh const & mesh, ISceneManager* smgr, ISceneNode* parent = nullptr, int id = -1 );
   ~SMeshSceneNode() override;
   void render() override;

   // SceneNodeType::EType getNodeType() const override { return SceneNodeType::Mesh; }
   SMesh const & getMesh() const { return m_mesh; }
   SMesh& getMesh() { return m_mesh; }
   void setMesh( SMesh const & mesh );

   Box3f getBoundingBox() const override { return m_mesh.getBoundingBox(); }
   std::string getName() const override { return m_mesh.Name; }
   void setName( std::string const & name ) override { m_mesh.setName( name ); }

   void setDebugData( VisualDebugData const & debugFlags ) override;

   uint32_t getBufferCount() const
   {
      size_t n = m_mesh.getMeshBufferCount();
      if ( getDebugData().flags > 0 ) n += m_DebugMesh.getVertexCount();
      return n;
   }

   uint64_t getVertexCount() const
   {
      uint64_t n = m_mesh.getVertexCount();
      if ( getDebugData().flags > 0 ) n += m_DebugMesh.getVertexCount();
      return n;
   }

   uint64_t getIndexCount() const
   {
      uint64_t n = m_mesh.getIndexCount();
      if ( getDebugData().flags > 0 ) n += m_DebugMesh.getIndexCount();
      return n;
   }

   uint64_t getByteCount() const
   {
      uint64_t n = sizeof( *this ) + m_mesh.getByteCount();
      if ( getDebugData().flags > 0 ) n += m_DebugMesh.getByteCount();
      return n;
   }

   int getLighting() const { return m_mesh.getLighting(); }
   int getFogEnable() const { return m_mesh.getFogEnable(); }
   int getWireframe() const { return m_mesh.getWireframe(); }

   void setLighting( int illum ) { m_mesh.setLighting( illum ); }
   void setFogEnable( int enable ) { m_mesh.setFogEnable( enable ); }
   void setWireframe( int enable ) { m_mesh.setWireframe( enable ); }
   void setCulling( Culling cull ) { m_mesh.setCulling( cull ); }
   void setCulling( bool enable ) { m_mesh.setCulling( enable ); }
   void setDepth( Depth depth ) { m_mesh.setDepth( depth ); }
   void setTexture( int stage, TexRef const & tex ) { m_mesh.setTexture( stage, tex ); }

   bool
   intersectRayWithBoundingBox( Ray3< T > const & ray,
                                gpu::MeshIntersectionResult< T > & result ) const override
   {
      return m_mesh.intersectRayWithBoundingBox( ray, result );
   }

   bool
   intersectRay( Ray3< T > const & ray,
                 gpu::MeshIntersectionResult< T > & result ) const override
   {
      return m_mesh.intersectRay( ray, result );
   }

protected:

   void updateDebugMesh();

   DE_CREATE_LOGGER("de.gpu.SMeshSceneNode")
   SMesh m_mesh;
   bool m_IsDirtyDebugMesh;
   SMesh m_DebugMesh;
   //std::vector< Triangle3f > m_CollisionTriangles;
};

// =======================================================================
struct DebugMesh
// =======================================================================
{
   VisualDebugData debugData;
   SMesh debugMesh;

   void clear();

   void add( SMeshBuffer const & dbgMesh,
               VisualDebugData const & dbgData = VisualDebugData::ALL );

   void add( SMesh const & dbgMesh,
               VisualDebugData const & dbgData = VisualDebugData::ALL  );

   DebugMesh();
};

} // end namespace scene.
} // end namespace de.
