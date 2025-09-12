#pragma once
#include <de/scene/ISceneManager.hpp>
#include <de/scene/CameraFPS_XZ.hpp>
#include <de/scene/SMeshSceneNode.hpp>

namespace de {
namespace scene {

// =======================================================================
struct SceneManager : public ISceneManager
// =======================================================================
{
   SceneManager( gpu::VideoDriver* driver );
   ~SceneManager() override;
   void init( int w, int h, int logLevel ) override;
   void clearTrashBin() override;
   void clear() override;
   void render() override;
   void resize( int w, int h ) override;
//   void onFrameBegin();
//   void onFrameEnd();
   // void postEvent( SEvent event );
   void setDebugData( VisualDebugData const & debugData ) override;

   gpu::VideoDriver* getVideoDriver() override { return m_driver; }
   ISceneNode* getRootSceneNode() override { return m_RootNode; }
   std::vector< ISceneNode* > const & getSceneNodes() const override { return m_TopLevel; }
   std::vector< ISceneNode* > & getSceneNodes() override { return m_TopLevel; }
   ISceneNode* getHoveredSceneNode() override { return m_Hovered; }
   ISceneNode* getPickedSceneNode() override { return m_Picked; }
   //virtual void clearDebugData();

   // #########################
   // ###     MeshCache     ###
   // #########################
   uint32_t getBufferCount( int mode ) const override;
   uint64_t getVertexCount( int mode ) const override;
   uint64_t getIndexCount( int mode ) const override;
   uint64_t getByteCount( int mode ) const override;
   uint32_t getMeshCount() const override;

   std::unordered_map< std::string, SMesh > const &
   getMeshCache() const override;

   bool hasMesh( std::string const & name ) const override;
   void removeMesh( std::string const & name ) override;
   SMesh& addEmptyMesh( std::string name ) override;
   SMesh& getMesh( std::string const & name ) override;
   bool addMesh( SMesh mesh ) override;

   ISceneNode* getSceneNode( std::string const & name ) override;
   void addSceneNode( ISceneNode* node ) override;
   SMeshSceneNode*
   createMeshSceneNode( SMesh const & mesh, ISceneNode* parent = nullptr, int id = -1 ) override;
   SMeshSceneNode*
   createMeshSceneNode( std::string uri, ISceneNode* parent = nullptr, int id = -1 ) override;
   void removeSceneNode( ISceneNode* node ) override;

   // #####################
   // ### CameraManager ###
   // #####################
   ICamera*
   getCamera() const override
   {
      return m_camera;
   }
   bool
   setCamera( ICamera* camera ) override
   {
      m_camera = camera;
      return true;
   }
   void
   setDefaultCamera() override
   {
      m_camera = &m_camera0;
   }

   // #########################
   // ### Collision control ###
   // #########################
   ISceneNode*
   intersectRay( de::Ray3< T > const & ray,
                 gpu::MeshIntersectionResult< T > & result ) override;

   ISceneNode*
   pickSceneNode() override;

private:
   DE_CREATE_LOGGER("de.gpu.SceneManager")
   gpu::VideoDriver* m_driver;    // ### Driver ###
   std::vector< ISceneNode* > m_TopLevel;    // ### SceneNodeManager ###
   std::vector< ISceneNode* > m_TrashBin;    // ### SceneNodeManager ###
   ISceneNode* m_RootNode;    // ### SceneNodeManager ###
   ISceneNode* m_Hovered;     // ### CollisionManager ###
   ISceneNode* m_Picked;      // ### CollisionManager ###
   std::unordered_map< std::string, SMesh > m_MeshLut;   // ### MeshCache ###
   Camera m_camera0;
   ICamera* m_camera;
};

} // end namespace scene.
} // end namespace de.
