#pragma once
#include <de/gpu/smesh/SMesh.hpp>

namespace de {

namespace gpu
{
   struct VideoDriver;
}

namespace scene {

class ISceneManager;

// =======================================================================
struct VisualDebugData
// =======================================================================
{
   enum EDebugFlag
   {
      NONE = 0, BBOX = 1, BBOXES = 2, NORMALS = 4,
      WIREFRAME = 16, TANGENTS = 32, BITANGENTS = 64,
      ALL = BBOX | BBOXES | NORMALS | WIREFRAME,
      EDebugFlagCount = 7
   };

   uint32_t flags;
   float normalLength;
   uint32_t normalColor;
   uint32_t bboxColor;
   uint32_t bboxesColor;
   float wireframeScale;
   uint32_t wireframeColor;

   VisualDebugData( uint32_t visualFlags = NONE );
   void toggle( EDebugFlag flag );
   bool operator== ( VisualDebugData const & other ) const;
};

// =======================================================================
struct SceneNodeType
// =======================================================================
{
   enum EType { User = 0, Mesh, Camera, SkyBox, Cube, Terrain, Plane, Water, ETypeCount };
};

// =======================================================================
struct ISceneNode
// =======================================================================
{
   typedef double T;
   typedef glm::tmat4x4< T > M4;
   typedef glm::tvec2< T > V2;
   typedef glm::tvec3< T > V3;
   typedef glm::tvec4< T > V4;
   typedef TRS< T > Transform;
   typedef std::unique_ptr< ISceneNode > UniquePtr;
   typedef std::shared_ptr< ISceneNode > SharedPtr;
   typedef std::weak_ptr< ISceneNode > WeakPtr;

public:

   ISceneNode( ISceneManager* smgr, ISceneNode* parent, int id = -1 );
   virtual              ~ISceneNode();
   virtual void         render() = 0;
   virtual void         animate( double driverTimeInSec )  {}

   virtual ISceneManager* getSceneManager() { return m_SceneManager; }

   virtual ISceneNode*  getParent() const { return m_Parent; }
   virtual void         setParent( ISceneNode* parent );   // Changes the parent of the scene node.

   virtual std::string  getName() const { return m_Name; }
   virtual void         setName( std::string const & name ) { m_Name = name; }

   // Returns whether the node should be visible (if all of its parents are visible).
   virtual bool         isVisible() const { return m_IsVisible; }
   // Check whether the node is truly visible, taking into accounts its parents' visibility
   virtual bool         isTrulyVisible() const;
   // Sets if the node should be visible or not.
   virtual void         setVisible( bool isVisible ) { m_IsVisible = isVisible; }
   // Get the id of the scene node.
   virtual int          getId() const { return m_Id; }
   // Sets the id of the scene node.
   virtual void         setId( int id ) { m_Id = id; }

   virtual int32_t      getUserData1() const { return m_userData1; }
   virtual void         setUserData1( int32_t typeId ) { m_userData1 = typeId; }

   virtual uint32_t     getUserData2() const { return m_userData2; }
   virtual void         setUserData2( uint32_t pickcolor ) { m_userData2 = pickcolor; }

   virtual Box3f        getBoundingBox() const = 0;
   virtual Box3f        getAbsoluteBoundingBox() const;

   virtual bool         isCollisionEnabled() const { return m_IsCollisionEnabled; }
   virtual void         setCollisionEnabled( bool enable ) { m_IsCollisionEnabled = enable; }

   virtual VisualDebugData const & getDebugData() const { return m_DebugData; }
   virtual VisualDebugData & getDebugData() { return m_DebugData; }
   virtual void         setDebugData( VisualDebugData const & debugData ) { m_DebugData = debugData; }

//   virtual SceneNodeType::EType getNodeType() const { return SceneNodeType::User; }
//   virtual std::vector< ISceneNode* > const & getChildren() const { return m_Children; }

   //virtual std::string  toString() const;
   //virtual void       removeFromParent();

   virtual uint32_t     getBufferCount() const { return 0; }
   virtual uint64_t     getVertexCount() const { return 0; }
   virtual uint64_t     getIndexCount() const { return 0; }
   virtual uint64_t     getByteCount() const { return 0; }

   virtual M4 const &   getAbsoluteTransform() const { return m_AbsoluteTransform; }
   // Gets the absolute position of the node in world coordinates.
   virtual V3           getAbsolutePosition() const;

   // Gets the scale of the scene node relative to its parent.
   virtual V3 const &   getScale() const { return m_RelativeScale; }
   // Gets the rotation of the node relative to its parent in degrees.
   virtual V3 const &   getRotation() const { return m_RelativeRotation; }
   // Gets the position of the node relative to its parent.
   virtual V3 const &   getPosition() const { return m_RelativeTranslation; }
   // Sets the relative scale of the scene node.
   virtual void         setScale( V3 const & scale );
   // Sets the rotation of the node relative to its parent in degrees.
   virtual void         setRotation( V3 const & rotation );
   // Sets the position of the node relative to its parent.
   virtual void         setPosition( V3 const & pos );
   // Sets the relative scale of the scene node.
   virtual void         setScale( T x, T y, T z ) { setScale( V3(x,y,z) ); }
   // Sets the relative rotation of the scene node in degrees.
   virtual void         setRotation( T x, T y, T z ) { setRotation( V3(x,y,z) ); }
   // Sets the relative position of the scene node.
   virtual void         setPosition( T x, T y, T z ) { setPosition( V3(x,y,z) ); }
   // Updates the absolute position based on the relative and the parents position
   virtual void         updateAbsoluteTransform();
   // Returns the relative transformation of the scene node.
   virtual M4           getRelativeTransform() const;
   // Removes all children of this scene node
   virtual void         removeAll();
   // Removes a child from this scene node.
   virtual bool         removeChild( ISceneNode* child );
   // Adds a child to this scene node.
   void                 addChild( ISceneNode* child );
   //
   virtual bool
   intersectRayWithBoundingBox( Ray3< T > const & ray, gpu::MeshIntersectionResult<T> & result ) const
   {
      return false;
   }
   virtual bool
   intersectRay( Ray3< T > const & ray, gpu::MeshIntersectionResult<T> & result ) const
   {
      return false;
   }
   //std::vector< Triangle3f > const & getCollisionTriangles() const { return m_CollisionTriangles; }
   //void               setCollisionTriangles( std::vector< Triangle3f > const & triangles ) { m_CollisionTriangles = triangles; }
   //void               createCollisionTriangles() {}

public:

   DE_CREATE_LOGGER("de.scene.ISceneNode")
   ISceneManager* m_SceneManager;
   ISceneNode* m_Parent;             //! Pointer to the parent
   int m_Id;               //! ID of the node.
   int32_t  m_userData1;         //! UserData 1, maybe a type to cast ISceneNode to specific user class
   uint32_t m_userData2;   //! UserData 2, maybe a pickcolor
   bool m_IsVisible;         //! Is the node visible?
   bool m_IsDirty;        //! Is dirty matrix
   bool m_IsCollisionEnabled;     //! Is enabled for interactivity
   //bool m_IsDebugObject; //! Is debug object?
   M4 m_AbsoluteTransform;   // Absolute TRS transformation of the node.
   V3 m_RelativeTranslation; // Relative Translation of the scene node.
   V3 m_RelativeRotation;    // Relative Rotation (euler angles) of the scene node.
   V3 m_RelativeScale;       // Relative Scale of the scene node.
   //AnimTimer m_AnimTime;
   std::vector< ISceneNode* > m_Children;   //! List of all children of this node
   VisualDebugData m_DebugData;
   std::string m_Name;
   //std::vector< Triangle3f > m_CollisionTriangles;
};

} // end namespace scene.
} // end namespace de.
