#include <de/scene/SMeshSceneNode.hpp>
#include <de/gpu/smesh/SMeshTool.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace scene {

using ::de::gpu::SMeshBufferTool;
using ::de::gpu::PrimitiveType;
using ::de::gpu::Blend;


SMeshSceneNode::SMeshSceneNode( SMesh const & mesh, ISceneManager* smgr, ISceneNode* parent, int id )
   : ISceneNode( smgr, parent, id ), m_mesh( mesh ), m_IsDirtyDebugMesh( true )
{
   if ( !m_Parent && m_SceneManager )
   {
      m_Parent = m_SceneManager->getRootSceneNode(); // TODO: change m_Driver to m_SceneManager
   }
}

SMeshSceneNode::SMeshSceneNode( ISceneManager* smgr, ISceneNode* parent, int id )
   : ISceneNode( smgr, parent, id ), m_mesh("SMeshSceneNode"), m_IsDirtyDebugMesh( true )
{
   if ( !m_Parent && m_SceneManager )
   {
      m_Parent = m_SceneManager->getRootSceneNode(); // TODO: change m_Driver to m_SceneManager
   }
}

SMeshSceneNode::~SMeshSceneNode()
{

}

void
SMeshSceneNode::render()
{
   if (!m_SceneManager)
   {
      /* DE_ERROR("No scene manager") */
      return;
   }
   if (!m_IsVisible)
   {
      /* DE_DEBUG("Not visible") */
      return;
   }

   auto driver = m_SceneManager->getVideoDriver();
   if (!driver)
   {
      /* DE_ERROR("No driver") */
      return;
   }

   updateDebugMesh();
   updateAbsoluteTransform();

   driver->setModelMatrix( m_AbsoluteTransform );
   driver->draw3D( m_mesh );

   // [Render] debug geometry
   if ( m_DebugData.flags > 0 && m_DebugMesh.getMeshBufferCount() > 0 )
   {
      for ( SMeshBuffer & p : m_DebugMesh.Buffers )
      {
         driver->draw3D( p );
      }
   }

}

void
SMeshSceneNode::setMesh( SMesh const & mesh )
{
   m_mesh = mesh;
   m_DebugMesh.clear();
   m_IsDirtyDebugMesh = true;

   if ( m_Name.empty() && !m_mesh.getName().empty() )
   {
      m_Name = m_mesh.getName();
   }
   updateDebugMesh();
}

void
SMeshSceneNode::setDebugData( VisualDebugData const & debugFlags )
{
   if ( m_DebugData == debugFlags ) return;
   m_DebugData = debugFlags;
   m_IsDirtyDebugMesh = true;
   updateDebugMesh();
}

void
SMeshSceneNode::updateDebugMesh()
{
   if ( !m_IsDirtyDebugMesh ) { return; } // nothing todo

   m_DebugMesh.clear();

   // [Debug] Tiny bboxes
   if ( m_DebugData.flags & VisualDebugData::BBOXES )
   {
      for ( SMeshBuffer const & p : m_mesh.Buffers )
      {
         SMeshBuffer dst( PrimitiveType::Lines );
         dst.setLighting( 0 );
         dst.setBlend( Blend::alphaBlend() );
         SMeshBufferTool::addLineBox( dst, p.getBoundingBox().widen( 0.1f ), m_DebugData.bboxesColor );
         m_DebugMesh.addMeshBuffer( dst );
      }
   }

   // [Debug] Normals (red)
   if ( m_DebugData.flags & VisualDebugData::NORMALS )
   {
      for ( SMeshBuffer const & p : m_mesh.Buffers )
      {
         SMeshBuffer dst( PrimitiveType::Lines );
         dst.setLighting( 0 );
         dst.setBlend( Blend::alphaBlend() );
         SMeshBufferTool::addLineNormals( dst, p, m_DebugData.normalLength, m_DebugData.normalColor );
         m_DebugMesh.addMeshBuffer( dst );
      }
   }

   // [Debug] Wireframe overlay
   if ( m_DebugData.flags & VisualDebugData::WIREFRAME )
   {
      for ( SMeshBuffer const & p : m_mesh.Buffers )
      {
         SMeshBuffer dst( PrimitiveType::Lines );
         dst.setLighting( 0 );
         dst.setBlend( Blend::alphaBlend() );
         SMeshBufferTool::addWireframe( dst, p, m_DebugData.wireframeScale, m_DebugData.wireframeColor );
         m_DebugMesh.addMeshBuffer( dst );
      }
   }

   // [Debug] Mesh bbox
   if ( m_DebugData.flags & VisualDebugData::BBOX )
   {
      SMeshBuffer dst( PrimitiveType::Lines );
      dst.setLighting( 0 );
      dst.setBlend( Blend::alphaBlend() );
      SMeshBufferTool::addLineBox( dst, m_mesh.getBoundingBox().widen( 0.03f ), m_DebugData.bboxColor );
      m_DebugMesh.addMeshBuffer( dst );
   }

   m_IsDirtyDebugMesh = false;
}

