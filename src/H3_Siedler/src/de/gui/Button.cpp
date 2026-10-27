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
{
    auto black = dbRGB(0,0,0);
    auto white = dbRGB(255,255,255);
    setFillColor(white, eWS_Idle);
    setFillColor(white, eWS_Hovered);
    setFillColor(white, eWS_Pressed);
    setFillColor(white, eWS_Released);

    setBorderColor(white, eWS_Idle);
    setBorderColor(white, eWS_Hovered);
    setBorderColor(white, eWS_Pressed);
    setBorderColor(white, eWS_Released);

    auto white2 = dbRGB(255,255,255,155);
    setTextColor(black, white2, eWS_Idle);
    setTextColor(black, white, eWS_Hovered);
    setTextColor(black, white, eWS_Pressed);
    setTextColor(black, white, eWS_Released);

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

    int tesselation = 24;

    u32 fillColor = m_fillColors[eWS_Idle];
    u32 borderColor = m_borderColors[eWS_Idle];
    u32 textFgColor = m_textFgColors[eWS_Idle];
    u32 textBgColor = m_textBgColors[eWS_Idle];

    if (isHovered())
    {
        fillColor = m_fillColors[eWS_Hovered];
        borderColor = m_fillColors[eWS_Hovered];
        textFgColor = m_textFgColors[eWS_Hovered];
        textBgColor = m_textBgColors[eWS_Hovered];
    }
    else if (isPressed())
    {
        fillColor = m_fillColors[eWS_Pressed];
        borderColor = m_fillColors[eWS_Pressed];
        textFgColor = m_textFgColors[eWS_Pressed];
        textBgColor = m_textBgColors[eWS_Pressed];
    }
    else if (isReleased())
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
            r_screen->draw2DRoundRect( pos, r, fillColor, m_tex, tesselation );
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
            r_screen->draw2DRoundRectLine( pos, r, borderColor, m_tex, tesselation );
        }
    }


    // --- [Draw] Disabled Overlay RoundRect/Rect ---
    if (!isEnabled())
    {
        uint32_t disabledColor = dbRGB(130,130,130,180);
        if ( m_radius < 1 )
        {
            driver->getScreenRenderer()->
                draw2DRect( pos, disabledColor, m_tex );
        }
        else
        {
            const auto r = glm::ivec2(m_radius,m_radius);
            driver->getScreenRenderer()->
                draw2DRoundRect( pos, r, disabledColor, de::gpu::TexRef(), tesselation );
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
            setHovered( true );
            setPressed( false );
            setReleased( false );
        //}
    }
    else if (event.type == EventType::LEAVE)
    {
        //if (event.leaveEvent.m_window == this)
        //{
            //DE_DEBUG(event.str())
            setHovered( false );
            setPressed( false );
            setReleased( false );
        //}
    }
    else if (event.type == EventType::MOUSE_PRESS)
    {
        if (isHovered())
        {
            setPressed( true );
            setReleased( false );
        }
    }
    else if (event.type == EventType::MOUSE_RELEASE)
    {
        if (isHovered())
        {
            setPressed( false );
            setReleased( true );

            // We love the clickEvents and decide action in callback
            // if (isEnabled())
            // {
                if (onClicked)
                {
                    onClicked();
                }

                if (event.mouseReleaseEvent.isLeft())
                {
                    if (onClickedLeft) { onClickedLeft(); }
                }
                else if (event.mouseReleaseEvent.isRight())
                {
                    if (onClickedRight) { onClickedRight(); }
                }
                else if (event.mouseReleaseEvent.isMiddle())
                {
                    if (onClickedMiddle) { onClickedMiddle(); }
                }
            // }
        }
        else
        {
            setPressed( false );
            setReleased( false );
        }
    }
}


} // end namespace gui.
} // end namespace de.
