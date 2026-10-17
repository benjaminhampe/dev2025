#pragma once
#include <de/gui/Widget.h>

namespace de {
namespace gui {

class Env;

// =====================================================================
class Panel : public Widget
// =====================================================================
{
public:
    Panel( Env* env, Widget* parent, u32 id, const Recti& r );
    ~Panel() override;

    u32 getType() const override { return eWT_Panel; }

    void draw() override;

    void onEvent( const Event& event ) override;

    Widget* pick( int mx, int my ) override;

    // --- Panel specific ---

    void setFillColor( u32 color )
    {
        for (u32 & c : m_fillColors) { c = color; }
    }

    void setFillColor( u32 color, int state ) // = eWS_Idle
    {
        m_fillColors[ state ] = color;
    }

    int getRadius() const { return m_radius; }
    void setRadius( int radius ) { m_radius = radius; }

    void setBorderColor( u32 color )
    {
        for (u32 & c : m_borderColors) { c = color; }
    }

    void setBorderColor( u32 color, int state ) // = eWS_Idle
    {
        m_borderColors[ state ] = color;
    }

    int getBorderWidth() const { return m_borderWidth; }
    void setBorderWidth( int width ) { m_borderWidth = width; }

    const gpu::TexRef& getTexture() const { return m_tex; }
    void setTexture( gpu::TexRef tex ) { m_tex = tex; }

protected:
    // u32 m_flags;
    // u32 m_id;
    // Posi m_pos;
    // Sizei m_size;
    // Sizei m_minimumSize;

    // Env* m_env;
    // Panel* m_parent;
    // std::string m_name;
    // std::vector< Panel* > m_children;

    // --- Fill ---
    int m_radius;
    gpu::TexRef m_tex;
    std::array<u32,4> m_fillColors; // Idle,Hovered,Pressed,Released

    // --- Border ---
    int m_borderWidth;
    std::array<u32,4> m_borderColors; // Idle,Hovered,Pressed,Released
};

} // end namespace gui.
} // end namespace de.
