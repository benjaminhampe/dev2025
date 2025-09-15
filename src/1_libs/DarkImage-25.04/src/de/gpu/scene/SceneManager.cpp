#include <de/gpu/scene/SceneManager.h>
#include <de/gpu/VideoDriver.h>
#include <de/gpu/smesh/SMeshLibrary.h>

namespace de {
namespace gpu {

// =====================================================================
ISceneNode::ISceneNode( SceneManager* env, ISceneNode* parent, u32 id, float x, float y, float z )
    // =====================================================================
    : m_visible(true)
    , m_pickable(true)
    , m_enabled(true)
    , m_id(id)
    , m_trs()
    , m_env(env)
    , m_parent(parent)
{
    m_trs.pos = glm::vec3(x,y,z);
    m_trs.update();

    if (!m_env)
    {
        std::ostringstream o;
        o << "No SceneManager given to SceneNode id " << id;
        throw std::runtime_error(o.str());
    }

    if (m_parent)
    {
        m_parent->addChild( this );
    }
    else
    {
        m_env->addTopLevelNode( this );
    }
}

ISceneNode::~ISceneNode()
{
    clearChildren();
}

void ISceneNode::draw()
{
    if (!isVisible()) { return; } // DE_WARN("Invisible, nothing todo.")

    for (auto & child : m_children)
    {
        child->draw();
    }
}

// const TRSf& getTRS() const = 0;
// void setTRS( const TRSf& trs ) = 0;
// M4 getAbsoluteTransform() = 0;
// Box3f getAbsoluteBoundingBox() = 0;

const TRSf&
ISceneNode::getTRS() const { return m_trs; }

void
ISceneNode::setTRS( const TRSf& trs ) { m_trs = trs; }

TRSf
ISceneNode::getAbsoluteTRS()
{
    m_trs.update();
    if (m_parent)
    {
        return m_parent->getAbsoluteTRS().transform(m_trs);
    }
    else
    {
        return m_trs;
    }
}

Box3f
ISceneNode::getAbsoluteBoundingBox()
{
    return m_bbox;
}

ISceneNode::M4
ISceneNode::getAbsoluteTransform()
{
    //m_trs.update();
    if (m_parent)
    {
        return m_parent->getAbsoluteTransform() * m_trs.modelMat;
    }
    else
    {
        return m_trs.modelMat;
    }
}

ISceneNode::V3
ISceneNode::getAbsolutePos() const
{
    return {};
    //return getAbsoluteTransform()
}

void ISceneNode::setParent( ISceneNode* parent ) { m_parent = parent; }
ISceneNode* ISceneNode::getParent() { return m_parent; }

void ISceneNode::setName( const std::string & name ) { m_name = name; }
const std::string & ISceneNode::getName() const { return m_name; }

void ISceneNode::setId(u32 id) { m_id = id; }
u32 ISceneNode::getId() const { return m_id; }

void ISceneNode::setVisible( bool visible ) { m_visible = visible; }
bool ISceneNode::isVisible() const { return m_visible; }

//void setEnabled( bool enabled ) = 0;

//bool isHovered() const = 0;
//bool isPickable() const = 0;
//void setPickable( bool pickable ) = 0;

// void onEvent( const Event& event ) = 0;
/*
    void resizeEvent( const ResizeEvent& event ) = 0;
    void paintEvent( const PaintEvent& event ) = 0;
    void timerEvent( const TimerEvent& event ) = 0;
    void showEvent( const ShowEvent& event ) = 0;
    void hideEvent( const HideEvent& event ) = 0;
    void enterEvent( const EnterEvent& event ) = 0;
    void leaveEvent( const LeaveEvent& event ) = 0;
    void focusEvent( const FocusEvent& event ) = 0;
    //void focusInEvent( FocusEvent event ) = 0;
    //void focusOutEvent( FocusEvent event ) = 0;
    //void keyEvent( const KeyEvent & event ) = 0;
    void keyPressEvent( const KeyPressEvent& event ) = 0;
    void keyReleaseEvent( const KeyReleaseEvent& event ) = 0;
    //void mouseButtonEvent( const MouseButtonEvent& event ) = 0;
    void mousePressEvent( const MousePressEvent& event ) = 0;
    void mouseReleaseEvent( const MouseReleaseEvent& event ) = 0;
    void mouseMoveEvent( const MouseMoveEvent& event ) = 0;
    void mouseWheelEvent( const MouseWheelEvent& event ) = 0;
    void joystickEvent( const JoystickEvent& event ) = 0;
*/


void ISceneNode::clearChildren()
{
    for (auto & child : m_children)
    {
        delete child;
    }
    m_children.clear();
}
void ISceneNode::addChild( ISceneNode* child )
{
    if (!child) return; // Nothing todo;

    auto it = std::find_if( m_children.begin(), m_children.end()
                           , [=](const auto cached) { return cached == child; });
    if (it != m_children.end())
    {
        DE_WARN("Child already added ", child->getName())
        return;
    }

    m_children.push_back(child);
}

ISceneNode* ISceneNode::pickChild( int mx, int my )
{
    auto camera = m_env->getVideoDriver()->getCamera();
    Ray3f ray = camera->computeRayf( mx, my );
    return pickChild( ray );
}

ISceneNode* ISceneNode::pickChild( Ray3f ray )
{
    ISceneNode* hoverElement = nullptr;

    // V3 hitPosition;
    // if ( getAbsoluteBoundingBox().intersectRay(ray.getPos(), ray.getDir(), hitPosition ) )
    // {
    //     hoverElement = this;
    // }

    for (ISceneNode* child : m_children)
    {
        if ( !child->isVisible() ) continue;

        V3 hitPosition;
        if ( child->getAbsoluteBoundingBox().intersectRay(ray.getPos(), ray.getDir(), hitPosition ) )
        {
            hoverElement = child->pickChild( ray ); // recursion!
            if (!hoverElement) // if (child->isPickable()) // We like to exclude panels.
            {
                hoverElement = child;
            }
            break;
        }
    }

    return hoverElement;
}

// =====================================================================
// =====================================================================
// =====================================================================


SMeshSceneNode::SMeshSceneNode( SceneManager* env, ISceneNode* parent, u32 id, f32 x, f32 y, f32 z )
    : ISceneNode(env,parent,id,x,y,z)
{}
SMeshSceneNode::~SMeshSceneNode()
{}

void SMeshSceneNode::draw()
{
    if (!isVisible()) { return; } // DE_WARN("Invisible, nothing todo.")

    // Draw myself:
    auto driver = m_env->getVideoDriver();
    if (!driver) { DE_ERROR("No driver") return; }
    // int w = driver->getScreenWidth();
    // int h = driver->getScreenHeight();


    auto renderer = driver->getSMaterialRenderer();
    if (renderer)
    {
        m_trs.update();

        // Draw this
        for (auto & limb : m_mesh.buffers)
        {
            limb.upload();

            driver->setModelMatrix( m_trs.modelMat );

            renderer->setMaterial( limb.getMaterial() );

            limb.draw();

            driver->getLine3DRenderer()->draw3DLineBox( limb.boundingBox, randomColorRGB() );
        }

        for (auto & child : m_children)
        {
            child->draw();
        }
    }

}

// =====================================================================
// =====================================================================
// =====================================================================
SceneManager::SceneManager()
    : m_driver( nullptr )
    , m_visible(true)
    , m_hoveredElement( nullptr )
    , m_lastHoveredElement( nullptr )
{
}

SceneManager::~SceneManager()
{
    clearSceneNodes();
}

void SceneManager::clearSceneNodes()
{
    for (auto & node : m_topLevelNodes)
    {
        delete node;
    }
    m_topLevelNodes.clear();
}

void SceneManager::init( VideoDriver* driver )
{
    m_driver = driver;
    // init camera
}

void SceneManager::draw()
{
    if (!isVisible()) return;

    //DE_ERROR("Draw ", m_topLevelNodes.size()," ISceneNodes.")

    for (auto & child : m_topLevelNodes)
    {
        if (!child)
        {
            DE_ERROR("Got nullptr in cache!")
            continue;
        }
        child->draw();
    }
}

void SceneManager::addTopLevelNode( ISceneNode* node )
{
    if (!node) return; // Nothing to add.

	auto& children = m_topLevelNodes;
	
    auto itChild = std::find_if( 
		children.begin(), 
		children.end(), 
		[=](const auto cached) { return cached == node; }
	);
    
	if (itChild != children.end())
    {
        DE_WARN("TopLevelNode already added ", node->getId())
        return;
    }

    children.push_back(node);
}

ISceneNode* SceneManager::pick( int mx, int my )
{
    ISceneNode* hoverElement = nullptr;
/*
    for (auto & node : m_topLevelNodes)
    {
		if ( !node ) continue;
		
        if ( !node->isVisible() ) continue;

        if ( node->getBoundingBox().contains( mx, my ) )
        {
            hoverElement = node->pickChild( mx, my );

            if (!hoverElement)
            {
                hoverElement = node;
            }

            break;
        }
    }
*/
    return hoverElement;
}

/*
void SceneManager::onEvent( const Event & event )
{
    //DE_DEBUG(event.str())
    if (event.type == EventType::MOUSE_PRESS)
    {
        if (m_hoveredElement)
        {
            m_hoveredElement->mousePressEvent(event.mousePressEvent);
        }
    }
    else if (event.type == EventType::MOUSE_RELEASE)
    {
        if (m_hoveredElement)
        {
            m_hoveredElement->mouseReleaseEvent(event.mouseReleaseEvent);
        }
    }
    else if (event.type == EventType::MOUSE_WHEEL)
    {
        if (m_hoveredElement)
        {
            m_hoveredElement->mouseWheelEvent(event.mouseWheelEvent);
        }
    }
    else if (event.type == EventType::MOUSE_MOVE)
    {
        const int mx = event.mouseMoveEvent.x;
        const int my = event.mouseMoveEvent.y;

        m_hoveredElement = pick(mx, my);
        if (m_hoveredElement)
        {
            auto childElement = m_hoveredElement->pickChild(mx,my);
            if (childElement)
            {
                m_hoveredElement = childElement;
            }
        }
        if (m_hoveredElement != m_lastHoveredElement)
        {
            if (m_lastHoveredElement)
            {
                LeaveEvent e;
                e.m_x = mx;
                e.m_y = my;
                m_lastHoveredElement->leaveEvent(e);
            }

            if (m_hoveredElement)
            {
                EnterEvent e;
                e.m_x = mx;
                e.m_y = my;
                m_hoveredElement->enterEvent(e);
            }

            m_lastHoveredElement = m_hoveredElement;
        }
    }
}
*/

} // end namespace gpu.
} // end namespace de.














































