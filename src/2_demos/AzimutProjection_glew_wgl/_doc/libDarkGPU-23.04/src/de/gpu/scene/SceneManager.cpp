#include <de/scene/SceneManager.hpp>
#include <de/scene/CameraFPS_XZ.hpp>
#include <de/gpu/VideoDriver.hpp>
#include <de/gpu/smesh/io/SMeshIO.hpp>

namespace de {
namespace scene {

using ::de::gpu::SMeshIO;

// =======================================================================
SceneManager::SceneManager( gpu::VideoDriver* driver )
// =======================================================================
   : m_driver( driver )
   , m_RootNode( nullptr )
   , m_Hovered( nullptr )
   , m_Picked( nullptr )
//   , m_LastHoveredNode( nullptr )
//   , m_SelectedNode( nullptr )
//   , m_LastSelectedNode( nullptr )
   , m_camera( nullptr )
{
   //DE_DEBUG("")
   setDefaultCamera();
}

SceneManager::~SceneManager()
{
   clear();
}
//VideoDriver* getVideoDriver() { return m_driver; }
//ISceneNode* getRootSceneNode() { return m_RootNode; }
//std::vector< ISceneNode* > const & getSceneNodes() const { return m_TopLevel; }
//std::vector< ISceneNode* > & getSceneNodes() { return m_TopLevel; }
//ISceneNode* getHoveredSceneNode() { return m_Hovered; }
//ISceneNode* getPickedSceneNode() { return m_Picked; }
//virtual void clearDebugData();
uint32_t
SceneManager::getBufferCount( int mode ) const
{
   size_t n = 0;
   for ( ISceneNode const * const p : m_TopLevel ) { if ( p ) n += p->getBufferCount(); }
   return n;
}

uint64_t
SceneManager::getVertexCount( int mode ) const
{
   uint64_t n = 0;
   for ( ISceneNode const * const p : m_TopLevel ) { if ( p ) n += p->getVertexCount(); }
   return n;
}

uint64_t
SceneManager::getIndexCount( int mode ) const
{
   uint64_t n = 0;
   for ( ISceneNode const * const p : m_TopLevel ) { if ( p ) n += p->getIndexCount(); }
   return n;
}

uint64_t
SceneManager::getByteCount( int mode ) const
{
   uint64_t n = sizeof( *this );
   for ( ISceneNode const * const p : m_TopLevel ) { if ( p ) n += p->getByteCount(); }
   return n;
}

uint32_t
SceneManager::getMeshCount() const
{
   return m_MeshLut.size();
}

bool
SceneManager::hasMesh( std::string const & name ) const
{
   auto it = m_MeshLut.find( name );
   if (it == m_MeshLut.end()) { return false; }
   return true;
}

void
SceneManager::removeMesh( std::string const & name )
{
   auto it = m_MeshLut.find( name );
   if (it != m_MeshLut.end()) {
      m_MeshLut.erase( it );
   }
}

SMesh&
SceneManager::addEmptyMesh( std::string name )
{
   if ( name.empty() )
   {
      throw std::runtime_error("No mesh with empty name allowed");
   }

   auto it = m_MeshLut.find( name );

   // Mesh already exist
   if (it != m_MeshLut.end())
   {
      DE_WARN("Mesh already exist ", name)
   }

   // Adding Empty mesh
   m_MeshLut[ name ] = SMesh( name );
   return m_MeshLut[ name ];
}

SMesh&
SceneManager::getMesh( std::string const & uri )
{
   if ( uri.empty() )
   {
      DE_RUNTIME_ERROR(uri,"No empty mesh name allowed")
   }

   auto it = m_MeshLut.find( uri );
   if (it != m_MeshLut.end())
   {
      return it->second;  // Cache hit
   }

   // Heavy load work
   if ( !dbExistFile( uri ) )
   {
      DE_RUNTIME_ERROR(uri,"No such mesh file exist")
   }

   // Easy work
   m_MeshLut[ uri ] = SMesh( uri );
   SMesh& mesh = m_MeshLut[ uri ];

   // Heavy work load
   if ( SMeshIO::load( mesh, uri, m_driver ) )
   {
      DE_DEBUG("Loaded 3d model file ", uri)
      if (mesh.getName().empty())
      {
         DE_ERROR("MeshLoader produced empty name, uri ",uri)
         mesh.setName(uri);
      }
   }
   else
   {
      mesh.setName(uri);
      DE_ERROR("Cant load mesh file ", uri)
   }

   return m_MeshLut[ uri ];
}

bool
SceneManager::addMesh( SMesh mesh )
{
//   if ( mesh->Name.empty() )
//   {
//      DE_WARN("No name")
//      return false;
//   }

//   auto it = m_MeshLut.find( mesh->Name );
//   if (it != m_MeshLut.end())
//   {
//      DE_ERROR("Mesh already exist, ",mesh->Name)
//      return false;
//   }

//   auto it = std::find_if( m_Meshes.begin(), m_Meshes.end(),
//      [&] ( SMesh const * const cached ) { return cached == mesh; } );
//   if ( it != m_Meshes.end() )
//   {
//      DE_ERROR("Mesh ",mesh->Name, " already added.")
//      return false;
//   }

//   DE_DEBUG("+ Added mesh ",mesh->Name)
////   m_MeshLut[ mesh->Name ] = mesh;
//   m_Meshes.emplace_back( mesh );
   return true;
}


void
SceneManager::init( int w, int h, int logLevel )
{

}

void
SceneManager::clearTrashBin()
{
   for ( size_t i = 0; i < m_TrashBin.size(); ++i )
   {
      SAFE_DELETE( m_TrashBin[ i ] );
   }
   m_TrashBin.clear();
}

void
SceneManager::clear()
{
   m_RootNode = m_Hovered = m_Picked = nullptr;

   for ( size_t i = 0; i < m_TopLevel.size(); ++i )
   {
      ISceneNode* trash = m_TopLevel[ i ];
      if ( trash )
      {
         auto it = std::find_if( m_TrashBin.begin(), m_TrashBin.end(),
            [&] ( ISceneNode const * const cached ) { return cached == trash; } );
         if ( it == m_TrashBin.end() )
         {
            m_TrashBin.emplace_back( trash );
         }
      }
   }

   m_TopLevel.clear();
   //clearCameras();
   clearTrashBin();
   // CLEAR MESHCACHE
   m_MeshLut.clear();
}


void
SceneManager::resize( int w, int h )
{
   // m_camera0.setScreenSize( w, h );

   if (m_camera)
   {
       m_camera->setScreenSize( w, h );
   }

   // for ( ICamera* camera : m_Cameras )
   // {
      // if ( camera )
      // {
         // camera->setScreenSize( w, h );
      // }
   // }
}

void
SceneManager::render()
{
   // [OncePerFrame] Update ressources
   clearTrashBin();

   // [Animate] SceneObjects
   auto timeNow = m_driver->getTimer();

   for ( size_t i = 0; i < m_TopLevel.size(); ++i )
   {
      ISceneNode* node = m_TopLevel[ i ];
      if ( node ) { node->animate( timeNow ); }
   }

   // [Pick] Scene
   pickSceneNode();

   // [Draw] Scene
   for ( size_t i = 0; i < m_TopLevel.size(); ++i )
   {
      ISceneNode* node = m_TopLevel[ i ];
      if ( !node ) continue;
      // SceneNode knows its visibility. Not checked here for being visible.
      node->render();
   }
}

void
SceneManager::setDebugData( VisualDebugData const & debugData )
{
   for ( size_t i = 0; i < m_TopLevel.size(); ++i )
   {
      ISceneNode * node = m_TopLevel[ i ];
      if ( node )
      {
         node->setDebugData( debugData );
      }
   }
}

ISceneNode*
SceneManager::getSceneNode( std::string const & name )
{
   if ( name.empty() ) { DE_ERROR("No name") return nullptr; }

   for ( ISceneNode* node : m_TopLevel )
   {
      if ( node && node->getName() == name )
      {
         return node;
      }
   }

   return nullptr;
}

void
SceneManager::addSceneNode( ISceneNode* node )
{
   if ( !node ) { DE_ERROR("No node") return; }

   // Search pointer in cache
   auto it = std::find_if( m_TopLevel.begin(), m_TopLevel.end(),
      [&] ( ISceneNode* cached ) { return cached == node; } );

   // Add only if pointer is unique
   if ( it == m_TopLevel.end() )
   {
      DE_ERROR("Add SceneNode ", m_TopLevel.size())
      m_TopLevel.emplace_back( node );
   }
   else
   {
      DE_WARN("SceneNode already added(", node, ")")
   }
}

SMeshSceneNode*
SceneManager::createMeshSceneNode( std::string uri, ISceneNode* parent, int id )
{
   SMesh & mesh = getMesh( uri );

   return createMeshSceneNode( mesh, parent, id );
}

SMeshSceneNode*
SceneManager::createMeshSceneNode( SMesh const & mesh, ISceneNode* parent, int id )
{
   if ( !parent ) { parent = getRootSceneNode(); }
   SMeshSceneNode* node = new SMeshSceneNode( mesh, this, parent, id );
   if ( !node )
   {
      DE_ERROR("No node")
      return nullptr;
   }
   m_TopLevel.push_back( node );
   return node;
}

void
SceneManager::removeSceneNode( ISceneNode* node )
{
   if ( !node ) { DE_ERROR("No node") return; }

   // Search pointer in cache
   auto it = std::find_if( m_TopLevel.begin(), m_TopLevel.end(),
      [&] ( ISceneNode* cached ) { return cached == node; } );

   // Add only if pointer is unique
   if ( it != m_TopLevel.end() )
   {
      ISceneNode* ptr = *it;
      if ( ptr )
      {
         DE_DEBUG("Mark SceneNode as obsolete(", ptr, ")")
         ptr->setVisible( false );  // Dont render anymore
         ptr->setParent( nullptr ); // Detach from tree
         m_TrashBin.emplace_back( ptr );
      }
      m_TopLevel.erase( it );
   }
}


// ###########################
// ###     MeshManager     ###
// ###########################
std::unordered_map< std::string, SMesh > const &
SceneManager::getMeshCache() const
{
   return m_MeshLut;
}

// ================
// Collision control
// ================
ISceneNode*
SceneManager::pickSceneNode()
{
   auto camera = getCamera();
   if ( !camera )
   {
      DE_ERROR("No camera for picking")
      return nullptr;
   }

   int msaa = std::max(m_driver->getMSAA(),1);
   int mx = m_driver->getMouseX() * msaa;
   int my = m_driver->getMouseY() * msaa;
   Ray3< T > ray = camera->computeRay( mx, my );
   gpu::MeshIntersectionResult< T > result;
   ISceneNode* hovered = intersectRay( ray, result );
   if ( hovered != m_Hovered )
   {
      // emit enterSceneNode();
      //DE_DEBUG("Changed pick from(",(void*)m_Picked,") to(",(void*)picked,")")
   }
   m_Hovered = hovered;
   m_Picked = hovered;
   return hovered;
}

// ================
// Collision control
// ================
ISceneNode*
SceneManager::intersectRay( de::Ray3< T > const & ray, gpu::MeshIntersectionResult< T > & result )
{
   if ( m_TopLevel.size() < 1 ) return nullptr;

   gpu::MeshIntersectionResult< T > globalResult;

   ISceneNode* pickNode = nullptr;
//   glm::tvec3< T > pickPos;
//   uint32_t pickTriangleIndex = 0;
//   uint32_t pickNodeIndex = 0;
   T const distMax = std::numeric_limits< T >::max();
//   T pickDist = distMax;

   globalResult.distance = distMax;

   bool doesIntersect = false;

   for ( size_t i = 0; i < m_TopLevel.size(); ++i )
   {
      ISceneNode* node = m_TopLevel[ i ];
      if ( !node ) { continue; }
      //if ( !node->isCollisionEnabled() ) { continue; }

//      glm::tvec3< T > tmpPos;
//      uint32_t tmpBufferIndex = 0;
//      uint32_t tmpTriangleIndex = 0;

      gpu::MeshIntersectionResult< T > localResult;
      if ( node->intersectRay( ray, localResult ) ) // &tmpPos, &tmpBufferIndex
      {
         T localDist = glm::length2( ray.getPos() - localResult.position );

         if ( globalResult.distance == distMax || globalResult.distance >= localDist )
         {
            globalResult.distance = localDist;
            globalResult.nodeIndex = int32_t( i );
            globalResult.bufferIndex = localResult.bufferIndex;
            globalResult.meshIndex = localResult.meshIndex;
            globalResult.triangleIndex = localResult.triangleIndex;
            globalResult.triangle = localResult.triangle;
            globalResult.position = localResult.position;
            pickNode = node;
         }

         doesIntersect = true;
      }
   }

   if ( doesIntersect )
   {
      globalResult.distance = dbSqrt( globalResult.distance );
      result = globalResult;
      //DE_DEBUG("pickNode", pickNode ,".Triangle[", pickTriangleIndex, "] "
      //         "pickPos(", pickPos, "), name(", pickNode->getName(), ")" )
   }


   return pickNode;
}


} // end namespace scene.
} // end namespace de.

