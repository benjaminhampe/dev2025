#pragma once
#include <de/gui/Widget.h>

namespace de {
namespace gui {

// =====================================================================
class Label : public Widget
// =====================================================================
{
public:
    Label( std::string text, Env* env, Widget* parent, u32 id, const Recti& r );
    ~Label() override;
    u32 getType() const override { return eWT_Label; }

    void draw() override;
    void onEvent( const Event& event) override;

    const std::string& getText() const { return m_text; }
    void setText( const std::string& text ) { m_text = text; }

    void setTextColors( u32 fg, u32 bg )
    {
        for (u32 & c : m_textFgColors) { c = fg; }
        for (u32 & c : m_textBgColors) { c = bg; }
    }

    void setTextColor( u32 fg, u32 bg, int state )
    {
        m_textFgColors[state] = fg;
        m_textBgColors[state] = bg;
    }

    void setFillColors( u32 color ) { for (u32 & c : m_fillColors) { c = color; } }
    void setFillColor( u32 color, int state ) { m_fillColors[ state ] = color; }

    int getRadius() const { return m_radius; }
    void setRadius( int radius ) { m_radius = radius; }

    void setBorderColors( u32 color ) { for (u32 & c : m_borderColors) { c = color; } }
    void setBorderColor( u32 color, int state ) { m_borderColors[ state ] = color; }

    int getBorderWidth() const { return m_borderWidth; }
    void setBorderWidth( int width ) { m_borderWidth = width; }

    const gpu::TexRef& getTexture() const { return m_tex; }
    void setTexture( gpu::TexRef tex ) { m_tex = tex; }

protected:
    // --- Fill ---
    int m_radius;
    gpu::TexRef m_tex;
    std::array<u32,4> m_fillColors; // Idle,Hovered,Pressed,Released

    // --- Border ---
    int m_borderWidth;
    std::array<u32,4> m_borderColors; // Idle,Hovered,Pressed,Released

    // --- Text ---
    std::string m_text;
    std::array<u32,4> m_textFgColors;
    std::array<u32,4> m_textBgColors;
};

} // end namespace gui.
} // end namespace de.
