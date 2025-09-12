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

    virtual void setOnClicked(const std::function<void(void)> & fnOnClicked )
    {
        m_fnOnClicked = fnOnClicked;
    }

/*
    void resizeEvent( const ResizeEvent& event ) override;
    void paintEvent( const PaintEvent& event ) override;
    void enterEvent( const EnterEvent& event ) override;
    void leaveEvent( const LeaveEvent& event ) override;
    //void mouseButtonEvent( const MouseButtonEvent& event ) override;
    void mousePressEvent( const MousePressEvent& event ) override;
    void mouseReleaseEvent( const MouseReleaseEvent& event ) override;
    //void showEvent( ShowEvent const & event ) override;
    //void hideEvent( HideEvent const & event ) override;
    //void focusEvent( FocusEvent const & event ) override;
    //void keyEvent( KeyEvent const & event ) override;
    //void mouseMoveEvent( MouseMoveEvent const & event ) override;
    //void mouseWheelEvent( MouseWheelEvent const & event ) override;
*/
protected:
    // --- Text ---
    std::string m_text;
    std::array<u32,4> m_textColors;

    // --- Callbacks ---
    std::function<void(void)> m_fnOnClicked;




};

} // end namespace gui.
} // end namespace de.