/*

ISceneNode*
SceneManager::pickSceneNode( Ray3< T > const & ray, ScenePickResult< T >* pickResult )
{
   ISceneNode* pickNode = nullptr;
   if ( m_TopLevel.size() < 1 ) return pickNode;
   glm::tvec3< T > pickPos;
   uint32_t pickTriangleIndex = 0;
   uint32_t pickNodeIndex = 0;
   T const distMax = std::numeric_limits< T >::max();
   T pickDist = distMax;
   bool doesIntersect = false;

   for ( size_t i = 0; i < m_TopLevel.size(); ++i )
   {
      ISceneNode* node = m_TopLevel[ i ];
      if ( !node ) { continue; }
      if ( !node->isCollisionEnabled() ) { continue; }

//      glm::tvec3< T > tmpPos;
//      uint32_t tmpBufferIndex = 0;
//      uint32_t tmpTriangleIndex = 0;

      gpu::MeshIntersectionResult< T > result;
      if ( node->intersectRayWithBoundingBox( ray, result ) ) // &tmpPos, &tmpBufferIndex
      {
         T d = glm::length( ray.getPos() - result.position );

         //DE_DEBUG("Node[",i,"](", node->getName(),").Triangle[",t,"].Hit(", tmpPosition,"), "
         //   "d(", d, "), dist(", dist, "), tri(", tri.toString(), ")")

         if ( pickDist == distMax || pickDist >= d )
         {
            pickDist = d;
            pickNode = node;
            pickNodeIndex = i;
            pickPos = result.position;
            pickTriangleIndex = result.triangleIndex;
         }

         doesIntersect = true;
      }
   }

   if ( pickResult )
   {
      pickResult->doesIntersect = doesIntersect;
      pickResult->pos = pickPos;
      pickResult->dist = pickDist;
      pickResult->node = pickNode;
      pickResult->nodeIndex = pickNodeIndex;
      pickResult->triangleIndex = pickTriangleIndex;
   }

   if ( doesIntersect )
   {
      //DE_DEBUG("pickNode", pickNode ,".Triangle[", pickTriangleIndex, "] "
      //         "pickPos(", pickPos, "), name(", pickNode->getName(), ")" )
   }

   return pickNode;

}
// =====================
// ### CameraManager ###
// =====================
// Camera* m_Camera;
// std::vector< Camera* > m_Cameras;
// void clearCameras();
// int32_t getCameraCount() const;
// int32_t findCamera( Camera* camera ) const;
// int32_t findCamera( std::string const & name ) const;
// Camera* getCamera( int32_t index ) const;
// Camera* getCamera( std::string const & name ) const;
// Camera* addCamera( std::string const & name, bool makeActive = true );
// void addCamera( Camera* camera, bool makeActive = true );
// Camera* getActiveCamera();
// Camera const* getActiveCamera() const;
// bool setActiveCamera( Camera* camera );


Camera*
SceneManager::addCamera( std::string const & name, bool makeActive )
{
   Camera* camera = new Camera();
   camera->setName( name );
   addCamera( camera, makeActive );
   return camera;
}

void
SceneManager::addCamera( Camera* camera, bool makeActive )
{
   if ( !camera ) return;
   int32_t found = findCamera( camera );
   if ( found > -1 ) { return; } // Already cached

   m_Cameras.emplace_back( camera );
   if ( makeActive )
   {
      setActiveCamera( camera );
   }
}

void
SceneManager::clearCameras()
{
   m_Camera = nullptr;
   for ( Camera* camera : m_Cameras )
   {
      if ( camera ) delete camera;
   }
   m_Cameras.clear();
}

int32_t
SceneManager::getCameraCount() const
{
   return m_Cameras.size();
}

int32_t
SceneManager::findCamera( Camera* camera ) const
{
   if ( !camera ) return -1;
   for ( size_t i = 0; i < m_Cameras.size(); ++i )
   {
      Camera const * const cached = m_Cameras[ i ];
      if ( cached == camera )
      {
         return int32_t( i );
      }
   }
   return -1;
}

int32_t
SceneManager::findCamera( std::string const & name ) const
{
   if ( name.empty() ) return -1;
   for ( size_t i = 0; i < m_Cameras.size(); ++i )
   {
      Camera const * const cached = m_Cameras[ i ];
      if ( cached && cached->getName() == name )
      {
         return int32_t( i );
      }
   }
   return -1;
}

Camera*
SceneManager::getCamera( int32_t index ) const
{
   return m_Cameras[ index ];
}

Camera*
SceneManager::getCamera( std::string const & name ) const
{
   int32_t found = findCamera( name );
   if ( found < 0 ) return nullptr;
   return m_Cameras[ found ];
}



// Camera* getActiveCamera();
// Camera const* getActiveCamera() const;
// bool setActiveCamera( Camera* camera );
Camera const*
SceneManager::getActiveCamera() const { return m_Camera; }

Camera*
SceneManager::getActiveCamera() { return m_Camera; }

bool
SceneManager::setActiveCamera( Camera* camera )
{
   m_Camera = camera;
   if ( m_Camera )
   {
      m_Camera->setScreenSize( m_driver->getScreenWidth(),
                               m_driver->getScreenHeight() );
      m_Camera->update();
   }
   return true;
}

ISceneNode*
SceneManager::pickSceneNode( int mx, int my )
{
   glm::vec3 rayStart;
   glm::vec3 rayEnd;

   if ( getCamera() )
   {
      getCamera()->update();
      int w = getCamera()->getWidth();
      int h = getCamera()->getHeight();
      //auto cameraPos = getCamera()->getPos();
      glm::dmat4 viewProj = getCamera()->getViewProjectionMatrix();
      glm::dmat4 viewProjInv = glm::inverse( viewProj );

      // DE_DEBUG("Camera :: ",getCamera()->toString())

      auto ndc_x = 2.0 * ( double( mx ) / double( w ) ) - 1.0;
      auto ndc_y = 2.0 * ( double( my ) / double( h ) ) - 1.0;
      auto drayStart = glm::dvec4( ndc_x, ndc_y, -1.0, 1.0 );
      auto drayEnd = glm::dvec4( ndc_x, ndc_y, 1.0, 1.0 );

//      GLM_FUNC_QUALIFIER vec<3, T, Q>
//      unProject(  vec<3, T, Q> const& win,
//                  mat<4, 4, T, Q> const& model,
//                  mat<4, 4, T, Q> const& proj,
//                  vec<4, U, Q> const& viewport );

      // glm::unProject( glm::dvec3( ) )

      // CameraMouse Ray
      rayStart = glm::dvec3( viewProjInv * drayStart );
      rayEnd = glm::dvec3( viewProjInv * drayEnd );
   }

   return pickSceneNode( rayStart, rayEnd - rayStart );
}

*/

