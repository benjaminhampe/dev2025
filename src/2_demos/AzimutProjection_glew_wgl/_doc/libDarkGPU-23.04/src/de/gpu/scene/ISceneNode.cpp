#include <de/scene/ISceneNode.hpp>

namespace de {
namespace scene {

// =======================================================================
VisualDebugData::VisualDebugData( uint32_t visualFlags )
// =======================================================================
   : flags( visualFlags )
   , normalLength( 1.0f )
   , normalColor( 0x2F0000FF )
   , bboxColor( 0x6F00DDEF )
   , bboxesColor( 0x7F30BBCC )
   , wireframeScale( 1.001f )
   , wireframeColor( 0x406AAADA )
{ }

void
VisualDebugData::toggle( EDebugFlag flag )
{
   uint32_t uflag = flag;
   if ( flags & uflag ) { flags &= ~uflag; } // ... disable bit flag,
   else                 { flags |= uflag; }  // ... enable bit flag
}

bool
VisualDebugData::operator== ( VisualDebugData const & other ) const
{
   if ( flags != other.flags ) return false;
   if ( normalLength != other.normalLength ) return false;
   if ( normalColor != other.normalColor ) return false;
   if ( bboxColor != other.bboxColor ) return false;
   if ( bboxesColor != other.bboxesColor ) return false;
   if ( wireframeScale != other.wireframeScale ) return false;
   if ( wireframeColor != other.wireframeColor ) return false;
   return true;
}

// =======================================================================
ISceneNode::ISceneNode( ISceneManager* smgr, ISceneNode* parent, int id )
// =======================================================================
   : m_SceneManager( smgr )
   , m_Parent( parent )
   , m_Id( id )
   , m_IsVisible( true )
   , m_IsDirty( true )
   , m_IsCollisionEnabled( true )
   , m_AbsoluteTransform(1)   // default ctr identity matrix.
   , m_RelativeTranslation(0,0,0)
   , m_RelativeRotation(0,0,0)
   , m_RelativeScale(1,1,1)
   , m_Children()
{
//   if ( m_SceneManager )
//   {
//      m_SceneManager->addSceneNode( this );
//   }

   if ( m_Parent )
   {
      m_Parent->addChild( this );
   }

   updateAbsoluteTransform();
}

ISceneNode::~ISceneNode()
{
   removeAll();
   // delete all animators
   // m_TriangleSelector->drop();
}

//std::string ISceneNode::toString() const
//{
//   std::stringstream s; s << getName();
//   return s.str();
//}
//void ISceneNode::removeFromParent();

bool
ISceneNode::isTrulyVisible() const
{
   if ( !m_IsVisible ) return false;
   if ( !m_Parent ) return true;
   return m_Parent->isTrulyVisible();
}

// Changes the parent of the scene node.
void
ISceneNode::setParent( ISceneNode* parent )
{
   m_Parent = parent;

//      for ( auto & child : m_Children )
//      {
//         if ( child )
//         {
//            child->setParent( parent )
//         }
//      }
}

Box3f
ISceneNode::getAbsoluteBoundingBox() const
{
   return getBoundingBox().transformBox( m_AbsoluteTransform );
}

// Gets the absolute position of the node in world coordinates.
ISceneNode::V3
ISceneNode::getAbsolutePosition() const
{
   T const * const m = glm::value_ptr( m_AbsoluteTransform );
   T x = m[ 12 ];
   T y = m[ 13 ];
   T z = m[ 14 ];
   //DE_DEBUG("AbsolutePosition = (",x,",",y,",",z,")")
   return { x, y, z };
}

void
ISceneNode::updateAbsoluteTransform()
{
   if ( !m_IsDirty ) return;

   if ( m_Parent )
   {
      m_AbsoluteTransform = m_Parent->getAbsoluteTransform() * getRelativeTransform();
   }
   else
   {
      m_AbsoluteTransform = getRelativeTransform();
   }
   m_IsDirty = false;
}

// Returns the relative transformation of the scene node.
ISceneNode::M4
ISceneNode::getRelativeTransform() const
{
   M4 t = glm::translate( M4(1), m_RelativeTranslation );
   M4 rx = glm::rotate( glm::dmat4( 1.0 ), glm::radians( m_RelativeRotation.x ), { 1., 0., 0. } );
   M4 ry = glm::rotate( glm::dmat4( 1.0 ), glm::radians( m_RelativeRotation.y ), { 0., 1., 0. } );
   M4 rz = glm::rotate( glm::dmat4( 1.0 ), glm::radians( m_RelativeRotation.z ), { 0., 0., 1. } );
   M4 r = rz * ry * rx;
   // M4 r = Quat< T >::fromEulerAngles( m_RelativeRotation ).toMat4();
   M4 s = glm::scale( M4(1), m_RelativeScale );
   M4 mat = t * r * s;
   // DE_DEBUG( getName(), ", r(",r,")")
   // DE_DEBUG( getName(), ", t(",t,")")
   // DE_DEBUG( getName(), ", s(",s,")")
   // DE_DEBUG( getName(), ", mat(",mat,")")
   return mat;
}

// Sets the relative scale of the scene node.
void
ISceneNode::setScale( V3 const & scale )
{
   m_RelativeScale = scale;
   m_IsDirty = true;
   updateAbsoluteTransform();
}
// Sets the rotation of the node relative to its parent in degrees.
void
ISceneNode::setRotation( V3 const & rotation )
{
   m_RelativeRotation = rotation;
   m_IsDirty = true;
   updateAbsoluteTransform();
}
// Sets the position of the node relative to its parent.
void
ISceneNode::setPosition( V3 const & pos )
{
   m_RelativeTranslation = pos;
   m_IsDirty = true;
   updateAbsoluteTransform();
}


// Removes all children of this scene node
void
ISceneNode::removeAll()
{
   for (ISceneNode* child : m_Children)
   {
      if ( child )
      {
         child->setParent( nullptr );
         //delete node;
         //node->drop();
      }
   }
   m_Children.clear();
}

// Removes a child from this scene node.
bool
ISceneNode::removeChild( ISceneNode* child )
{
   if ( !child ) return false;
   for ( size_t i = 0; i < m_Children.size(); ++i )
   {
      if ( m_Children[ i ] == child )
      {
         m_Children[ i ]->m_Parent = nullptr;
         m_Children.erase( m_Children.begin() + i );
         return true;
      }
   }
   return false;
}


// Adds a child to this scene node.
void
ISceneNode::addChild( ISceneNode* child )
{
   if ( !child || ( child == this ) ) return;
   child->setParent( nullptr ); // remove from old parent
   m_Children.push_back( child );
   child->setParent( this );
}

//   std::vector< Triangle3f > const & getCollisionTriangles() const { return m_CollisionTriangles; }
//   void setCollisionTriangles( std::vector< Triangle3f > const & triangles ) { m_CollisionTriangles = triangles; }
//   void createCollisionTriangles() {}

} // end namespace scene.
} // end namespace de.