/*

ISceneNode::V3
SMeshSceneNode::getAbsolutePos() const
{
    if (m_parent)
    {
        return m_parent->getAbsolutePos() + m_pos;
    }
    else
    {
        return m_pos;
    }
}

void SMeshSceneNode::clearChildren()
{
    for (auto & child : m_children)
    {
        delete child;
    }
    m_children.clear();
}

void SMeshSceneNode::addChild( ISceneNode* child )
{
    if (!child) return; // Nothing todo;

    auto it = std::find_if( m_children.begin(), m_children.end()
                           , [=](const auto cached) { return cached == child; });
    if (it != m_children.end())
    {
        DE_WARN("Child already added ", child->getName())
        return;
    }

    m_children.push_back(child);
}

ISceneNode* SMeshSceneNode::pickChild( int mx, int my )
{
    return nullptr;
}
ISceneNode* SMeshSceneNode::pickChild( Ray3f ray )
{
    return nullptr;
}

ISceneNode* SMeshSceneNode::pickChild( int mx, int my )
{
    ISceneNode* hoverElement = nullptr;

    for (ISceneNode* child : m_children)
    {
        if ( !child->isVisible() ) continue;

        if ( child->getAbsolutePos().contains( mx, my ) )
        {
            hoverElement = child->pickChild( mx, my ); // recursion!
            if (!hoverElement) // if (child->isPickable()) // We like to exclude panels.
            {
                hoverElement = child;
            }
            break;
        }
    }

    return hoverElement;
}

// =====================================================================
SceneNode::SceneNode( SceneManager* env, ISceneNode* parent, u32 id,
                       float x, float y, float z )
    // =====================================================================
    : m_visible(true)
    , m_pickable(true)
    , m_enabled(true)
    , m_id(id)
    , m_pos(x,y,z)
    , m_env(env)
    , m_parent(parent)
{
    if (!m_env)
    {
        std::ostringstream o;
        o << "No SceneManager given to SceneNode id " << id;
        throw std::runtime_error(o.str());
    }

    if (m_parent)
    {
        m_parent->addChild( this );
    }
    else
    {
        m_env->addTopLevelNode( this );
    }
}

SceneNode::~SceneNode()
{
    clearChildren();
}

void SceneNode::draw()
{
    if (!isVisible()) { return; } // DE_WARN("Invisible, nothing todo.")

    // Draw myself:
    auto m_driver = m_env->getVideoDriver();
    if (!m_driver) { DE_ERROR("No driver") return; }
    int w = m_driver->getScreenWidth();
    int h = m_driver->getScreenHeight();

    // Draw others (children):
    for (auto & child : m_children)
    {
        child->draw();
    }
}

ISceneNode*
SceneNode::pickChild( int mx, int my )
{
    ISceneNode* hoverElement = nullptr;

    for (ISceneNode* child : m_children)
    {
        if ( !child->isVisible() ) continue;

        if ( child->getAbsolutePos().contains( mx, my ) )
        {
            hoverElement = child->pickChild( mx, my ); // recursion!
            if (!hoverElement) // if (child->isPickable()) // We like to exclude panels.
            {
                hoverElement = child;
            }
            break;
        }
    }

    return hoverElement;
}

SceneNode::V3
SceneNode::getAbsolutePos() const
{
    if (m_parent)
    {
        return m_parent->getAbsolutePos() + m_pos;
    }
    else
    {
        return m_pos;
    }
}

void
SceneNode::clearChildren()
{
    for (auto & child : m_children)
    {
        delete child;
    }
    m_children.clear();
}

void
SceneNode::addChild( ISceneNode* child )
{
    if (!child) return; // Nothing todo;

    auto it = std::find_if( m_children.begin(), m_children.end()
                           , [=](const auto cached) { return cached == child; });
    if (it != m_children.end())
    {
        DE_WARN("Child already added ", child->getName())
        return;
    }

    m_children.push_back(child);
}

ISceneNode* SceneNode::pickChild( int mx, int my )
{
    return nullptr;
}
ISceneNode* SceneNode::pickChild( Ray3f ray )
{
    return nullptr;
}
*/
