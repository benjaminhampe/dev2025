#pragma once
#include <de/scene/ICamera.hpp>
#include <de/scene/SMeshSceneNode.hpp>
#include <de/RuntimeError.hpp>

namespace de {
namespace scene {

template < typename T >
// =======================================================================
struct ScenePickResult
// =======================================================================
{
   ISceneNode* node = nullptr;
   glm::tvec3< T > pos;
   uint32_t triangleIndex = 0;
   uint32_t nodeIndex = 0;
   T const distMax = std::numeric_limits< T >::max();
   T dist = distMax;
   bool doesIntersect = false;
};

// =======================================================================
struct ISceneManager
// =======================================================================
{
   typedef double T; // computeRay

   virtual ~ISceneManager() {}
   virtual void render() = 0;
   virtual void resize( int w, int h ) = 0;
   virtual void init( int w, int h, int logLevel ) = 0;
   virtual void clear() = 0;
   virtual void clearTrashBin() = 0;
//   virtual void onFrameBegin() = 0;
//   virtual void onFrameEnd() = 0;
   //virtual void postEvent( SEvent event ) = 0;
   virtual ISceneNode* getHoveredSceneNode() = 0;
   virtual ISceneNode* getPickedSceneNode() = 0;

   virtual void setDebugData( VisualDebugData const & debugData ) = 0;

   virtual gpu::VideoDriver* getVideoDriver() = 0;
   virtual ISceneNode* getRootSceneNode() = 0;
   virtual std::vector< ISceneNode* > const & getSceneNodes() const = 0;
   virtual std::vector< ISceneNode* > & getSceneNodes() = 0;

   virtual std::vector< ISceneNode* >
   getSceneNodesInRange( glm::vec3 pos, float radius )
   {
      std::vector< ISceneNode* > result;

      auto const & nodes = getSceneNodes();
      for ( auto const & node : nodes )
      {
         if (!node) continue;
         auto bbox = node->getAbsoluteBoundingBox();
         if ( glm::length2( bbox.getCenter() - pos ) <= radius * radius )
         {
            result.emplace_back( node );
         }
      }
      return result;
   }

   //virtual void clearDebugData();
   virtual uint32_t getBufferCount( int mode ) const = 0;
   virtual uint64_t getVertexCount( int mode ) const = 0;
   virtual uint64_t getIndexCount( int mode ) const = 0;
   virtual uint64_t getByteCount( int mode ) const = 0;
   virtual uint32_t getMeshCount() const = 0;
   virtual bool hasMesh( std::string const & name ) const = 0;
   virtual void removeMesh( std::string const & name ) = 0;
   virtual SMesh& addEmptyMesh( std::string name ) = 0;
   virtual SMesh& getMesh( std::string const & name ) = 0;
   virtual bool addMesh( SMesh mesh ) = 0;

   virtual void addSceneNode( ISceneNode* node ) = 0;
   virtual void removeSceneNode( ISceneNode* node ) = 0;

   virtual ISceneNode*
   getSceneNode( std::string const & name ) = 0;

   virtual SMeshSceneNode*
   createMeshSceneNode( SMesh const & mesh, ISceneNode* parent = nullptr, int id = -1 ) = 0;

   virtual SMeshSceneNode*
   createMeshSceneNode( std::string uri, ISceneNode* parent = nullptr, int id = -1 ) = 0;

   virtual ICamera* getCamera() const = 0;
   virtual bool setCamera( ICamera* camera ) = 0;
   virtual void setDefaultCamera() = 0;

   virtual std::unordered_map< std::string, SMesh > const &
   getMeshCache() const = 0;

   virtual ISceneNode*
   intersectRay( de::Ray3< T > const & ray,
                 gpu::MeshIntersectionResult< T > & result ) = 0;

   virtual ISceneNode*
   pickSceneNode() = 0;

};

} // end namespace scene.
} // end namespace de.

