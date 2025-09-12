#include <de/gui/Widget.h>
#include <de/gui/Env.h>
#include <de/gpu/VideoDriver.h>

namespace de {
namespace gui {
    
// =====================================================================
Widget::Widget( Env* env, Widget* parent, u32 id, const Recti& r )
    // =====================================================================
    : m_flags(eWF_Visible | eWF_Enabled)
    , m_id(id)
    , m_pos(r.x,r.y)
    , m_size(r.w,r.h)
    , m_minimumSize(r.w,r.h)
    , m_env(env)
    , m_parent(parent)
    // --- Fill ---
    , m_radius(0)
    , m_tex()
    , m_fillColors{ 0,0,0,0 }
    // --- Border ---
    , m_borderWidth(1)
    , m_borderColors{ 0,0,0,0 }
    // --- Text ---
    , m_font("Postamt",16)
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
        m_env->addTopLevelWidget( this );
    }
}

Widget::~Widget()
{
    clearChildren();
}

void Widget::draw()
{
    if (!isVisible()) return;
    if (!m_env) { DE_ERROR("No env") return; }
    auto driver = m_env->getVideoDriver();
    if (!driver) { DE_ERROR("No driver") return; }
    auto r_screen = driver->getScreenRenderer();

    u32 fillColor = m_fillColors[eWS_Idle];
    u32 borderColor = m_borderColors[eWS_Idle];

    if (hasFlags(eWF_Pressed))
    {
        fillColor = m_fillColors[eWS_Pressed];
        borderColor = m_fillColors[eWS_Pressed];
    }
    else if (hasFlags(eWS_Released))
    {
        fillColor = m_fillColors[eWS_Released];
        borderColor = m_fillColors[eWS_Released];
    }
    else if (hasFlags(eWS_Hovered))
    {
        fillColor = m_fillColors[eWS_Hovered];
        borderColor = m_fillColors[eWS_Hovered];
    }

    const auto pos = getAbsoluteRect();

    // --- [Draw] Fill ---
    if (dbRGBA_A(fillColor) > 0)
    {
        if ( m_radius < 1 )
        {
            r_screen->draw2DRect( pos, fillColor, m_tex );
        }
        else
        {
            const auto r = glm::ivec2(m_radius,m_radius);
            r_screen->draw2DRoundRect( pos, r, fillColor, m_tex, 24 );
        }
    }

    // --- [Draw] Border ---
    if (dbRGBA_A(borderColor) > 0 && m_borderWidth > 0)
    {
        if ( m_radius < 1 )
        {
            r_screen->draw2DRectLine( pos, borderColor );
        }
        else
        {
            const auto r = glm::ivec2(m_radius,m_radius);
            r_screen->draw2DRoundRectLine( pos, r, borderColor, m_tex, 24 );
        }
    }

    // --- [Draw] Child widgets ---
    for (auto & child : m_children)
    {
        child->draw();
    }
}

void Widget::onEvent( const Event& event )
{
    if (!isVisible()) return;

    if (event.type == EventType::ENTER)
    {
        //DE_DEBUG(event.str())
        setFlags( eWS_Hovered, true );
    }
    else if (event.type == EventType::LEAVE)
    {
        //DE_DEBUG(event.str())
        setFlags( eWS_Hovered, false );
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
        return this;
    }
    else
    {
        return nullptr;
    }
}

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

/*
Recti Widget::getAbsolutePos() const
{
    if (m_parent)
    {
        const Recti r_parent = m_parent->getAbsolutePos();

        auto pos = Recti( m_pos.x + r_parent.x,
                         m_pos.y + r_parent.y,
                         m_pos.w,
                         m_pos.h);

// Clipping
#ifdef USE_BENNI_CLIPPING
        if (pos.x2() > pp.x2() )
        {
            pos.w = pos.x - pp.x2() + 1;
            if (pos.w < 0) pos.w = 0;
        }

        if (pos.y2() > pp.y2() )
        {
            pos.h = pos.y - pp.y2() + 1;
            if (pos.h < 0) pos.h = 0;
        }
#endif
        return pos;
    }
    else
    {
        return m_pos;
    }
}*/


} // end namespace gui.
} // end namespace de.
