#include <de/gui/Label.h>
#include <de/gui/Env.h>
#include <de/gpu/VideoDriver.h>

namespace de {
namespace gui {

// =====================================================================
Label::Label( std::string text, Env* env, Widget* parent, u32 id, const Recti& r )
// =====================================================================
    : Widget(env, parent, id, r)
    , m_text(text)
{
    // setFillColor(dbRGB(255,255,255), eWS_Idle);
    // setFillColor(dbRGB(255,255,255), eWS_Hovered);
    // setFillColor(dbRGB(255,255,255), eWS_Pressed);
    // setFillColor(dbRGB(255,255,255), eWS_Released);

    // setBorderColor(dbRGB(0,0,0), eWS_Idle);
    // setBorderColor(dbRGB(0,0,0), eWS_Hovered);
    // setBorderColor(dbRGB(0,0,0), eWS_Pressed);
    // setBorderColor(dbRGB(0,0,0), eWS_Released);

    // setTextColor(dbRGB(0,0,0), eWS_Idle);
    // setTextColor(dbRGB(0,0,0), eWS_Hovered);
    // setTextColor(dbRGB(0,0,0), eWS_Pressed);
    // setTextColor(dbRGB(0,0,0), eWS_Released);

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

    setPickable(false);
    setDrawChildren(false);
}

Label::~Label()
{

}

void Label::onEvent( const Event& event)
{
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
}

void Label::draw()
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

    const auto pos = getAbsoluteRect();

    // --- [Draw] Fill ---
    if (dbRGB_A(fillColor) > 0)
    {
        if ( m_radius < 1 )
        {
            driver->getScreenRenderer()->
                draw2DRect( pos, fillColor, m_tex );
        }
        else
        {
            const auto r = glm::ivec2(m_radius,m_radius);
            driver->getScreenRenderer()->
                draw2DRoundRect( pos, r, fillColor, m_tex, 24 );
        }
    }

    // --- [Draw] Text ---
    if (m_text.size())
    {
        const int cx = pos.centerX();
        const int cy = pos.centerY();
        driver->getFontRenderer()->
            draw2DText( cx,cy, m_text, textFgColor, Align::Centered,
                        m_font, textBgColor, 10 );
    }

    // --- [Draw] Border ---
    if (m_borderWidth > 0)
    {
        if ( m_radius < 1 )
        {
            driver->getScreenRenderer()->
                draw2DRectLine( pos, borderColor );
        }
        else
        {
            const auto r = glm::ivec2(m_radius,m_radius);
            driver->getScreenRenderer()->
                draw2DRoundRectLine( pos, r, borderColor, m_tex, 24 );
        }
    }
}

} // end namespace gui.
} // end namespace de.
