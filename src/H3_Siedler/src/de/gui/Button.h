#pragma once
#include <de/gui/Widget.h>

namespace de {
namespace gui {

// =====================================================================
class Button : public Widget
// =====================================================================
{
public:
    Button( std::string text, Env* env, Widget* parent, u32 id, const Recti& r );
    ~Button() override;
    u32 getType() const override { return eWT_Button; }

    void onEvent( const Event& event ) override;
    void draw() override;

    const std::string& getText() const { return m_text; }
    void setText( const std::string& text ) { m_text = text; }

    void setTextColor( u32 color ) { for (u32 & c : m_textColors) { c = color; } }
    void setTextColor( u32 color, int state ) { m_textColors[state] = color; }

    void setFillColor( u32 color ) { for (u32 & c : m_fillColors) { c = color; } }
    void setFillColor( u32 color, int state ) { m_fillColors[ state ] = color; }

    int getRadius() const { return m_radius; }
    void setRadius( int radius ) { m_radius = radius; }

    void setBorderColor( u32 color ) { for (u32 & c : m_borderColors) { c = color; } }
    void setBorderColor( u32 color, int state ) { m_borderColors[ state ] = color; }

    int getBorderWidth() const { return m_borderWidth; }
    void setBorderWidth( int width ) { m_borderWidth = width; }

    const gpu::TexRef& getTexture() const { return m_tex; }
    void setTexture( gpu::TexRef tex ) { m_tex = tex; }


    void setOnClicked(const std::function<void(void)> & fnOnClicked )
    {
        m_fnOnClicked = fnOnClicked;
    }

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
    std::array<u32,4> m_textColors;

    // --- Callbacks ---
    std::function<void(void)> m_fnOnClicked;

};

} // end namespace gui.
} // end namespace de.
