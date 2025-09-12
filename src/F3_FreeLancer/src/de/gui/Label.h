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

    virtual const std::string& getText() const
    {
        return m_text;
    }

    virtual void setText( const std::string& text )
    {
        m_text = text;
    }

    virtual void setTextColor( u32 color )
    {
        for (u32 & c : m_textColors) { c = color; }
    }

    virtual void setTextColor( u32 color, int state ) // = eWS_Idle
    {
        m_textColors[state] = color;
    }

protected:
    // --- Text ---
    std::string m_text;
    std::array<u32,4> m_textColors;
};

} // end namespace gui.
} // end namespace de.