/*
bool
SMeshSceneNode::intersectRayWithBoundingBox( Ray3< T > const & ray, gpu::MeshIntersectionResult< T > & result ) const
{
   auto abox = getAbsoluteBoundingBox();

   Box3< T >         bbox( abox.getMin(), abox.getMax() );

   T                 beginDist = std::numeric_limits< T >::max();

   T                 globalHitDist = beginDist;

   glm::tvec3< T >   globalHitPos;

   Triangle3< T >    globalTriangle;

   int32_t           globalTriangleIndex = -1;

   bool              doesIntersect = false;

   for ( size_t i = 0; i < 12; ++i ) // Loop 12 bbox triangles
   {
      glm::tvec3< T > localHitPos; // local

      Triangle3< T > const localTriangle = bbox.getTriangle( i );

      if ( localTriangle.intersectRay( ray.getPos(), ray.getDir(), localHitPos ) )
      {
         auto localHitDist = glm::length( ray.getPos() - localHitPos );
         if ( localHitDist == beginDist || globalHitDist > localHitDist )
         {
            doesIntersect = true;
            globalHitDist = localHitDist;
            globalHitPos = localHitPos;
            globalTriangle = localTriangle;
            globalTriangleIndex = i;
         }
      }
      //DE_DEBUG("Mesh[",i,"].Triangle[", t, "] = ", tri.toString() )
   }

   if ( doesIntersect )
   {
      result.meshIndex = -1;     // indicates bbox
      result.bufferIndex = -1;   // indicates bbox
      result.triangle = globalTriangle;
      result.triangleIndex = globalTriangleIndex; // triangle of bbox 0..11
      result.distance = globalHitDist;
      result.position = globalHitPos;
   }

   return doesIntersect;
}

bool
SMeshSceneNode::intersectRay( Ray3< T > const & ray, gpu::MeshIntersectionResult< T > & result ) const
{
//   if ( !intersectRayWithBoundingBox( ray, result ) )
//   {
//      return false;
//   }

   bool doesIntersect = false;

   T const beginDist = std::numeric_limits< T >::max();

   T globalHitDist = beginDist;

   glm::tvec3< T > globalHitPos;

   uint32_t globalBufferIndex = 0;

   Triangle3< T > globalTriangle;

   size_t globalTriangleIndex = 0;

   auto const trs = getAbsoluteTransform();

   for ( size_t i = 0; i < m_mesh.Buffers.size(); ++i )
   {
      SMeshBuffer const & buffer = m_mesh.Buffers[ i ];

      if ( !buffer.intersectRayWithBoundingBox( ray, result, trs ) )
      {
         continue;
      }

      buffer.forAllTriangles(
         [&] ( uint32_t triangleIdx, Triangle3f const & tri )
         {
            glm::tvec3< T > tmpPos;
            auto a = glm::tvec3< T >( trs * glm::tvec4< T >(tri.A,T(1)) );
            auto b = glm::tvec3< T >( trs * glm::tvec4< T >(tri.B,T(1)) );
            auto c = glm::tvec3< T >( trs * glm::tvec4< T >(tri.C,T(1)) );
            Triangle3< T > triangle( a, b, c );
            if ( triangle.intersectRay( ray.getPos(), ray.getDir(), tmpPos ) )
            {
               auto tmpDist = glm::length( ray.getPos() - tmpPos );
               //DE_DEBUG("Node[",i,"](", node->getName(),").Triangle[",t,"].Hit(", tmpPosition,"), "
               //   "d(", d, "), dist(", dist, "), tri(", tri.toString(), ")")
               if ( globalHitDist == beginDist || globalHitDist >= tmpDist )
               {
                  doesIntersect = true;
                  globalBufferIndex = uint32_t( i );
                  globalHitDist = tmpDist;
                  globalHitPos = tmpPos;
                  globalTriangle = triangle;
                  globalTriangleIndex = triangleIdx;
               }
            }
            //DE_DEBUG("Mesh[",i,"].Triangle[", t, "] = ", tri.toString() )
         }
      );
   }

   if ( doesIntersect )
   {
      result.meshIndex = 0;
      result.bufferIndex = globalBufferIndex;
      result.triangle = globalTriangle;
      result.triangleIndex = globalTriangleIndex;
      result.distance = globalHitDist;
      result.position = globalHitPos;

      //DE_DEBUG("Got intersect buffer ", result.bufferIndex, " of ", m_mesh.getMeshBufferCount())
   }

   return doesIntersect;
}

void
SMeshSceneNode::createCollisionTriangles()
{
   updateAbsoluteTransform();

   m_CollisionTriangles.clear();
   if ( m_mesh )
   {
      m_CollisionTriangles = m_mesh.createCollisionTriangles();
   }

   glm::dmat4 const & modelMat = getAbsoluteTransform();
   for ( size_t i = 0; i < m_CollisionTriangles.size(); ++i )
   {
      m_CollisionTriangles[ i ].transform( modelMat );
   }
}
*/


