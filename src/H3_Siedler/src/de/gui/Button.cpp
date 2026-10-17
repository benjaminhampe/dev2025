#include <de/gui/Button.h>
#include <de/gui/Env.h>
#include <de/gpu/VideoDriver.h>

namespace de {
namespace gui {

// =====================================================================
Button::Button( std::string text, Env* env, Widget* parent, u32 id, const Recti& r)
// =====================================================================
    : Widget(env, parent, id, r)
    // --- Fill ---
    , m_radius(0)
    // --- Border ---
    , m_borderWidth(0)
    // --- Text ---
    , m_text(text)
    // --- Callbacks ---
    , m_fnOnClicked( [] () {} )
{
    setFillColor(dbRGB( 50, 50, 50), eWS_Idle);
    setFillColor(dbRGB(155,155,155), eWS_Hovered);
    setFillColor(dbRGB(100,100,100), eWS_Pressed);
    setFillColor(dbRGB(105,105,105), eWS_Released);

    setBorderColor(dbRGB(0,0,0), eWS_Idle);
    setBorderColor(dbRGB(0,0,0), eWS_Hovered);
    setBorderColor(dbRGB(0,0,0), eWS_Pressed);
    setBorderColor(dbRGB(0,0,0), eWS_Released);

    setTextColor(dbRGB(220,220,220), dbRGB(20,20,20), eWS_Idle);
    setTextColor(dbRGB(255,255,255), dbRGB(20,20,20), eWS_Hovered);
    setTextColor(dbRGB(100,100,100), dbRGB(20,20,20), eWS_Pressed);
    setTextColor(dbRGB(105,105,105), dbRGB(20,20,20), eWS_Released);

    setPickable(true);
    setDrawChildren(false);
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
    u32 textFgColor = m_textFgColors[eWS_Idle];
    u32 textBgColor = m_textBgColors[eWS_Idle];

    if (hasFlags(eWF_Hovered))
    {
        fillColor = m_fillColors[eWS_Hovered];
        borderColor = m_fillColors[eWS_Hovered];
        textFgColor = m_textFgColors[eWS_Hovered];
        textBgColor = m_textBgColors[eWS_Hovered];
    }
    else if (hasFlags(eWF_Pressed))
    {
        fillColor = m_fillColors[eWS_Pressed];
        borderColor = m_fillColors[eWS_Pressed];
        textFgColor = m_textFgColors[eWS_Pressed];
        textBgColor = m_textBgColors[eWS_Pressed];
    }
    else if (hasFlags(eWF_Released))
    {
        fillColor = m_fillColors[eWS_Released];
        borderColor = m_fillColors[eWS_Released];
        textFgColor = m_textFgColors[eWS_Released];
        textBgColor = m_textBgColors[eWS_Released];
    }

    const Recti pos = getAbsoluteRect();
    // DE_DEBUG("AbsolutePos(",pos.str(),")")

    // --- [Draw] Fill ---
    if (dbRGB_A(fillColor) > 0)
    {
#if 0
        auto r_screen = driver->getScreenQuadRenderer();
        r_screen->draw2D( pos, m_tex, true );
#else
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
#endif
    }

    // --- [Draw] Text ---
    if (m_text.size())
    {
        const int cx = pos.centerX();
        const int cy = pos.centerY();
        auto r_font = driver->getFontRenderer();
        r_font->draw2DText( cx,cy, m_text, textFgColor,
                            Align::Centered, m_font,
                            textBgColor, 5 );
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
