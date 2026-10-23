#include <de/gui/Widget.h>
#include <de/gui/Env.h>
#include <de/gpu/VideoDriver.h>

namespace de {
namespace gui {

// =====================================================================
Widget::Widget( Env* env, Widget* parent, u32 id, const Recti& pos )
    // =====================================================================
    : m_env{ env }
    , m_parent{ parent }
    , m_id{ id }
    , m_bVisible{ true }
    , m_bEnabled{ true }
    , m_bHovered{ false }
    , m_bPickable{ false }
    , m_bPressed{ false }
    , m_bReleased{ false }
    , m_bDrawChildren{ true }
    , m_bDrawing{ false }
    , m_rect{ pos }
    , m_font{ "Postamt",20 }
{
    if (!m_env)
    {
        std::ostringstream o;
        o << "No Env given to Widget id " << id;
        throw std::runtime_error(o.str());
    }

    if (m_parent)
    {
        m_parent->addChild( this );
    }
    else
    {
        m_env->addTopLevelWidget(this);
    }
}

Widget::~Widget()
{
    clearChildren();
}

void Widget::resize(int x, int y, int w, int h)
{
    m_rect = Recti(x,y,w,h);

    // for (auto & child : m_children)
    // {
    //     child->resize(x,y,w,h);
    // }
}

void Widget::draw()
{
    if (!isVisible())
    {
        return;
    }

    if (isDrawChildren())
    {
        for (auto child : m_children)
        {
            child->draw();
        }
    }
}

void Widget::onEvent( const Event& event )
{
    if (!isVisible()) return;

    if (event.type == EventType::ENTER)
    {
        //DE_DEBUG(event.str())
        setHovered( true );
    }
    else if (event.type == EventType::LEAVE)
    {
        //DE_DEBUG(event.str())
        setHovered( false );
    }
    else
    {
        for (auto & child : m_children)
        {
            child->onEvent( event );
        }
    }
}

Widget* Widget::pick( int mx, int my )
{
    // Pick nothing if container is invisible, including all children.
    if (!isVisible()) return nullptr;

    // Abort if this widget is not hit at all.
    // (Also clips children, even if they are drawn outside somehow)
    if (!getAbsoluteRect().contains(mx,my))
    {
        return nullptr;
    }

    // DE_DEBUG("Pick(",mx,",",my,") contains(",getAbsoluteRect().str(),")")

    // Loop children, if any, and hit test them...
    for (auto child : m_children)
    {
        auto hoverElement = child->pick(mx,my);
        if (hoverElement)
        {
            return hoverElement;
        }
    }

    // No child was picked, but we are still inside this widget
    // So return this pointer when we are pickable, else nullptr.
    if (isPickable())
    {
        return this;    // A button
    }
    else
    {
        return nullptr; // A panel
    }
}

Recti Widget::getAbsoluteRect() const
{
    Posi pos = m_rect.pos();

    if (m_parent)
    {
        pos += m_parent->getAbsoluteRect().pos();
    }

    return Recti( pos.x, pos.y, m_rect.w, m_rect.h );
}

/*
Posi Widget::getAbsolutePos() const
{
    if (m_parent)
    {
        return m_pos + m_parent->getAbsolutePos();
    }
    else
    {
        return m_pos;
    }
}
*/

void Widget::clearChildren()
{
    for (auto & child : m_children)
    {
        delete child;
    }
    m_children.clear();
}

void Widget::addChild( Widget* child )
{
    if (!child) return; // Nothing todo;

    auto it = std::find_if( m_children.begin(), m_children.end(),
        [=](const auto cached) { return cached == child; });
    if (it != m_children.end())
    {
        DE_WARN("Child already added ", child->getName())
        return;
    }

    m_children.push_back(child);
}

} // end namespace gui.
} // end namespace de.
