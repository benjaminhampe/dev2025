#include <de/gui/Button.h>
#include <de/gui/Env.h>
#include <de/gpu/VideoDriver.h>

namespace de {
namespace gui {
    
// =====================================================================
Button::Button( std::string text, Env* env, Widget* parent, u32 id, const Recti& r)
// =====================================================================
    : Widget(env, parent, id, r)
    , m_text(text)
    , m_fnOnClicked( [] () {} )
{
    setPickable(true);

    setFillColor(0xFFFFFFFF, eWS_Idle);
    setFillColor(0xFFFFFFFF, eWS_Hovered);
    setFillColor(0xFFFFFFFF, eWS_Pressed);
    setFillColor(0xFFFFFFFF, eWS_Released);

    setBorderColor(0x00000000, eWS_Idle);
    setBorderColor(0x00000000, eWS_Hovered);
    setBorderColor(0x00000000, eWS_Pressed);
    setBorderColor(0x00000000, eWS_Released);

    setTextColor(dbRGBA(0,0,0), eWS_Idle);
    setTextColor(dbRGBA(0,0,0), eWS_Hovered);
    setTextColor(dbRGBA(0,0,0), eWS_Pressed);
    setTextColor(dbRGBA(0,0,0), eWS_Released);
}

Button::~Button()
{
    
}


void
Button::draw()
{
    if (!isVisible()) return;
    if (!m_env) { DE_ERROR("No env") return; }
    auto driver = m_env->getVideoDriver();
    if (!driver) { DE_ERROR("No driver") return; }

    u32 fillColor = m_fillColors[eWS_Idle];
    u32 borderColor = m_borderColors[eWS_Idle];
    u32 textColor = m_textColors[eWS_Idle];

    if (hasFlags(eWF_Pressed))
    {
        fillColor = m_fillColors[eWS_Pressed];
        borderColor = m_fillColors[eWS_Pressed];
        textColor = m_textColors[eWS_Pressed];
    }
    else if (hasFlags(eWF_Released))
    {
        fillColor = m_fillColors[eWS_Released];
        borderColor = m_fillColors[eWS_Released];
        textColor = m_textColors[eWS_Released];
    }
    else if (hasFlags(eWF_Hovered))
    {
        fillColor = m_fillColors[eWS_Hovered];
        borderColor = m_fillColors[eWS_Hovered];
        textColor = m_textColors[eWS_Hovered];
    }

    const auto pos = getAbsoluteRect();

    // --- [Draw] Fill ---
    if (dbRGBA_A(fillColor) > 0)
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


    // --- [Draw] Children ---
    for (auto & child : m_children)
    {
        child->draw();
    }

    // --- [Draw] Text ---
    if (dbRGBA_A(textColor) > 0 && !m_text.empty())
    {
        const int cx = pos.centerX();
        const int cy = pos.centerY();
        auto r_font = driver->getFontRenderer();
        r_font->draw2DText( cx,cy, m_text, textColor,
                            Align::Centered, m_font,
                            0x00000000, 10 );
    }

    // --- [Draw] Border ---
    if (dbRGBA_A(borderColor) > 0 && m_borderWidth > 0)
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
}

void Button::onEvent( const Event& event )
{
    if (event.type == EventType::ENTER)
    {
        //if (event.enterEvent.m_window == this)
        //{
            //DE_DEBUG(event.str())
            setFlags( eWF_Hovered, true );
            setFlags( eWF_Pressed, false );
            setFlags( eWF_Released, false );
        //}
    }
    else if (event.type == EventType::LEAVE)
    {
        //if (event.leaveEvent.m_window == this)
        //{
            //DE_DEBUG(event.str())
            setFlags( eWF_Hovered, false );
            setFlags( eWF_Pressed, false );
            setFlags( eWF_Released, false );
        //}
    }
    else if (event.type == EventType::MOUSE_PRESS)
    {
        if (hasFlags(eWF_Hovered))
        {
            setFlags( eWF_Pressed, true );
            setFlags( eWF_Released, false );
        }
    }
    else if (event.type == EventType::MOUSE_RELEASE)
    {
        if (hasFlags(eWF_Hovered))
        {
            m_fnOnClicked();
            setFlags( eWF_Pressed, false );
            setFlags( eWF_Released, true );
        }
        else
        {
            setFlags( eWF_Pressed, false );
            setFlags( eWF_Released, false );
        }
    }
}


/*
void
Button::resizeEvent( const ResizeEvent& event )
{
}

void
Button::paintEvent( const PaintEvent& event )
{
}

void
Button::enterEvent( EnterEvent const & event )
{
    //DE_DEBUG(event.str())
    m_hovered = true;
    m_pressed = false;
    m_released = false;
}

void
Button::leaveEvent( LeaveEvent const & event )
{
    //DE_DEBUG(event.str())
    m_hovered = false;
    m_pressed = false;
    m_released = false;
}


void
Button::mouseButtonEvent( MouseButtonEvent const & event )
{
    //DE_DEBUG(event.str())
    if (event.isPressed())
    {
        m_pressed = true;
        m_released = false;
    }
    else if (event.isReleased())
    {
        m_fnOnClicked();
        m_pressed = false;
        m_released = true;
    }
}


void
Button::mousePressEvent( MousePressEvent const & event )
{
    m_pressed = true;
    m_released = false;
}

void
Button::mouseReleaseEvent( MouseReleaseEvent const & event )
{
    m_fnOnClicked();
    m_pressed = false;
    m_released = true;
}
*/

} // end namespace gui.
} // end namespace de.
