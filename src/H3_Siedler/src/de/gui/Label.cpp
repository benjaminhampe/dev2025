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

} // end namespace gui.
} // end namespace de.
