#include <de/gui/Panel.h>
#include <de/gui/Env.h>
#include <de/gpu/VideoDriver.h>

namespace de {
namespace gui {

// =====================================================================
Panel::Panel( Env* env, Widget* parent, u32 id, const Recti& r )
    // =====================================================================
    : Widget(env,parent,id,r)
    // --- Fill ---
    , m_radius(0)
    , m_tex()
    , m_fillColors{ 0,0,0,0 }
    // --- Border ---
    , m_borderWidth(1)
    , m_borderColors{ 0,0,0,0 }
{
    setFillColor(dbRGB(255,255,255), eWS_Idle);
    setFillColor(dbRGB(255,255,255), eWS_Hovered);
    setFillColor(dbRGB(255,255,255), eWS_Pressed);
    setFillColor(dbRGB(255,255,255), eWS_Released);

    setBorderColor(dbRGB(0,0,0), eWS_Idle);
    setBorderColor(dbRGB(0,0,0), eWS_Hovered);
    setBorderColor(dbRGB(0,0,0), eWS_Pressed);
    setBorderColor(dbRGB(0,0,0), eWS_Released);

    // setTextColor(dbRGB(0,0,0), eWS_Idle);
    // setTextColor(dbRGB(0,0,0), eWS_Hovered);
    // setTextColor(dbRGB(0,0,0), eWS_Pressed);
    // setTextColor(dbRGB(0,0,0), eWS_Released);

    setPickable(false);
}

Panel::~Panel()
{

}

void Panel::draw()
{
    if (!isVisible()) return;
    if (!m_env) { DE_ERROR("No env") return; }
    auto driver = m_env->getVideoDriver();
    if (!driver) { DE_ERROR("No driver") return; }

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
    if (dbRGB_A(fillColor) > 0)
    {
        auto r_screen = driver->getScreenRenderer();
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
    if (dbRGB_A(borderColor) > 0 && m_borderWidth > 0)
    {
        auto r_screen = driver->getScreenRenderer();
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

    if (isDrawChildren())
    {
        for (auto child : m_children)
        {
            child->draw();
        }
    }
}

void Panel::onEvent( const Event& event )
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

Widget* Panel::pick( int mx, int my )
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

} // end namespace gui.
} // end namespace de.