DebugMesh::DebugMesh()
{}

void
DebugMesh::clear()
{
   debugMesh.clear();
}

void
DebugMesh::add( SMesh const & mesh, VisualDebugData const & dbgData )
{
   for ( SMeshBuffer const & src : mesh.Buffers )
   {
      add( src, dbgData );
   }
}

void
DebugMesh::add( SMeshBuffer const & src, VisualDebugData const & dbgData )
{
   debugData = dbgData;

   if ( debugMesh.getMeshBufferCount() < 1 )
   {
      SMeshBuffer dst( PrimitiveType::Lines );
      dst.setLighting( 0 );
      dst.setBlend( Blend::alphaBlend() );
      debugMesh.addMeshBuffer( dst );
   }

   SMeshBuffer & mesh = debugMesh.Buffers.back();

   if ( debugData.flags & VisualDebugData::BBOXES )
   {
      SMeshBufferTool::addLineBox( mesh, src.BoundingBox.widen( 0.1f ), debugData.bboxesColor );
   }
   if ( debugData.flags & VisualDebugData::NORMALS )
   {
      SMeshBufferTool::addLineNormals( mesh, src, debugData.normalLength, debugData.normalColor );
   }
   if ( debugData.flags & VisualDebugData::WIREFRAME )
   {
      SMeshBufferTool::addWireframe( mesh, src, debugData.wireframeScale, debugData.wireframeColor );
   }
   if ( debugData.flags & VisualDebugData::BBOX )
   {
      SMeshBufferTool::addLineBox( mesh, src.BoundingBox.widen( 0.03f ), debugData.bboxColor );
   }
}

} // end namespace scene.
} // end namespace de.


