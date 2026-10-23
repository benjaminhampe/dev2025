#pragma once
#include <de/os/Events.h>
#include <de/gpu/GPU.h>
#include <de/image/font/Font.h>

namespace de {
namespace gui {

class Env;

// =====================================================================
class Widget
// =====================================================================
{
public:
    enum eWidgetType
    {
        eWT_Widget = 0,
        eWT_Panel,
        eWT_Label,
        eWT_Button,
        eWT_IntField,
        eWT_Dialog,
        eWT_Slider,
        eWT_Edit,
    };

    enum eWidgetState
    {
        eWS_Idle = 0,
        eWS_Hovered = 1,
        eWS_Pressed = 2,
        eWS_Released = 3,
    };

    Widget( Env* env, Widget* parent = nullptr, u32 id = 0, const Recti& r = de::Recti() );
    virtual ~Widget();
    virtual u32 getType() const { return eWT_Widget; }
    virtual void resize(int x, int y, int w, int h);
    virtual void draw();

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
    virtual void onEvent( const Event& event );
    virtual Widget* pick( int mx, int my );

    Recti getAbsoluteRect() const;
    const Recti& getRect() const { return m_rect; }
    void clearChildren();
    void addChild( Widget* child );

    Widget* getParent() { return m_parent; }
    void setParent( Widget* parent ) { m_parent = parent; }

    u32 getId() const { return m_id; }
    void setId( u32 id ) { m_id = id; }

    const std::string& getName() const { return m_name; }
    void setName( const std::string & name ) { m_name = name; }

/*
    enum eWidgetFlag
    {
        eWF_None = 0,       // 0000 0000
        eWF_Visible = 1,    // 0000 0001
        eWF_Enabled = 2,    // 0000 0010
        eWF_Hovered = 4,    // 0000 0100
        eWF_Pickable = 8,   // 0000 1000
        eWF_Pressed = 16,   // 0001 0000
        eWF_Released = 32,  // 0010 0000
        eWF_DrawChildren = 64, // 0100 0000
        eWF_Drawing = 128,  // 1000 0000
        eWF_FlagMask = 0x7F
    };
*/
    bool isVisible() const { return m_bVisible; }
    void setVisible( bool visible ) { m_bVisible = visible; }

    bool isEnabled() const { return m_bEnabled; }
    void setEnabled( bool enabled ) { m_bEnabled = enabled; }

    bool isHovered() const { return m_bHovered; }
    void setHovered( bool hovered ) { m_bHovered = hovered; }

    bool isPickable() const { return m_bPickable; }
    void setPickable( bool pickable ) { m_bPickable = pickable; }

    bool isPressed() const { return m_bPressed; }
    void setPressed( bool pressed ) { m_bPressed = pressed; }

    bool isReleased() const { return m_bReleased; }
    void setReleased( bool released ) { m_bReleased = released; }

    bool isDrawChildren() const { return m_bDrawChildren; }
    void setDrawChildren( bool drawing ) { m_bDrawChildren = drawing; }

    bool isDrawing() const { return m_bDrawing; }
    void setDrawing( bool drawing ) { m_bDrawing = drawing; }

    const Font& getFont() const { return m_font; }

    virtual void setFont( const Font& font )
    {
        m_font = font;
        for ( auto & child : m_children)
        {
            child->setFont( font );
        }
    }

/*
    virtual Recti getRect() const { return Recti( getPos(), getSize() ); }
    virtual void setRect( const Recti& r ) { setPos(r.pos()); setSize(r.size()); }
    virtual void setRect( int x, int y, int w, int h ) { setRect( {x,y,w,h} ); }

    virtual Posi getAbsolutePos() const;

    virtual const Posi& getPos() const { return m_pos; }
    virtual void setPos( const Posi& pos ) { m_pos = pos; }
    virtual void setPos( int x, int y ) { setPos( Posi(x,y) ); }

    virtual const Sizei& getSize() const { return m_size; }
    virtual void setSize( const Sizei& size ) { m_size = size; }
    virtual void setSize( int w, int h ) { setSize( Sizei(w,h) ); }

    virtual const Sizei& getMinimumSize() const { return m_minimumSize; }
    virtual void setMinimumSize( const Sizei& size ) { m_minimumSize = size; }
    virtual void setMinimumSize( int w, int h ) { setMinimumSize( Sizei(w,h) ); }

*/


/*

    virtual void setFillColor( u32 color )
    {
        for (u32 & c : m_fillColors) { c = color; }
    }

    virtual void setFillColor( u32 color, int state ) // = eWS_Idle
    {
        m_fillColors[ state ] = color;
    }

    virtual int getRadius() const { return m_radius; }
    virtual void setRadius( int radius ) { m_radius = radius; }

    virtual void setBorderColor( u32 color )
    {
        for (u32 & c : m_borderColors) { c = color; }
    }

    virtual void setBorderColor( u32 color, int state ) // = eWS_Idle
    {
        m_borderColors[ state ] = color;
    }

    virtual int getBorderWidth() const { return m_borderWidth; }
    virtual void setBorderWidth( int width ) { m_borderWidth = width; }

    virtual const gpu::TexRef& getTexture() const { return m_tex; }
    virtual void setTexture( gpu::TexRef tex ) { m_tex = tex; }
*/

protected:
    Env* m_env;
    Widget* m_parent;
    u32 m_id;
    bool m_bVisible;
    bool m_bEnabled;
    bool m_bHovered;
    bool m_bPickable;
    bool m_bPressed;
    bool m_bReleased;
    bool m_bDrawChildren;
    bool m_bDrawing;
    Recti m_rect;
    Font m_font;
    std::string m_name;
    std::vector< Widget* > m_children;

/*
    // --- Fill ---
    int m_radius;
    gpu::TexRef m_tex;
    std::array<u32,4> m_fillColors; // Idle,Hovered,Pressed,Released

    // --- Border ---
    int m_borderWidth;
    std::array<u32,4> m_borderColors; // Idle,Hovered,Pressed,Released

    // --- Text ---


*/
};

} // end namespace gui.
} // end namespace de.
