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

    virtual void draw();
    virtual void onEvent( const Event& event );
    virtual Posi getAbsolutePos() const;

    virtual Widget* pick( int mx, int my );
    virtual void clearChildren();
    virtual void addChild( Widget* child );

    // Inline impl.

    virtual u32 getId() const { return m_id; }
    virtual void setId( u32 id ) { m_id = id; }

    virtual u32 getFlags() const { return m_flags; }
    virtual void setFlags( u32 flags ) { m_flags = flags; }
    virtual bool hasFlags( u32 flags ) const { return (m_flags & flags) == flags; }
    virtual void enableFlags( u32 flags ) { m_flags |= flags; }
    virtual void disableFlags( u32 flags ) { m_flags &= ~flags; }
    virtual void setFlags( u32 flags, bool enable )
    {
        if (enable) { enableFlags(flags); }
        else        { disableFlags(flags); }
    }

    virtual const std::string & getName() const { return m_name; }
    virtual void setName( const std::string & name ) { m_name = name; }

    virtual bool isVisible() const { return hasFlags( eWF_Visible ); }
    virtual void setVisible( bool visible ) { setFlags( eWF_Visible, visible ); }

    virtual bool isEnabled() const { return hasFlags( eWF_Enabled ); }
    virtual void setEnabled( bool enabled ) { setFlags( eWF_Enabled, enabled ); }

    virtual void setPickable( bool pickable ) { setFlags( eWF_Pickable, pickable ); }
    virtual bool isPickable() const { return hasFlags( eWF_Pickable ); }

    virtual Widget* getParent() { return m_parent; }
    virtual void setParent( Widget* parent ) { m_parent = parent; }

    virtual const Posi& getPos() const { return m_pos; }
    virtual void setPos( const Posi& pos ) { m_pos = pos; }
    virtual void setPos( int x, int y ) { setPos( Posi(x,y) ); }

    virtual const Sizei& getSize() const { return m_size; }
    virtual void setSize( const Sizei& size ) { m_size = size; }
    virtual void setSize( int w, int h ) { setSize( Sizei(w,h) ); }

    virtual const Sizei& getMinimumSize() const { return m_minimumSize; }
    virtual void setMinimumSize( const Sizei& size ) { m_minimumSize = size; }
    virtual void setMinimumSize( int w, int h ) { setMinimumSize( Sizei(w,h) ); }

    virtual Recti getRect() const { return Recti( getPos(), getSize() ); }
    virtual void setRect( const Recti& r ) { setPos(r.pos()); setSize(r.size()); }
    virtual void setRect( int x, int y, int w, int h ) { setRect( {x,y,w,h} ); }

    virtual Recti getAbsoluteRect() const
    {
        const auto& p = getAbsolutePos();
        const auto& s = getSize();
        return Recti( p.x, p.y, s.w, s.h );
    }

    virtual const Font& getFont() const { return m_font; }
    virtual void setFont( const Font& font )
    {
        m_font = font;
        for ( auto & child : m_children)
        {
            child->setFont( font );
        }
    }

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

protected:
    u32 m_flags;
    u32 m_id;
    Posi m_pos;
    Sizei m_size;
    Sizei m_minimumSize;

    Env* m_env;
    Widget* m_parent;
    std::string m_name;

    // --- Fill ---
    int m_radius;
    gpu::TexRef m_tex;
    std::array<u32,4> m_fillColors; // Idle,Hovered,Pressed,Released

    // --- Border ---
    int m_borderWidth;
    std::array<u32,4> m_borderColors; // Idle,Hovered,Pressed,Released

    // --- Text ---
    Font m_font;
    std::vector< Widget* > m_children;
};

} // end namespace gui.
} // end namespace de.
