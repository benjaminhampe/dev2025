#include <de/gui/Env.h>
#include <de/gpu/VideoDriver.h>

namespace de {
namespace gui {
    
// =====================================================================
Env::Env()
    : m_driver( nullptr )
    , m_hoveredElement( nullptr )
    , m_lastHoveredElement( nullptr )
    , m_iScalePc( 200 )
    , m_mouseX(0)
    , m_mouseY(0)
    , m_mouseMoved(false)
{
}

Env::~Env()
{
    clear();
}

void Env::init( gpu::VideoDriver* driver )
{ 
	m_driver = driver; 
}

void Env::draw()
{
    for (auto & child : m_topLevelWidgets)
    {
        child->draw();
    }
}

void Env::clear()
{
    for (auto & elem : m_topLevelWidgets)
    {
        delete elem;
    }
    m_topLevelWidgets.clear();
}

int Env::getMouseX() const { return m_mouseX; }
int Env::getMouseY() const { return m_mouseY; }
int Env::getScreenWidth() const { return m_driver ? m_driver->getScreenWidth() : 0; }
int Env::getScreenHeight() const { return m_driver ? m_driver->getScreenHeight() : 0; }
gpu::VideoDriver* Env::getVideoDriver() { return m_driver; }

void Env::addTopLevelWidget( Widget* widget )
{
    if (!widget) return; // Nothing todo;

    auto it = std::find_if( m_topLevelWidgets.begin(), m_topLevelWidgets.end()
                 , [=](const auto cached) { return cached == widget; });
    if (it != m_topLevelWidgets.end())
    {
        DE_WARN("TopLevelWidget already added ", widget->getId())
        return;
    }

    m_topLevelWidgets.push_back(widget);
}

Widget* Env::pick( int mx, int my )
{
    for (const auto& element : m_topLevelWidgets)
    {
        auto hoverElement = element->pick( mx, my );
        if (hoverElement)
        {
            return hoverElement;
        }
    }
    return nullptr;
}

/*
IElement* Env::pick( int mx, int my )
{
    IElement* hoverElement = nullptr;

    for (IElement* child : m_topLevelWidgets)
    {
        if ( !child->isVisible() ) continue;

        if ( child->getAbsoluteRect().contains( mx, my ) )
        {
            hoverElement = child->pickChild( mx, my );

            if (!hoverElement)
            {
                hoverElement = child;
            }

            break;
        }
    }

    return hoverElement;
}
*/

void Env::onEvent( const Event & event )
{
    // if (event.type == EventType::RESIZE)
    // {
    //     for (auto & child : m_topLevelWidgets)
    //     {
    //         child->onEvent(event);
    //     }
    // }
    // else
    if (event.type == EventType::MOUSE_PRESS)
    {
        //DE_BENNI("MousePressEvent = ", event.mousePressEvent.str())
        if (m_hoveredElement)
        {
            m_hoveredElement->onEvent(event);
        }
    }
    else if (event.type == EventType::MOUSE_RELEASE)
    {
        //DE_BENNI("MouseReleaseEvent = ", event.mouseReleaseEvent.str())
        if (m_hoveredElement)
        {
            m_hoveredElement->onEvent(event);
        }
    }
    else if (event.type == EventType::MOUSE_WHEEL)
    {
        if (m_hoveredElement)
        {
            m_hoveredElement->onEvent(event);
        }
    }
    else if (event.type == EventType::MOUSE_MOVE)
    {
        const int mx = event.mouseMoveEvent.x;
        const int my = event.mouseMoveEvent.y;

        m_mouseX = mx;
        m_mouseY = my;

        m_hoveredElement = pick(mx, my);
        // if (m_hoveredElement)
        // {
        //     auto childElement = m_hoveredElement->pickChild(mx,my);
        //     if (childElement)
        //     {
        //         m_hoveredElement = childElement;
        //     }
        // }

        if (m_hoveredElement != m_lastHoveredElement)
        {
            if (m_lastHoveredElement)
            {
                LeaveEvent e;
                e.m_x = mx;
                e.m_y = my;
                e.m_window = m_lastHoveredElement;
                m_lastHoveredElement->onEvent(e);
            }

            if (m_hoveredElement)
            {
                EnterEvent e;
                e.m_x = mx;
                e.m_y = my;
                e.m_window = m_hoveredElement;
                m_hoveredElement->onEvent(e);
            }

            m_lastHoveredElement = m_hoveredElement;
        }

        if (m_hoveredElement)
        {
            m_hoveredElement->onEvent(event);
        }
    }
}

} // end namespace gui.
} // end namespace de.
