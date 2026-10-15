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

    enum eWidgetFlag
    {
        eWF_None = 0,       // 0000 0000
        eWF_Visible = 1,    // 0000 0001
        eWF_Enabled = 2,    // 0000 0010
        eWF_Hovered = 4,    // 0000 0100
        eWF_Pickable = 8,   // 0000 0100
        eWF_Pressed = 16,   // 0000 1000
        eWF_Released = 32,  // 0001 0000
        eWF_DrawChildren = 64, // 0010 0000
        eWF_FlagMask = 0x1F
    };

    enum eWidgetState
    {
        eWS_Idle = 0,
        eWS_Hovered = 1,
        eWS_Pressed = 2,
        eWS_Released = 3,
    };

    Widget( Env* env, Widget* parent, u32 id, const Recti& r );
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

    bool isEnabled() const { return hasFlags( eWF_Enabled ); }
    void setEnabled( bool enabled ) { setFlags( eWF_Enabled, enabled ); }

    bool isVisible() const { return hasFlags( eWF_Visible ); }
    void setVisible( bool visible ) { setFlags( eWF_Visible, visible ); }

    void setPickable( bool pickable ) { setFlags( eWF_Pickable, pickable ); }
    bool isPickable() const { return hasFlags( eWF_Pickable ); }

    void setDrawChildren( bool enabled ) { setFlags( eWF_DrawChildren, enabled ); }
    bool isDrawChildren() const { return hasFlags( eWF_DrawChildren ); }

    u32 getFlags() const { return m_flags; }
    void setFlags( u32 flags ) { m_flags = flags; }
    bool hasFlags( u32 flags ) const { return (m_flags & flags) == flags; }
    void enableFlags( u32 flags ) { m_flags |= flags; }
    void disableFlags( u32 flags ) { m_flags &= ~flags; }
    void setFlags( u32 flags, bool enable )
    {
        if (enable) { enableFlags(flags); }
        else        { disableFlags(flags); }
    }

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
    u32 m_flags;
    u32 m_id;
    Recti m_rect;
    //Posi m_pos;
    //Sizei m_size;
    //Sizei m_minimumSize;

    Env* m_env;
    Widget* m_parent;
    std::string m_name;

    std::vector< Widget* > m_children;

    Font m_font;

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
